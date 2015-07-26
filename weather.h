#pragma once

#pragma pack(push, 4)

struct weather_param_config
{
	double zero;
	double amplitude;
	double min;
	double max;
	double period; // roughly seconds of each cycle (shorter value produces more rapid changes)
	int octave; // stages of synthesizing noise (higher value produces more complex changes)
	const char* expression; // expression for calculating value (null: default setting)
};

struct weather_config
{
	double rain_chance; // probability of rain (0.0-1.0)
	double min_air_temp; // minimum air temperature
	double max_air_temp; // maximum air temperature
	weather_param_config cloud;
	weather_param_config rain;
	weather_param_config wind_speed;
	weather_param_config air_temp;
};

struct weather_info
{
	double cloud; // amount of cloud (0.0-1.0)
	double rain; // amount of rain (0.0-1.0)
	double wind_speed; // wind speed (0.0-1.0)
	double air_temp; // air temperature (celsius)
	char reserved[32]; // for future expansion
};

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

	// init weather lib
	void weather_init(unsigned int seed, weather_config *config);

	// simulate weather
	void weather_simulate(double time, weather_info *info);

#ifdef __cplusplus
}
#endif//__cplusplus
