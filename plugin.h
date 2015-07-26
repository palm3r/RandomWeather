#pragma once
#include "rf2plugin.h"

class plugin : public rf2::plugin
{
public:
	plugin() : rf2::plugin("RandomWeather") {}
	virtual ~plugin() {}

protected:
	void OnStartup(long version);
	void OnLoad();
	void OnStartSession();
	void OnEndSession();
	bool OnWantsScoringUpdates();
	void OnUpdateScoring(const ScoringInfoV01 &info);
	bool OnWantsWeatherAccess();
	bool OnAccessWeather(double trackNodeSize, WeatherControlInfoV01 &info);
};