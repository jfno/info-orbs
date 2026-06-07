#ifndef CLOCK_TILE_H
#define CLOCK_TILE_H

#include "Tile.h"

// A single-orb world clock. Uses the POSIX TZ database (with DST rules) entirely
// on-device -- no per-zone API call. Each instance holds a label + POSIX TZ
// string (e.g. "PST8PDT,M3.2.0,M11.1.0").
class ClockTile : public Tile {
public:
    ClockTile(const String &label, const String &posixTz);

    void render(ScreenManager &sm, int screen, bool force) override;
    String getName() override { return "Clock:" + m_label; }

private:
    String m_label;
    String m_posixTz;
    String m_lastTime;  // last drawn HH:MM, for incremental redraw
    int m_lastScreen = -1;
};

#endif // CLOCK_TILE_H
