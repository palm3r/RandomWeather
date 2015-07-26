#include "plugin.h"
#include "weather.h"
#include "ini.h"

#include <string>
#include <deque>
#include <sstream>
#include <iomanip>

extern "C" {

	__declspec(dllexport)
	const char* GetPluginName()
	{
		return "RandomWeather";
	}

	__declspec(dllexport)
	PluginObjectType GetPluginType()
	{
		return PluginObjectType::PO_INTERNALS;
	}

	__declspec(dllexport)
	int GetPluginVersion()
	{
		return 6;
	}

	__declspec(dllexport)
		PluginObject* CreatePluginObject()
	{
		return new plugin();
	}

	__declspec(dllexport)
		void DestroyPluginObject(PluginObject *obj)
	{
		auto p = static_cast<plugin*>(obj);
		delete p;
	}

}

using namespace std;

double speed = 1;
bool session_started = false;
double base_time = 0;
double session_duration = 0;
double margin = 0, max_margin = 180;

void plugin::OnStartup(long version)
{
	weather_config config;
	memset(&config, 0, sizeof(config));

	// weather
	speed = 60.0 / Config<double>("weather", "event_duration", 60);
	config.rain_chance = Config<double>("weather", "rain_chance", 30) / 100.0;
	config.min_air_temp = Config<double>("weather", "min_air_temp", 20);
	config.max_air_temp = Config<double>("weather", "max_air_temp", 30);

	// cloud
	config.cloud.zero = Config<double>("cloud", "zero", 0);
	config.cloud.amplitude = Config<double>("cloud", "amplitude", 1);
	config.cloud.period = Config<double>("cloud", "period", 3600);
	config.cloud.octave = Config<int>("cloud", "octave", 2);
	config.cloud.min = Config<double>("cloud", "min", -1);
	config.cloud.max = Config<double>("cloud", "max", 1);
	auto cloud_expr = Config<std::string>("cloud", "expression", "cloud(T)");
	config.cloud.expression = cloud_expr.c_str();

	// rain
	config.rain.zero = Config<double>("rain", "zero", 0);
	config.rain.amplitude = Config<double>("rain", "amplitude", 1);
	config.rain.period = Config<double>("rain", "period", 3600);
	config.rain.octave = Config<int>("rain", "octave", 2);
	config.rain.min = Config<double>("rain", "min", -1);
	config.rain.max = Config<double>("rain", "max", 1);
	auto rain_expr = Config<std::string>("rain", "expression", "rain(T)");
	config.rain.expression = rain_expr.c_str();

	// wind_speed
	config.wind_speed.zero = Config<double>("wind_speed", "zero", 0);
	config.wind_speed.amplitude = Config<double>("wind_speed", "amplitude", 1);
	config.wind_speed.period = Config<double>("wind_speed", "period", 3600);
	config.wind_speed.octave = Config<int>("wind_speed", "octave", 2);
	config.wind_speed.min = Config<double>("wind_speed", "min", -1);
	config.wind_speed.max = Config<double>("wind_speed", "max", 1);
	auto wind_speed_expr = Config<std::string>("wind_speed", "expression", "wind_speed(T)");
	config.wind_speed.expression = wind_speed_expr.c_str();

	// air_temp
	config.air_temp.zero = Config<double>("air_temp", "zero", 0);
	config.air_temp.amplitude = Config<double>("air_temp", "amplitude", 1);
	config.air_temp.period = Config<double>("air_temp", "period", 3600);
	config.air_temp.octave = Config<int>("air_temp", "octave", 2);
	config.air_temp.min = Config<double>("air_temp", "min", -1);
	config.air_temp.max = Config<double>("air_temp", "max", 1);
	auto air_temp_expr = Config<std::string>("air_temp", "expression", "air_temp(T)");
	config.air_temp.expression = air_temp_expr.c_str();

	weather_init(0, &config);
}

void plugin::OnLoad()
{
	base_time = 0;
	margin = 0;
}

void plugin::OnStartSession()
{
	session_started = true;
}

bool plugin::OnWantsScoringUpdates()
{
	return true;
}

void plugin::OnUpdateScoring(const ScoringInfoV01 &info)
{
	if (session_started)
	{
		session_duration = info.mEndET;
		session_started = false;
	}
}

void plugin::OnEndSession()
{
	base_time += session_duration;
}

bool plugin::OnWantsWeatherAccess()
{
	return true;
}

bool plugin::OnAccessWeather(double trackNodeSize, WeatherControlInfoV01 &info)
{
	if (margin < max_margin)
		margin += 1.0;

	weather_info w;
	weather_simulate((base_time + margin + info.mET) * speed, &w);

	info.mET += margin;
	info.mCloudiness = w.cloud;
	info.mRaining[1][1] = w.rain;
	info.mAmbientTempK = w.air_temp + 273.15;
	info.mWindMaxSpeed = w.wind_speed;
	info.mApplyCloudinessInstantly = false;

#ifdef _DEBUG
	static int i = 0;
	if (++i % 10 == 0)
	{
		DisplayMessage("C%1% R%2% W%3% T%4$.1f",
			(int)(w.cloud * 100),
			(int)(w.rain * 100),
			(int)(w.wind_speed * 100),
			w.air_temp);
	}
#endif

	return true;
}
