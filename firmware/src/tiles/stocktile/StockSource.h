#ifndef STOCK_SOURCE_H
#define STOCK_SOURCE_H

#include "TileSource.h"
#include "stockwidget/StockDataModel.h"
#include <vector>

// Owns the full ticker list and refreshes it on a throttled schedule. Fetches are
// spread one-per-tick (m_fetchSpacing) so we stay under the TwelveData free-tier
// limit of 8 requests/minute, and a full pass repeats every STOCK_REFRESH_MINUTES.
class StockSource : public TileSource {
public:
    StockSource(const String &tickerList);

    void update(bool force = false) override;
    String getName() override { return "Stocks"; }

    StockDataModel *getBySymbol(const String &symbol);
    size_t count() const { return m_stocks.size(); }
    String symbolAt(size_t i) { return m_stocks[i].getSymbol(); }

private:
    void fetchSymbol(StockDataModel &stock);
    bool marketIsOpen();

    std::vector<StockDataModel> m_stocks;

    unsigned long m_refreshInterval; // full-pass interval (ms)
    unsigned long m_fetchSpacing = 8000; // min gap between single fetches (ms)

    bool m_cycleActive = false;
    size_t m_fetchIndex = 0;
    unsigned long m_lastCycleStart = 0;
    unsigned long m_lastFetch = 0;
    bool m_firstPass = true;
};

#endif // STOCK_SOURCE_H
