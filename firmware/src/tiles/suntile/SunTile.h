#ifndef SUN_TILE_H
#define SUN_TILE_H

#include "SunSource.h"
#include "Tile.h"

// Sunrise/sunset (formatted in a display TZ) plus a locally-computed moon phase.
class SunTile : public Tile {
public:
    SunTile(SunSource *source, const String &displayTz, const String &label = "Sun & Moon");

    void render(ScreenManager &sm, int screen, bool force) override;
    String getName() override { return "SunTile"; }

private:
    String formatLocal(time_t epoch);
    String moonPhaseName();

    SunSource *m_source;
    String m_displayTz;
    String m_label;
    int m_lastMoonIndex = -1;
};

#endif // SUN_TILE_H
