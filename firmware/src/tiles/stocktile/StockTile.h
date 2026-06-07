#ifndef STOCK_TILE_H
#define STOCK_TILE_H

#include "StockSource.h"
#include "Tile.h"

// Renders one ticker (price + % change, green/red) on a single orb. Reads cached
// data from a shared StockSource; does no networking itself.
class StockTile : public Tile {
public:
    StockTile(StockSource *source, const String &symbol);

    void render(ScreenManager &sm, int screen, bool force) override;
    void onTap() override; // force an immediate refresh
    String getName() override { return "Stock:" + m_symbol; }

private:
    StockSource *m_source;
    String m_symbol;
};

#endif // STOCK_TILE_H
