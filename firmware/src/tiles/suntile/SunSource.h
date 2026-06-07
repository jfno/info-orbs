#ifndef SUN_SOURCE_H
#define SUN_SOURCE_H

#include "TileSource.h"

// Sunrise/sunset for one location via the keyless sunrise-sunset.org API.
// Stores the event times as UTC epochs (the tile formats them in its display TZ).
// Refreshed roughly once per day.
class SunSource : public TileSource {
public:
    SunSource(float lat, float lng);

    void update(bool force = false) override;
    String getName() override { return "Sun"; }

    time_t getSunrise() { return m_sunrise; }
    time_t getSunset() { return m_sunset; }
    bool isValid() { return m_valid; }
    bool consumeChanged();

private:
    bool fetch();

    float m_lat;
    float m_lng;
    time_t m_sunrise = 0;
    time_t m_sunset = 0;
    bool m_valid = false;
    bool m_changed = false;

    const unsigned long m_refreshInterval = 86400000UL; // 24h
    unsigned long m_lastRefresh = 0;
    bool m_firstPass = true;
};

#endif // SUN_SOURCE_H
