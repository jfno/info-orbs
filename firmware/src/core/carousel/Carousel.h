#ifndef CAROUSEL_H
#define CAROUSEL_H

#include "ScreenManager.h"
#include "Tile.h"
#include "TileSource.h"
#include "config_helper.h"
#include <Arduino.h>
#include <vector>

#ifndef CAROUSEL_ADVANCE_DELAY
    #define CAROUSEL_ADVANCE_DELAY 6 // seconds per step
#endif

// The Carousel owns an ordered ribbon of single-orb Tiles and a set of
// TileSources. Each draw shows a window of NUM_SCREENS consecutive tiles mapped
// onto the physical orbs; the window auto-advances one orb at a time.
class Carousel {
public:
    Carousel(ScreenManager *sm);

    // Ownership: the Carousel deletes everything added here in its destructor.
    void addTile(Tile *tile);
    void addSource(TileSource *source);

    // Polls every source, runs the one-time blocking initial load, and advances
    // the window when the auto-advance timer fires (unless paused).
    void update();

    // Draws the current window. force=true redraws all orbs unconditionally.
    void draw(bool force = false);

    // Button actions.
    void next();         // advance window by one orb
    void prev();         // retreat window by one orb
    void togglePause();  // pause/resume auto-advance
    void tapCenter();    // forward onTap() to the centered tile

    // Ported from WidgetSet::updateBrightnessByTime so night dimming keeps working.
    void updateBrightnessByTime(uint8_t hour24);

    // Force the next draw() to repaint all orbs (e.g. after a WiFi reconnect).
    void forceRedraw() { m_lastDrawnOffset = -1; }

private:
    void renderWindow(bool force);

    ScreenManager *m_screenManager;
    std::vector<Tile *> m_tiles;
    std::vector<TileSource *> m_sources;

    int m_offset = 0;
    int m_lastDrawnOffset = -1;
    size_t m_pollIndex = 0; // round-robin index for staggered source polling
    bool m_paused = false;
    bool m_initialized = false;

    unsigned long m_advanceDelay = CAROUSEL_ADVANCE_DELAY * 1000UL;
    unsigned long m_advancePrev = 0;
};

#endif // CAROUSEL_H
