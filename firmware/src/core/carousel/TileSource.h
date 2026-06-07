#ifndef TILE_SOURCE_H
#define TILE_SOURCE_H

#include <Arduino.h>

// A TileSource owns network data plus its own refresh timer. update() is polled
// every loop by the Carousel; the source decides (via its own millis() cadence)
// when to actually fetch. This decouples N tiles from M network calls and lets
// each source throttle independently. Mirrors the Widget::update(force) pattern.
class TileSource {
public:
    virtual ~TileSource() = default;

    // Polled every loop. Implementations fetch/cache on their own schedule.
    // `force` requests an immediate refresh regardless of the timer (used for the
    // initial blocking load and for on-demand refresh from a tile's onTap()).
    virtual void update(bool force = false) = 0;

    virtual String getName() = 0;
};

#endif // TILE_SOURCE_H
