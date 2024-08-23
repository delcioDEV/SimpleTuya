#ifndef SimpleTuya_h
#define SimpleTuya_h

#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <mbedtls/aes.h>

class SimpleTuya {
public:
    SimpleTuya(const char* deviceID, const char* localKey, int port = 6668);
    void begin(const char* ssid, const char* password);
    void setDeviceIP(const char* ipAddress);
    bool getDeviceStatus(JsonDocument& doc);
    void sendCommand(const char* command);

private:
    const char* _deviceID;
    const char* _localKey;
    const char* _ipAddress;
    int _port;
    WiFiUDP _udp;

    void encryptAES(const byte* input, byte* output, const char* key, int length);
    void decryptAES(const byte* input, byte* output, const char* key, int length);
};

#endif
