#ifndef WEATHER_SOURCE_H
#define WEATHER_SOURCE_H

#include "TileSource.h"
#include "weatherwidget/WeatherDataModel.h"

// Fetches Visual Crossing weather for one location into a WeatherDataModel.
// Parameterizes the compile-time single WEATHER_LOCATION into a constructor arg.
class WeatherSource : public TileSource {
public:
    WeatherSource(const String &location, const String &units = "metric");

    void update(bool force = false) override;
    String getName() override { return "Weather:" + m_location; }

    WeatherDataModel *getModel() { return &m_model; }

private:
    bool fetch();
    String buildUrl();

    String m_location;
    String m_units;
    WeatherDataModel m_model;

    const unsigned long m_refreshInterval = 600000; // 10 min
    unsigned long m_lastRefresh = 0;
    bool m_firstPass = true;
};

#endif // WEATHER_SOURCE_H
