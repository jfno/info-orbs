#include "SunSource.h"

#include "TimeParse.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

SunSource::SunSource(float lat, float lng) : m_lat(lat), m_lng(lng) {}

bool SunSource::consumeChanged() {
    bool c = m_changed;
    m_changed = false;
    return c;
}

void SunSource::update(bool force) {
    if (force || m_firstPass || (millis() - m_lastRefresh) >= m_refreshInterval) {
        if (fetch()) {
            m_lastRefresh = millis();
            m_firstPass = false;
        } else if (force || m_firstPass) {
            // Avoid hammering on failure during the initial pass; retry next interval.
            m_lastRefresh = millis();
            m_firstPass = false;
        }
    }
}

bool SunSource::fetch() {
    String url = "https://api.sunrise-sunset.org/json?lat=" + String(m_lat, 4) +
                 "&lng=" + String(m_lng, 4) + "&formatted=0";
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, http.getStream());
        http.end();
        if (!error && doc["status"].as<String>() == "OK") {
            m_sunrise = parseIso8601Utc(doc["results"]["sunrise"].as<String>());
            m_sunset = parseIso8601Utc(doc["results"]["sunset"].as<String>());
            m_valid = m_sunrise > 0 && m_sunset > 0;
            m_changed = true;
            return m_valid;
        }
        Serial.println("SunSource: parse/status error");
        return false;
    }
    Serial.printf("SunSource: HTTP error %s\n", http.errorToString(httpCode).c_str());
    http.end();
    return false;
}
