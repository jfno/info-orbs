#include "LaunchSource.h"

#include "TimeParse.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

LaunchSource::LaunchSource() {}

void LaunchSource::update(bool force) {
    if (force || m_firstPass || (millis() - m_lastRefresh) >= m_refreshInterval) {
        fetch();
        m_lastRefresh = millis();
        m_firstPass = false;
    }
}

bool LaunchSource::fetch() {
    String url = "https://ll.thespacedevs.com/2.2.0/launch/upcoming/?limit=1&mode=list";
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
        // Filter to the fields we need to keep the JSON document small.
        JsonDocument doc;
        JsonDocument filter;
        filter["results"][0]["name"] = true;
        filter["results"][0]["net"] = true;
        DeserializationError error = deserializeJson(doc, http.getString(), DeserializationOption::Filter(filter));
        http.end();
        if (!error) {
            JsonObject first = doc["results"][0];
            if (!first.isNull()) {
                m_name = first["name"].as<String>();
                m_net = parseIso8601Utc(first["net"].as<String>());
                m_valid = m_net > 0;
                return m_valid;
            }
        }
        Serial.println("LaunchSource: parse error");
        return false;
    }
    Serial.printf("LaunchSource: HTTP error %s\n", http.errorToString(httpCode).c_str());
    http.end();
    return false;
}
