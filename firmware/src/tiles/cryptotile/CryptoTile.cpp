#include "CryptoTile.h"

#include "Utils.h"
#include "config_helper.h"

CryptoTile::CryptoTile(CryptoSource *source, const String &id, const String &label)
    : m_source(source), m_id(id), m_label(label) {}

void CryptoTile::onTap() {
    m_source->update(true);
}

void CryptoTile::render(ScreenManager &sm, int screen, bool force) {
    CryptoSource::Coin *coin = m_source->getById(m_id);
    bool changed = coin != nullptr && coin->changed;
    if (!force && !changed) {
        return;
    }
    if (coin != nullptr) {
        coin->changed = false;
    }

    sm.setFont(DEFAULT_FONT);
    sm.selectScreen(screen);
    sm.fillScreen(TFT_BLACK);

    const int centre = 120;

    if (coin == nullptr || !coin->valid) {
        sm.setFontColor(TFT_WHITE, TFT_BLACK);
        sm.drawFittedString(m_label, centre, 100, 180, 32, Align::MiddleCenter);
        sm.drawString("...", centre, 145, 29, Align::MiddleCenter);
        return;
    }

    bool negative = coin->change24h < 0.0;
    uint32_t accent = negative ? TFT_RED : TFT_GREEN;

    sm.drawArc(centre, centre, 120, 116, 0, 360, accent, accent);

    sm.setFontColor(TFT_WHITE, TFT_BLACK);
    sm.drawFittedString(m_label, centre, 70, 180, 36, Align::MiddleCenter);

    int8_t digits = coin->price >= 1000.0 ? 0 : 2;
    sm.drawFittedString("$" + Utils::formatFloat(coin->price, digits), centre, 120, 200, 30, Align::MiddleCenter);

    sm.setFontColor(accent, TFT_BLACK);
    sm.drawFittedString(Utils::formatFloat(coin->change24h, 2) + "%", centre, 165, 180, 28, Align::MiddleCenter);
}
