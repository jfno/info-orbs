#ifndef LAUNCH_TILE_H
#define LAUNCH_TILE_H

#include "LaunchSource.h"
#include "Tile.h"

// Next rocket launch: name + live countdown (T- d:hh:mm).
class LaunchTile : public Tile {
public:
    LaunchTile(LaunchSource *source);

    void render(ScreenManager &sm, int screen, bool force) override;
    String getName() override { return "LaunchTile"; }

private:
    LaunchSource *m_source;
    String m_lastCountdown;
    int m_lastScreen = -1;
};

#endif // LAUNCH_TILE_H
