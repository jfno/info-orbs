#ifndef WEATHER_TILE_H
#define WEATHER_TILE_H

#include "Tile.h"
#include "WeatherSource.h"

// Compact single-orb weather overview: city, current icon, current temp, hi/lo.
class WeatherTile : public Tile {
public:
    WeatherTile(WeatherSource *source, const String &label = "");

    void render(ScreenManager &sm, int screen, bool force) override;
    void onTap() override;
    String getName() override { return "WeatherTile:" + m_source->getName(); }

private:
    void drawIcon(ScreenManager &sm, const String &condition, int y, int scale);

    WeatherSource *m_source;
    String m_label; // optional override; otherwise use resolved city name
};

#endif // WEATHER_TILE_H
