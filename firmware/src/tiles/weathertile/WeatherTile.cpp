#include "WeatherTile.h"

#include "Utils.h"
#include "config_helper.h"
#include "icons.h"
#include <TJpg_Decoder.h>

WeatherTile::WeatherTile(WeatherSource *source, const String &label)
    : m_source(source), m_label(label) {}

void WeatherTile::onTap() {
    m_source->update(true);
}

// Dark-mode (B) icon set, mirroring WeatherWidget::drawWeatherIcon. Centred
// horizontally at the given y, at the given scale.
void WeatherTile::drawIcon(ScreenManager &sm, const String &condition, int y, int scale) {
    const byte *iconStart = nullptr;
    const byte *iconEnd = nullptr;

    if (condition == "partly-cloudy-night") {
        iconStart = moonCloudB_start;
        iconEnd = moonCloudB_end;
    } else if (condition == "partly-cloudy-day") {
        iconStart = sunCloudsB_start;
        iconEnd = sunCloudsB_end;
    } else if (condition == "clear-day") {
        iconStart = sunB_start;
        iconEnd = sunB_end;
    } else if (condition == "clear-night") {
        iconStart = moonB_start;
        iconEnd = moonB_end;
    } else if (condition == "snow") {
        iconStart = snowB_start;
        iconEnd = snowB_end;
    } else if (condition == "rain") {
        iconStart = rainB_start;
        iconEnd = rainB_end;
    } else if (condition == "fog" || condition == "wind" || condition == "cloudy") {
        iconStart = cloudsB_start;
        iconEnd = cloudsB_end;
    } else {
        Serial.println("WeatherTile: unknown icon " + condition);
        return;
    }

    int size = iconEnd - iconStart;
    if (size <= 0) {
        return;
    }
    TJpgDec.setJpgScale(scale); // scale must be a power of 2 (1/2/4/8)
    uint16_t w = 0, h = 0;
    TJpgDec.getJpgSize(&w, &h, iconStart, size); // native (unscaled) size
    int scaledW = w / scale;                     // actual rendered width
    int x = (SCREEN_SIZE - scaledW) / 2;         // centre the scaled icon
    TJpgDec.drawJpg(x, y, iconStart, size);
}

void WeatherTile::render(ScreenManager &sm, int screen, bool force) {
    WeatherDataModel *model = m_source->getModel();
    bool changed = model->isChanged();
    if (!force && !changed) {
        return;
    }
    model->setChangedStatus(false);

    sm.setFont(DEFAULT_FONT);
    sm.selectScreen(screen);
    sm.fillScreen(TFT_BLACK);
    sm.setFontColor(TFT_WHITE, TFT_BLACK);

    // City name (label override, else resolved city up to the first comma).
    String city = m_label;
    if (city.isEmpty()) {
        city = model->getCityName();
        int comma = city.indexOf(',');
        if (comma != -1) {
            city.remove(comma);
        }
    }
    // City -- narrow box so long names shrink and stay inside the round bezel.
    sm.drawFittedString(city, ScreenCenterX, 40, 168, 28, Align::MiddleCenter);

    if (model->getCurrentTemperature() == 0.0 && model->getCurrentIcon().isEmpty()) {
        sm.drawString("...", ScreenCenterX, ScreenCenterY, 29, Align::MiddleCenter);
        return;
    }

    // Weather icon (scale 4 -> 60px) in its own band below the city.
    drawIcon(sm, model->getCurrentIcon(), 62, 4);

    // Current temperature, large, in its own row -- no overlap with the icon.
    sm.setFontColor(TFT_WHITE, TFT_BLACK);
    sm.drawFittedString(model->getCurrentTemperature(0), ScreenCenterX, 158, 150, 50, Align::MiddleCenter);

    // Today high / low, two-tone, at the bottom.
    sm.drawString("H " + model->getTodayHigh(0), ScreenCenterX - 42, 200, 20, Align::MiddleCenter, TFT_ORANGE, TFT_BLACK);
    sm.drawString("L " + model->getTodayLow(0), ScreenCenterX + 42, 200, 20, Align::MiddleCenter, TFT_SKYBLUE, TFT_BLACK);
}
