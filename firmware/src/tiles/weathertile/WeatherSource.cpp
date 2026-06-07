#include "WeatherSource.h"

#include "GlobalTime.h" // for LOC_LANG
#include "config_helper.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "../../core/utils/HttpHelper.h"

WeatherSource::WeatherSource(const String &location, const String &units)
    : m_location(location), m_units(units) {}

String WeatherSource::buildUrl() {
    return "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/" +
           m_location + "/next3days?key=" + String(WEATHER_API_KEY) + "&unitGroup=" + m_units +
           "&include=days,current&iconSet=icons1&lang=" + LOC_LANG;
}

void WeatherSource::update(bool force) {
    if (force || m_firstPass || (millis() - m_lastRefresh) >= m_refreshInterval) {
        if (force) {
            int retry = 0;
            while (!fetch() && retry++ < MAX_RETRIES)
                ;
        } else {
            fetch();
        }
        m_lastRefresh = millis();
        m_firstPass = false;
    }
}

bool WeatherSource::fetch() {
    HTTPClient http;
    HttpHelper::prepare(http);
    http.begin(buildUrl());
    int httpCode = http.GET();
    if (httpCode == 200) {
        JsonDocument doc;
        DeserializationError error = HttpHelper::deserialize(http, doc);
        http.end();
        if (!error) {
            m_model.setCityName(doc["resolvedAddress"].as<String>());
            m_model.setCurrentTemperature(doc["currentConditions"]["temp"].as<float>());
            m_model.setCurrentText(doc["days"][0]["description"].as<String>());
            m_model.setCurrentIcon(doc["currentConditions"]["icon"].as<String>());
            m_model.setTodayHigh(doc["days"][0]["tempmax"].as<float>());
            m_model.setTodayLow(doc["days"][0]["tempmin"].as<float>());
            for (int i = 0; i < 3; i++) {
                m_model.setDayIcon(i, doc["days"][i + 1]["icon"].as<String>());
                m_model.setDayHigh(i, doc["days"][i + 1]["tempmax"].as<float>());
                m_model.setDayLow(i, doc["days"][i + 1]["tempmin"].as<float>());
            }
            return true;
        }
        Serial.println("WeatherSource: deserializeJson() failed for " + m_location);
        return false;
    }
    Serial.printf("WeatherSource: HTTP error %s for %s\n", http.errorToString(httpCode).c_str(), m_location.c_str());
    http.end();
    return false;
}
