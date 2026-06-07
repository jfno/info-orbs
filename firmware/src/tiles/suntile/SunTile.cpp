#include "SunTile.h"

#include "GlobalTime.h"
#include "Utils.h"
#include "config_helper.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

static const float SYNODIC_MONTH = 29.530588853f;
static const time_t KNOWN_NEW_MOON = 947182440; // 2000-01-06 18:14 UTC

SunTile::SunTile(SunSource *source, const String &displayTz, const String &label)
    : m_source(source), m_displayTz(displayTz), m_label(label) {}

String SunTile::formatLocal(time_t epoch) {
    if (epoch <= 0) {
        return "--:--";
    }
    setenv("TZ", m_displayTz.c_str(), 1);
    tzset();
    struct tm t;
    localtime_r(&epoch, &t);
    char buf[6];
    snprintf(buf, sizeof(buf), "%02d:%02d", t.tm_hour, t.tm_min);
    return String(buf);
}

String SunTile::moonPhaseName() {
    time_t utc = GlobalTime::getInstance()->getUtcEpoch();
    float days = (float)(utc - KNOWN_NEW_MOON) / 86400.0f;
    float age = fmodf(days, SYNODIC_MONTH);
    if (age < 0) {
        age += SYNODIC_MONTH;
    }
    int index = (int)((age / SYNODIC_MONTH) * 8.0f + 0.5f) % 8;
    m_lastMoonIndex = index;
    static const char *names[8] = {
        "New", "Wax Cresc", "First Qtr", "Wax Gibb",
        "Full", "Wan Gibb", "Last Qtr", "Wan Cresc"};
    return String(names[index]);
}

void SunTile::render(ScreenManager &sm, int screen, bool force) {
    bool changed = m_source->consumeChanged();
    if (!force && !changed) {
        return;
    }

    sm.setFont(DEFAULT_FONT);
    sm.selectScreen(screen);
    sm.fillScreen(TFT_BLACK);
    sm.setFontColor(TFT_WHITE, TFT_BLACK);

    const int centre = 120;
    sm.drawString(m_label, centre, 40, 20, Align::MiddleCenter);

    sm.setFontColor(TFT_YELLOW, TFT_BLACK);
    sm.drawString("Rise " + formatLocal(m_source->getSunrise()), centre, 95, 22, Align::MiddleCenter);
    sm.setFontColor(TFT_ORANGE, TFT_BLACK);
    sm.drawString("Set  " + formatLocal(m_source->getSunset()), centre, 130, 22, Align::MiddleCenter);

    sm.setFontColor(TFT_LIGHTGREY, TFT_BLACK);
    sm.drawString("Moon: " + moonPhaseName(), centre, 190, 18, Align::MiddleCenter);
}
