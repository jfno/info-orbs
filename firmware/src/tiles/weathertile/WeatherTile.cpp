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
    TJpgDec.setJpgScale(scale);
    uint16_t w = 0, h = 0;
    TJpgDec.getJpgSize(&w, &h, iconStart, size);
    int x = (SCREEN_SIZE - w) / 2;
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
    sm.drawFittedString(city, ScreenCenterX, 32, 210, 36, Align::MiddleCenter);

    if (model->getCurrentTemperature() == 0.0 && model->getCurrentIcon().isEmpty()) {
        sm.drawString("...", ScreenCenterX, ScreenCenterY, 29, Align::MiddleCenter);
        return;
    }

    // Weather icon (scale 3 ~ 80px) near the top.
    drawIcon(sm, model->getCurrentIcon(), 55, 3);

    // Current temperature, large.
    sm.setFontColor(TFT_WHITE, TFT_BLACK);
    sm.drawString(model->getCurrentTemperature(0), ScreenCenterX, 160, 48, Align::MiddleCenter);

    // Today high / low.
    sm.setFontColor(TFT_LIGHTGREY, TFT_BLACK);
    sm.drawString("H:" + model->getTodayHigh(0) + "  L:" + model->getTodayLow(0), ScreenCenterX, 205, 18, Align::MiddleCenter);
}
