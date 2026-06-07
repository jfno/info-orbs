#include "FxSource.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>

FxSource::FxSource(const String &currencies) {
    int start = 0;
    while (start < (int)currencies.length()) {
        int comma = currencies.indexOf(',', start);
        if (comma == -1) {
            comma = currencies.length();
        }
        String cur = currencies.substring(start, comma);
        cur.trim();
        if (cur.length() > 0) {
            Rate r;
            r.currency = cur;
            m_rates.push_back(r);
        }
        start = comma + 1;
    }
}

FxSource::Rate *FxSource::getByCurrency(const String &currency) {
    for (auto &r : m_rates) {
        if (r.currency == currency) {
            return &r;
        }
    }
    return nullptr;
}

void FxSource::update(bool force) {
    if (m_rates.empty()) {
        return;
    }
    if (force || m_firstPass || (millis() - m_lastRefresh) >= m_refreshInterval) {
        fetch();
        m_lastRefresh = millis();
        m_firstPass = false;
    }
}

bool FxSource::fetch() {
    String url = "https://open.er-api.com/v6/latest/USD";
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, http.getStream());
        http.end();
        if (!error && doc["result"].as<String>() == "success") {
            for (auto &r : m_rates) {
                float v = doc["rates"][r.currency].as<float>();
                if (v > 0.0) {
                    if (v != r.value) {
                        r.changed = true;
                    }
                    r.value = v;
                    r.valid = true;
                }
            }
            return true;
        }
        Serial.println("FxSource: parse/result error");
        return false;
    }
    Serial.printf("FxSource: HTTP error %s\n", http.errorToString(httpCode).c_str());
    http.end();
    return false;
}
