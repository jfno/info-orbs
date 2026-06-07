#include "CryptoSource.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>

CryptoSource::CryptoSource(const String &ids) : m_idsCsv(ids) {
    int start = 0;
    while (start < (int)ids.length()) {
        int comma = ids.indexOf(',', start);
        if (comma == -1) {
            comma = ids.length();
        }
        String id = ids.substring(start, comma);
        id.trim();
        if (id.length() > 0) {
            Coin c;
            c.id = id;
            m_coins.push_back(c);
        }
        start = comma + 1;
    }
}

CryptoSource::Coin *CryptoSource::getById(const String &id) {
    for (auto &c : m_coins) {
        if (c.id == id) {
            return &c;
        }
    }
    return nullptr;
}

void CryptoSource::update(bool force) {
    if (m_coins.empty()) {
        return;
    }
    if (force || m_firstPass || (millis() - m_lastRefresh) >= m_refreshInterval) {
        fetch();
        m_lastRefresh = millis();
        m_firstPass = false;
    }
}

bool CryptoSource::fetch() {
    String url = "https://api.coingecko.com/api/v3/simple/price?ids=" + m_idsCsv +
                 "&vs_currencies=usd&include_24hr_change=true";
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, http.getStream());
        http.end();
        if (!error) {
            for (auto &c : m_coins) {
                JsonObject obj = doc[c.id];
                if (!obj.isNull()) {
                    float price = obj["usd"].as<float>();
                    float change = obj["usd_24h_change"].as<float>();
                    if (price != c.price || change != c.change24h) {
                        c.changed = true;
                    }
                    c.price = price;
                    c.change24h = change;
                    c.valid = true;
                }
            }
            return true;
        }
        Serial.println("CryptoSource: deserializeJson() failed");
        return false;
    }
    Serial.printf("CryptoSource: HTTP error %s\n", http.errorToString(httpCode).c_str());
    http.end();
    return false;
}
