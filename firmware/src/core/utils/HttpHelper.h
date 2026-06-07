#ifndef HTTP_HELPER_H
#define HTTP_HELPER_H

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>

class HttpHelper {
public:
    // Prepares the HTTP client to collect the Transfer-Encoding header.
    // MUST be called before http.GET() / http.POST().
    static void prepare(HTTPClient &http) {
        const char *keys[] = {"Transfer-Encoding"};
        http.collectHeaders(keys, 1);
        http.setUserAgent("Mozilla/5.0");
    }

    // Safely deserializes HTTP response using chunked decoding if necessary.
    template <typename TDoc, typename... TOptions>
    static DeserializationError deserialize(HTTPClient &http, TDoc &doc, TOptions... options) {
        WiFiClient *stream = http.getStreamPtr();
        if (!stream) {
            return DeserializationError(DeserializationError::InvalidInput);
        }
        ChunkDecodingStream decodedStream(*stream);
        Stream &response = (http.header("Transfer-Encoding") == "chunked") ? (Stream &)decodedStream : (Stream &)*stream;
        return deserializeJson(doc, response, options...);
    }
};

#endif // HTTP_HELPER_H
