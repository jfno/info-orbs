#include "StockTile.h"

#include "Utils.h"
#include "config_helper.h"

StockTile::StockTile(StockSource *source, const String &symbol)
    : m_source(source), m_symbol(symbol) {}

void StockTile::onTap() {
    m_source->update(true);
}

void StockTile::render(ScreenManager &sm, int screen, bool force) {
    StockDataModel *stock = m_source->getBySymbol(m_symbol);

    bool hasData = stock != nullptr && stock->getCurrentPrice() > 0.0;
    bool changed = stock != nullptr && stock->isChanged();
    if (!force && !changed) {
        return;
    }
    if (stock != nullptr) {
        stock->setChangedStatus(false);
    }

    sm.setFont(DEFAULT_FONT);
    sm.selectScreen(screen);
    sm.fillScreen(TFT_BLACK);

    const int centre = 120;

    if (!hasData) {
        // Placeholder until the source fills this symbol in.
        sm.setFontColor(TFT_WHITE, TFT_BLACK);
        sm.drawFittedString(m_symbol, centre, 100, 180, 32, Align::MiddleCenter);
        sm.drawString("...", centre, 145, 29, Align::MiddleCenter);
        return;
    }

    bool negative = stock->getPercentChange() < 0.0;
    uint32_t accent = negative ? TFT_RED : TFT_GREEN;

    // Coloured ring + up/down arrow at the top.
    sm.drawArc(centre, centre, 120, 116, 0, 360, accent, accent);
    sm.setFontColor(accent, TFT_BLACK);
    if (negative) {
        sm.fillTriangle(110, 30, 130, 30, 120, 44, accent);
    } else {
        sm.fillTriangle(110, 44, 130, 44, 120, 30, accent);
    }

    // Ticker, percent change, price.
    sm.setFontColor(accent, TFT_BLACK);
    sm.drawFittedString(stock->getPercentChange(2) + "%", centre, 70, 180, 28, Align::MiddleCenter);

    sm.setFontColor(TFT_WHITE, TFT_BLACK);
    String ticker = stock->getTicker();
    if (ticker.isEmpty()) {
        ticker = m_symbol;
    }
    sm.drawFittedString(ticker, centre, 120, 200, 36, Align::MiddleCenter);
    sm.drawFittedString(stock->getCurrencySymbol() + stock->getCurrentPrice(2), centre, 165, 180, 30, Align::MiddleCenter);
}
