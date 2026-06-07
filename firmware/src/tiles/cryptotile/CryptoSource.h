#ifndef CRYPTO_SOURCE_H
#define CRYPTO_SOURCE_H

#include "TileSource.h"
#include <vector>

// CoinGecko simple price (keyless). Fetches USD price + 24h change for a set of
// coin ids (e.g. "bitcoin,ethereum").
class CryptoSource : public TileSource {
public:
    struct Coin {
        String id;
        float price = 0.0;
        float change24h = 0.0;
        bool valid = false;
        bool changed = false;
    };

    CryptoSource(const String &ids);

    void update(bool force = false) override;
    String getName() override { return "Crypto"; }

    Coin *getById(const String &id);

private:
    bool fetch();

    std::vector<Coin> m_coins;
    String m_idsCsv;

    const unsigned long m_refreshInterval = 300000; // 5 min
    unsigned long m_lastRefresh = 0;
    bool m_firstPass = true;
};

#endif // CRYPTO_SOURCE_H
