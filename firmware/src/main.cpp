#include "Button.h"
#include "Carousel.h"
#include "CarouselConfig.h"
#include "GlobalTime.h"
#include "ScreenManager.h"
#include "Utils.h"
#include "config_helper.h"
#include "icons.h"
#include "wifiwidget/WifiWidget.h"
#include <Arduino.h>
#include <TJpg_Decoder.h>

TFT_eSPI tft = TFT_eSPI();

Button buttonLeft(BUTTON_LEFT);
Button buttonOK(BUTTON_OK);
Button buttonRight(BUTTON_RIGHT);

GlobalTime *globalTime; // Initialize the global time

String connectingString{""};

WifiWidget *wifiWidget{nullptr};

int connectionTimer{0};
const int connectionTimeout{10000};
bool isConnected{true};

ScreenManager *sm;
Carousel *carousel;

// This function should probably be moved somewhere else
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    if (y >= tft.height())
        return 0;
    // Dim bitmap
    for (int i = 0; i < w * h; i++) {
        bitmap[i] = Utils::rgb565dim(bitmap[i], sm->getBrightness(), true);
    }
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}

/**
 * The ISR handlers must be static
 */
void isrButtonChangeLeft() { buttonLeft.isrButtonChange(); }
void isrButtonChangeMiddle() { buttonOK.isrButtonChange(); }
void isrButtonChangeRight() { buttonRight.isrButtonChange(); }

void setupButtons() {
    buttonLeft.begin();
    buttonOK.begin();
    buttonRight.begin();

    attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), isrButtonChangeLeft, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BUTTON_OK), isrButtonChangeMiddle, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), isrButtonChangeRight, CHANGE);
}

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Starting up...");

    TJpgDec.setSwapBytes(true); // JPEG rendering setup
    TJpgDec.setCallback(tft_output);
    setupButtons();

    sm = new ScreenManager(tft);
    sm->fillAllScreens(TFT_BLACK);
    sm->setFontColor(TFT_WHITE);

#ifdef GC9A01_DRIVER
    Serial.println("GC9A01 Driver");
#endif
#if HARDWARE == WOKWI
    Serial.println("Wokwi Build");
#endif

    pinMode(BUSY_PIN, OUTPUT);
    Serial.println("Connecting to WiFi");

    wifiWidget = new WifiWidget(*sm);
    wifiWidget->setup();

    globalTime = GlobalTime::getInstance();

    carousel = new Carousel(sm);
    buildCarousel(*carousel);
}

void checkButtons() {
    if (buttonLeft.pressedShort()) {
        Serial.println("Left button short pressed -> previous tile");
        carousel->prev();
    } else if (buttonRight.pressedShort()) {
        Serial.println("Right button short pressed -> next tile");
        carousel->next();
    } else {
        ButtonState middleState = buttonOK.getState();
        if (middleState == BTN_SHORT) {
            Serial.println("Middle button short pressed -> pause/resume");
            carousel->togglePause();
        } else if (middleState == BTN_MEDIUM || middleState == BTN_LONG) {
            Serial.printf("Middle button pressed, state=%d -> tap center tile\n", middleState);
            carousel->tapCenter();
        }
    }
}

void loop() {
    if (wifiWidget->isConnected() == false) {
        wifiWidget->update();
        wifiWidget->draw();
        carousel->forceRedraw(); // repaint tiles once WiFi is back
        delay(100);
    } else {
        globalTime->updateTime();

        checkButtons();

        carousel->update();
        carousel->updateBrightnessByTime(globalTime->getHour24());
        carousel->draw();
    }
}
