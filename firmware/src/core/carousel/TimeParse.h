#ifndef TIME_PARSE_H
#define TIME_PARSE_H

#include <Arduino.h>
#include <stdlib.h>
#include <time.h>

// Parse an ISO-8601 timestamp (e.g. "2026-06-07T12:34:56Z" or
// "2026-06-07T12:34:56+00:00") into a UTC epoch. The trailing zone suffix is
// ignored -- the APIs we use (sunrise-sunset.org, Launch Library) return UTC.
// Returns 0 on parse failure.
static inline time_t parseIso8601Utc(const String &iso) {
    struct tm t = {};
    if (strptime(iso.c_str(), "%Y-%m-%dT%H:%M:%S", &t) == nullptr) {
        return 0;
    }
    // Interpret the parsed fields as UTC by forcing TZ=UTC for the conversion.
    setenv("TZ", "UTC0", 1);
    tzset();
    return mktime(&t);
}

#endif // TIME_PARSE_H
