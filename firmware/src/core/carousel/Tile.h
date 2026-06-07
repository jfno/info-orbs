#ifndef TILE_H
#define TILE_H

#include "ScreenManager.h"
#include <Arduino.h>

// A Tile renders exactly ONE orb's worth of content. The Carousel maps a window
// of consecutive tiles onto the 5 physical orbs and advances it one orb at a time.
// Tiles never do any networking themselves -- they read cached data from a
// TileSource (or, like ClockTile, need no source at all).
class Tile {
public:
    virtual ~Tile() = default;

    // Render this tile to physical orb `screen` (0..4). When `force` is true the
    // tile must redraw everything (e.g. the orb just changed which tile it shows);
    // otherwise it may redraw only what changed since the last call on this orb.
    virtual void render(ScreenManager &sm, int screen, bool force) = 0;

    // Optional per-tile action (e.g. toggle a sub-mode or force a refresh).
    virtual void onTap() {}

    virtual String getName() = 0;
};

#endif // TILE_H
