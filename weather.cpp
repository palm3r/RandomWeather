#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_no_case.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <random>
#include <string>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <sdkddkver.h>
#include <Windows.h>

#include "weather.h"
#include "perlin.h"
#include "misc.h"

using namespace boost::spirit;

template <typename Iterator>
struct grammar : public qi::grammar<Iterator, double(), ascii::space_type>
{
	qi::rule<Iterator, double(), ascii::space_type> expr, term, fctr;
	double time;
	grammar(double t) : grammar::base_type(expr), time(t)
	{
		expr = term[_val = _1] >> *(('+' >> term[_val += _1]) | ('-' >> term[_val -= _1]));
		term = fctr[_val = _1] >> *(('*' >> fctr[_val *= _1]) | ('/' >> fctr[_val /= _1]));
		fctr = double_[_val = _1]
			| '(' >> expr[_val = _1] >> ')'
			| qi::no_case[qi::lit('t')][_val = time]
			| qi::no_case["cloud("] >> expr[_val = boost::phoenix::bind(cloud_, _1)] >> ")"
			| qi::no_case["rain("] >> expr[_val = boost::phoenix::bind(rain_, _1)] >> ")"
			| qi::no_case["wind_speed("] >> expr[_val = boost::phoenix::bind(wind_speed_, _1)] >> ")"
			| qi::no_case["air_temp("] >> expr[_val = boost::phoenix::bind(air_temp_, _1)] >> ")"
			;
	}
};

double rain_chance_;
double min_air_temp_;
double max_air_temp_;

perlin::noise cloud_;
perlin::noise rain_;
perlin::noise wind_speed_;
perlin::noise air_temp_;

std::string cloud_expr_;
std::string rain_expr_;
std::string wind_speed_expr_;
std::string air_temp_expr_;

std::random_device rd_;
std::mt19937 mt_;
bool rainfall_ = false;
bool stoprain_ = false;

void weather_init(unsigned int seed, weather_config *config)
{
	if (seed == 0)
		seed = rd_();
	mt_.seed(seed);

	if (config != nullptr)
	{
		rain_chance_ = config->rain_chance;
		min_air_temp_ = config->min_air_temp;
		max_air_temp_ = config->max_air_temp;

		cloud_.set_seed(mt_());
		cloud_.set_zero(config->cloud.zero);
		cloud_.set_amplitude(config->cloud.amplitude);
		cloud_.set_min_value(config->cloud.min);
		cloud_.set_max_value(config->cloud.max);
		cloud_.set_frequency(1.0 / config->cloud.period);
		cloud_.set_octave(config->cloud.octave);
		cloud_expr_ = config->cloud.expression != nullptr
			? config->cloud.expression : "cloud(T)";

		rain_.set_seed(mt_());
		rain_.set_zero(config->rain.zero);
		rain_.set_amplitude(config->rain.amplitude);
		rain_.set_min_value(config->rain.min);
		rain_.set_max_value(config->rain.max);
		rain_.set_frequency(1.0 / config->rain.period);
		rain_.set_octave(config->rain.octave);
		rain_expr_ = config->rain.expression != nullptr
			? config->rain.expression : "rain(T)";

		wind_speed_.set_seed(mt_());
		wind_speed_.set_zero(config->wind_speed.zero);
		wind_speed_.set_amplitude(config->wind_speed.amplitude);
		wind_speed_.set_min_value(config->wind_speed.min);
		wind_speed_.set_max_value(config->wind_speed.max);
		wind_speed_.set_frequency(1.0 / config->wind_speed.period);
		wind_speed_.set_octave(config->wind_speed.octave);
		wind_speed_expr_ = config->wind_speed.expression != nullptr
			? config->wind_speed.expression : "wind_speed(T)";

		air_temp_.set_seed(mt_());
		air_temp_.set_zero(config->air_temp.zero);
		air_temp_.set_amplitude(config->air_temp.amplitude);
		air_temp_.set_min_value(config->air_temp.min);
		air_temp_.set_max_value(config->air_temp.max);
		air_temp_.set_frequency(1.0 / config->air_temp.period);
		air_temp_.set_octave(config->air_temp.octave);
		air_temp_expr_ = config->air_temp.expression != nullptr
			? config->air_temp.expression : "air_temp(T)";
	}
}

void weather_simulate(double time, weather_info *info)
{
	double c = 0, r = 0, w = 0, a = 0;
	grammar<std::string::iterator> g(time);
	qi::phrase_parse(cloud_expr_.begin(), cloud_expr_.end(), g, ascii::space, c);
	qi::phrase_parse(rain_expr_.begin(), rain_expr_.end(), g, ascii::space, r);
	qi::phrase_parse(wind_speed_expr_.begin(), wind_speed_expr_.end(), g, ascii::space, w);
	qi::phrase_parse(air_temp_expr_.begin(), air_temp_expr_.end(), g, ascii::space, a);

	r = clamp<double>(r, rain_.min_value(), c);
	a *= (max_air_temp_ - min_air_temp_) / 2;
	a += min_air_temp_ + (max_air_temp_ - min_air_temp_) / 2;

	if (!rainfall_)
	{
		if (0 < r)
		{
			if (!stoprain_)
			{
				std::uniform_real_distribution<double> chance(0, 1);
				if (rain_chance_ < chance(mt_))
				{
					stoprain_ = true;
				}
			}
		}
		else
		{
			stoprain_ = false;
		}
	}

	r = stoprain_ ? 0 : r;
	rainfall_ = 0 < r;

	info->cloud = c;
	info->rain = r;
	info->air_temp = a;
	info->wind_speed = w;
}
