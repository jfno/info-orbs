#include "FxTile.h"

#include "Utils.h"
#include "config_helper.h"

FxTile::FxTile(FxSource *source, const String &currency)
    : m_source(source), m_currency(currency) {}

void FxTile::onTap() {
    m_source->update(true);
}

void FxTile::render(ScreenManager &sm, int screen, bool force) {
    FxSource::Rate *rate = m_source->getByCurrency(m_currency);
    bool changed = rate != nullptr && rate->changed;
    if (!force && !changed) {
        return;
    }
    if (rate != nullptr) {
        rate->changed = false;
    }

    sm.setFont(DEFAULT_FONT);
    sm.selectScreen(screen);
    sm.fillScreen(TFT_BLACK);
    sm.setFontColor(TFT_WHITE, TFT_BLACK);

    const int centre = 120;
    sm.drawFittedString("USD/" + m_currency, centre, 85, 180, 32, Align::MiddleCenter);

    if (rate == nullptr || !rate->valid) {
        sm.drawString("...", centre, 140, 29, Align::MiddleCenter);
        return;
    }
    sm.drawFittedString(Utils::formatFloat(rate->value, 4), centre, 140, 200, 36, Align::MiddleCenter);
}
