#include <SimpleTuya.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* deviceID = "YOUR_DEVICE_ID";
const char* localKey = "YOUR_LOCAL_KEY";

SimpleTuya tuya(deviceID, localKey);

void setup() {
    Serial.begin(115200);
    tuya.begin(ssid, password);

    const char* ipAddress = "DEVICE_IP_ADDRESS"; // Substitua pelo IP do dispositivo Tuya
    tuya.setDeviceIP(ipAddress);
}

void loop() {
    DynamicJsonDocument doc(1024);
    if (tuya.getDeviceStatus(doc)) {
        Serial.println("Device status:");
        serializeJson(doc, Serial);
        Serial.println();
    } else {
        Serial.println("Failed to get device status.");
    }
    delay(5000); // Consultar a cada 5 segundos
}
