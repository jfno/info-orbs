#ifndef LAUNCH_SOURCE_H
#define LAUNCH_SOURCE_H

#include "TileSource.h"

// Next upcoming rocket launch via the keyless Launch Library 2 API. Refreshed
// once per hour (the API allows ~15 requests/hour unauthenticated).
class LaunchSource : public TileSource {
public:
    LaunchSource();

    void update(bool force = false) override;
    String getName() override { return "Launch"; }

    String getLaunchName() { return m_name; }
    time_t getNet() { return m_net; }
    bool isValid() { return m_valid; }

private:
    bool fetch();

    String m_name;
    time_t m_net = 0; // launch time, UTC epoch
    bool m_valid = false;

    const unsigned long m_refreshInterval = 3600000UL; // 1h
    unsigned long m_lastRefresh = 0;
    bool m_firstPass = true;
};

#endif // LAUNCH_SOURCE_H
