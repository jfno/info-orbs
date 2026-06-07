#include "Carousel.h"

#include "Utils.h"

Carousel::Carousel(ScreenManager *sm) : m_screenManager(sm) {
    m_advancePrev = millis();
}

void Carousel::addTile(Tile *tile) {
    m_tiles.push_back(tile);
}

void Carousel::addSource(TileSource *source) {
    m_sources.push_back(source);
}

void Carousel::showLoading() {
    m_screenManager->selectScreen(3);
    m_screenManager->fillScreen(TFT_BLACK);
    m_screenManager->setFontColor(TFT_WHITE);
    m_screenManager->drawCentreString("Loading data:", ScreenCenterX, ScreenCenterY, 22);
}

void Carousel::update() {
    if (!m_initialized) {
        // One-time blocking load so the first frame isn't blank.
        showLoading();
        for (TileSource *source : m_sources) {
            m_screenManager->selectScreen(4);
            m_screenManager->fillScreen(TFT_BLACK);
            m_screenManager->setFontColor(TFT_WHITE);
            m_screenManager->drawCentreString(source->getName(), ScreenCenterX, ScreenCenterY, 22);
            source->update(true);
        }
        m_screenManager->clearAllScreens();
        m_initialized = true;
        m_advancePrev = millis();
        return;
    }

    for (TileSource *source : m_sources) {
        source->update();
    }

    if (!m_paused && !m_tiles.empty() && (millis() - m_advancePrev) >= m_advanceDelay) {
        m_offset = (m_offset + 1) % m_tiles.size();
        m_advancePrev = millis();
    }
}

void Carousel::renderWindow(bool force) {
    if (m_tiles.empty()) {
        return;
    }
    int count = m_tiles.size();
    for (int i = 0; i < NUM_SCREENS; i++) {
        Tile *tile = m_tiles[(m_offset + i) % count];
        tile->render(*m_screenManager, i, force);
    }
}

void Carousel::draw(bool force) {
    if (!m_initialized) {
        return;
    }
    bool offsetChanged = (m_offset != m_lastDrawnOffset);
    renderWindow(force || offsetChanged);
    m_lastDrawnOffset = m_offset;
}

void Carousel::next() {
    if (m_tiles.empty()) {
        return;
    }
    m_offset = (m_offset + 1) % m_tiles.size();
    m_advancePrev = millis();
}

void Carousel::prev() {
    if (m_tiles.empty()) {
        return;
    }
    m_offset = (m_offset - 1 + (int)m_tiles.size()) % m_tiles.size();
    m_advancePrev = millis();
}

void Carousel::togglePause() {
    m_paused = !m_paused;
    m_advancePrev = millis();
    Serial.printf("Carousel %s\n", m_paused ? "paused" : "resumed");
}

void Carousel::tapCenter() {
    if (m_tiles.empty()) {
        return;
    }
    // The middle physical orb (index 2) shows tile (m_offset + 2).
    int center = (m_offset + 2) % m_tiles.size();
    m_tiles[center]->onTap();
}

void Carousel::updateBrightnessByTime(uint8_t hour24) {
#if defined(DIM_START_HOUR) && defined(DIM_END_HOUR) && defined(DIM_BRIGHTNESS)
    bool isInDimRange;

    if (DIM_START_HOUR < DIM_END_HOUR) {
        // Normal case: the range does not cross midnight
        isInDimRange = (hour24 >= DIM_START_HOUR && hour24 < DIM_END_HOUR);
    } else {
        // Case where the range crosses midnight
        isInDimRange = (hour24 >= DIM_START_HOUR || hour24 < DIM_END_HOUR);
    }

    uint8_t brightness = isInDimRange ? DIM_BRIGHTNESS : TFT_BRIGHTNESS;
    if (m_screenManager->setBrightness(brightness)) {
        // brightness was changed -> force a full redraw
        m_screenManager->clearAllScreens();
        draw(true);
    }
#endif
}
