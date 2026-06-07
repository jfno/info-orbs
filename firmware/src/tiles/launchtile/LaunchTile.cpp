#include "LaunchTile.h"

#include "GlobalTime.h"
#include "Utils.h"
#include "config_helper.h"

LaunchTile::LaunchTile(LaunchSource *source) : m_source(source) {}

void LaunchTile::render(ScreenManager &sm, int screen, bool force) {
    String countdown;
    if (m_source->isValid()) {
        time_t now = GlobalTime::getInstance()->getUtcEpoch();
        long remaining = (long)(m_source->getNet() - now);
        if (remaining < 0) {
            countdown = "LIFTOFF";
        } else {
            long days = remaining / 86400;
            long hours = (remaining % 86400) / 3600;
            long mins = (remaining % 3600) / 60;
            char buf[24];
            snprintf(buf, sizeof(buf), "T- %ldd %02ld:%02ld", days, hours, mins);
            countdown = String(buf);
        }
    } else {
        countdown = "...";
    }

    // Redraw only when forced (moved orb) or the displayed minute changed.
    if (!force && countdown == m_lastCountdown && screen == m_lastScreen) {
        return;
    }
    m_lastCountdown = countdown;
    m_lastScreen = screen;

    sm.setFont(DEFAULT_FONT);
    sm.selectScreen(screen);
    sm.fillScreen(TFT_BLACK);

    const int centre = 120;
    sm.setFontColor(TFT_CYAN, TFT_BLACK);
    sm.drawString("Next Launch", centre, 40, 20, Align::MiddleCenter);

    sm.setFontColor(TFT_WHITE, TFT_BLACK);
    String name = m_source->isValid() ? m_source->getLaunchName() : String("");
    sm.drawFittedString(name, centre, 110, 220, 70, Align::MiddleCenter);

    sm.setFontColor(TFT_YELLOW, TFT_BLACK);
    sm.drawString(countdown, centre, 195, 22, Align::MiddleCenter);
}
