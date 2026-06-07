#ifndef FX_TILE_H
#define FX_TILE_H

#include "FxSource.h"
#include "Tile.h"

// One USD/XXX exchange rate on a single orb.
class FxTile : public Tile {
public:
    FxTile(FxSource *source, const String &currency);

    void render(ScreenManager &sm, int screen, bool force) override;
    void onTap() override;
    String getName() override { return "FX:" + m_currency; }

private:
    FxSource *m_source;
    String m_currency;
};

#endif // FX_TILE_H
