#ifndef TEXT_TILE_H
#define TEXT_TILE_H

#include "Tile.h"
#include "Utils.h"

// A trivial static-text tile. Useful for seeding/testing the carousel window
// logic and as a fallback label.
class TextTile : public Tile {
public:
    TextTile(const String &title, const String &subtitle = "", uint32_t color = TFT_WHITE)
        : m_title(title), m_subtitle(subtitle), m_color(color) {}

    void render(ScreenManager &sm, int screen, bool force) override {
        if (!force) {
            return; // static content; only redraw when forced
        }
        sm.selectScreen(screen);
        sm.fillScreen(TFT_BLACK);
        sm.setFontColor(m_color, TFT_BLACK);
        if (m_subtitle.isEmpty()) {
            sm.drawCentreString(m_title, ScreenCenterX, ScreenCenterY, 29);
        } else {
            sm.drawCentreString(m_title, ScreenCenterX, ScreenCenterY - 20, 29);
            sm.drawCentreString(m_subtitle, ScreenCenterX, ScreenCenterY + 25, 18);
        }
    }

    String getName() override { return "Text:" + m_title; }

private:
    String m_title;
    String m_subtitle;
    uint32_t m_color;
};

#endif // TEXT_TILE_H
