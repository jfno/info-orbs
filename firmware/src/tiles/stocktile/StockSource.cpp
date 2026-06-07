#include "StockSource.h"

#include "GlobalTime.h"
#include "config_helper.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "../../core/utils/HttpHelper.h"
#include <stdlib.h>
#include <time.h>

#ifndef STOCK_REFRESH_MINUTES
    #define STOCK_REFRESH_MINUTES 20
#endif

StockSource::StockSource(const String &tickerList) {
    m_refreshInterval = (unsigned long)STOCK_REFRESH_MINUTES * 60UL * 1000UL;

    // Parse the comma-separated ticker list (same shape as StockWidget).
    int start = 0;
    while (start < (int)tickerList.length()) {
        int comma = tickerList.indexOf(',', start);
        if (comma == -1) {
            comma = tickerList.length();
        }
        String sym = tickerList.substring(start, comma);
        sym.trim();
        if (sym.length() > 0) {
            StockDataModel model = StockDataModel();
            model.setSymbol(sym);
            m_stocks.push_back(model);
        }
        start = comma + 1;
    }
    Serial.printf("StockSource: %d symbols\n", (int)m_stocks.size());
}

StockDataModel *StockSource::getBySymbol(const String &symbol) {
    for (auto &stock : m_stocks) {
        if (stock.getSymbol() == symbol) {
            return &stock;
        }
    }
    return nullptr;
}

bool StockSource::marketIsOpen() {
#ifdef STOCK_MARKET_HOURS_ONLY
    time_t utc = GlobalTime::getInstance()->getUtcEpoch();
    setenv("TZ", "EST5EDT,M3.2.0,M11.1.0", 1); // US Eastern
    tzset();
    struct tm t;
    localtime_r(&utc, &t);
    if (t.tm_wday == 0 || t.tm_wday == 6) {
        return false; // weekend
    }
    int minutes = t.tm_hour * 60 + t.tm_min;
    return minutes >= (9 * 60 + 30) && minutes < (16 * 60); // 9:30-16:00 ET
#else
    return true;
#endif
}

void StockSource::update(bool force) {
    if (m_stocks.empty()) {
        return;
    }

    // Start a new refresh pass when due (or on the first/forced call).
    if (!m_cycleActive && (force || m_firstPass || (millis() - m_lastCycleStart) >= m_refreshInterval)) {
        if (force || m_firstPass || marketIsOpen()) {
            m_cycleActive = true;
            m_fetchIndex = 0;
            m_lastCycleStart = millis();
        } else {
            // Market closed: defer to next interval without fetching.
            m_lastCycleStart = millis();
        }
    }

    if (m_cycleActive && (force || (millis() - m_lastFetch) >= m_fetchSpacing)) {
        fetchSymbol(m_stocks[m_fetchIndex]);
        m_lastFetch = millis();
        m_fetchIndex++;
        if (m_fetchIndex >= m_stocks.size()) {
            m_cycleActive = false;
            m_firstPass = false;
        }
        // On the initial blocking (force) load, only fetch the first symbol so boot
        // stays fast; the rest stream in via subsequent polled update() calls.
        if (force) {
            m_cycleActive = m_fetchIndex < m_stocks.size();
        }
    }
}

void StockSource::fetchSymbol(StockDataModel &stock) {
    String url = "https://api.twelvedata.com/quote?apikey=" + String(STOCK_API_KEY) + "&symbol=" + stock.getSymbol();

    HTTPClient http;
    HttpHelper::prepare(http);
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
        JsonDocument doc;
        DeserializationError error = HttpHelper::deserialize(http, doc);
        if (!error) {
            float currentPrice = doc["close"].as<float>();
            if (currentPrice > 0.0) {
                stock.setCurrentPrice(currentPrice);
                stock.setPercentChange(doc["percent_change"].as<float>() / 100);
                stock.setPriceChange(doc["change"].as<float>());
                stock.setHighPrice(doc["fifty_two_week"]["high"].as<float>());
                stock.setLowPrice(doc["fifty_two_week"]["low"].as<float>());
                stock.setCompany(doc["name"].as<String>());
                stock.setTicker(doc["symbol"].as<String>());
                stock.setCurrencySymbol(doc["currency"].as<String>());
            } else {
                Serial.println("StockSource: invalid data for " + stock.getSymbol());
            }
        } else {
            Serial.println("StockSource: deserializeJson() failed for " + stock.getSymbol());
        }
    } else {
        Serial.printf("StockSource: HTTP error %s for %s\n", http.errorToString(httpCode).c_str(), stock.getSymbol().c_str());
    }
    http.end();
}
