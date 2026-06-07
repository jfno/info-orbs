#ifndef FX_SOURCE_H
#define FX_SOURCE_H

#include "TileSource.h"
#include <vector>

// Foreign-exchange rates with USD base, via the keyless open.er-api.com endpoint.
class FxSource : public TileSource {
public:
    struct Rate {
        String currency; // e.g. "EUR"
        float value = 0.0;
        bool valid = false;
        bool changed = false;
    };

    FxSource(const String &currencies); // e.g. "EUR,CAD"

    void update(bool force = false) override;
    String getName() override { return "FX"; }

    Rate *getByCurrency(const String &currency);

private:
    bool fetch();

    std::vector<Rate> m_rates;

    const unsigned long m_refreshInterval = 1800000; // 30 min
    unsigned long m_lastRefresh = 0;
    bool m_firstPass = true;
};

#endif // FX_SOURCE_H
