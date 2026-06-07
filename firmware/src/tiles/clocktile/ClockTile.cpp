#include "ClockTile.h"

#include "GlobalTime.h"
#include "Utils.h"
#include "config_helper.h"
#include <stdlib.h>
#include <time.h>

ClockTile::ClockTile(const String &label, const String &posixTz)
    : m_label(label), m_posixTz(posixTz) {}

void ClockTile::render(ScreenManager &sm, int screen, bool force) {
    time_t utc = GlobalTime::getInstance()->getUtcEpoch();

    if (!force && utc == m_lastUtc && screen == m_lastScreen) {
        return;
    }
    m_lastUtc = utc;

    // Resolve this zone's local time on-device via the POSIX TZ database.
    setenv("TZ", m_posixTz.c_str(), 1);
    tzset();
    struct tm t;
    localtime_r(&utc, &t);

    int hour24 = t.tm_hour;
    int minute = t.tm_min;
    bool isPM = hour24 >= 12;

    int hour = hour24;
    if (!FORMAT_24_HOUR) {
        hour = hour24 % 12;
        if (hour == 0) {
            hour = 12;
        }
    }

    char hhmm[6];
    snprintf(hhmm, sizeof(hhmm), "%02d:%02d", hour, minute);
    String timeStr(hhmm);

    // Only redraw when forced (tile moved to a new orb / first draw) or the
    // displayed minute changed.
    if (!force && timeStr == m_lastTime && screen == m_lastScreen) {
        return;
    }
    m_lastTime = timeStr;
    m_lastScreen = screen;

    sm.setFont(DEFAULT_FONT);
    sm.selectScreen(screen);
    sm.fillScreen(TFT_BLACK);
    sm.setFontColor(TFT_WHITE, TFT_BLACK);

    // City label -- fitted so long names (e.g. "Los Angeles") shrink to stay
    // inside the round bezel instead of bleeding off the edges.
    sm.drawFittedString(m_label, ScreenCenterX, 56, 180, 30, Align::MiddleCenter);

    // Big HH:MM, bounded to a box that fits within the circle.
    sm.drawFittedString(timeStr, ScreenCenterX, 122, 200, 92, Align::MiddleCenter);

    // Day-of-week + AM/PM marker
    String footer = String(LOC_WEEKDAY[t.tm_wday]);
    footer.remove(3);
    if (!FORMAT_24_HOUR) {
        footer += isPM ? "  PM" : "  AM";
    }
    sm.setFontColor(TFT_LIGHTGREY, TFT_BLACK);
    sm.drawFittedString(footer, ScreenCenterX, 190, 160, 26, Align::MiddleCenter);
}
