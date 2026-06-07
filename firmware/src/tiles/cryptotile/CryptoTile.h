#ifndef CRYPTO_TILE_H
#define CRYPTO_TILE_H

#include "CryptoSource.h"
#include "Tile.h"

// One cryptocurrency on a single orb: USD price + 24h % change (green/red).
class CryptoTile : public Tile {
public:
    CryptoTile(CryptoSource *source, const String &id, const String &label);

    void render(ScreenManager &sm, int screen, bool force) override;
    void onTap() override;
    String getName() override { return "Crypto:" + m_label; }

private:
    CryptoSource *m_source;
    String m_id;
    String m_label;
};

#endif // CRYPTO_TILE_H
