#include "SimpleTuya.h"

SimpleTuya::SimpleTuya(const char* deviceID, const char* localKey, int port) {
    _deviceID = deviceID;
    _localKey = localKey;
    _ipAddress = nullptr; // Inicializa o IP como nulo; deve ser definido usando setDeviceIP
    _port = port;
}

void SimpleTuya::begin(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    _udp.begin(6667);
}

void SimpleTuya::setDeviceIP(const char* ipAddress) {
    _ipAddress = ipAddress;
}

void SimpleTuya::encryptAES(const byte* input, byte* output, const char* key, int length) {
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, (const unsigned char*)key, 128);

    for (int i = 0; i < length; i += 16) {
        mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, input + i, output + i);
    }

    mbedtls_aes_free(&aes);
}

void SimpleTuya::decryptAES(const byte* input, byte* output, const char* key, int length) {
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, (const unsigned char*)key, 128);

    for (int i = 0; i < length; i += 16) {
        mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, input + i, output + i);
    }

    mbedtls_aes_free(&aes);
}

bool SimpleTuya::getDeviceStatus(JsonDocument& doc) {
    if (_ipAddress == nullptr) {
        Serial.println("Device IP not set. Use setDeviceIP() to set it.");
        return false;
    }

    const char* query = "{\"gwId\":\"YOUR_DEVICE_ID\",\"devId\":\"YOUR_DEVICE_ID\"}";

    byte encryptedQuery[128];
    encryptAES((byte*)query, encryptedQuery, _localKey, strlen(query));

    _udp.beginPacket(_ipAddress, _port);
    _udp.write(encryptedQuery, sizeof(encryptedQuery));
    _udp.endPacket();

    int packetSize = _udp.parsePacket();
    if (packetSize) {
        byte buffer[512];
        int len = _udp.read(buffer, 512);
        if (len > 0) {
            buffer[len] = '\0';
        }

        byte decryptedResponse[512];
        decryptAES(buffer, decryptedResponse, _localKey, len);

        deserializeJson(doc, (char*)decryptedResponse);
        return true;
    }

    return false;
}

void SimpleTuya::sendCommand(const char* command) {
    if (_ipAddress == nullptr) {
        Serial.println("Device IP not set. Use setDeviceIP() to set it.");
        return;
    }

    byte encryptedCommand[128];
    encryptAES((byte*)command, encryptedCommand, _localKey, strlen(command));

    _udp.beginPacket(_ipAddress, _port);
    _udp.write(encryptedCommand, sizeof(encryptedCommand));
    _udp.endPacket();
}
