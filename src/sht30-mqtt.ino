/*
  ESP8266 (D1 mini)+ SHT30 > MQTT
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WEMOS_SHT3X.h>

#define WIFI_SSID           "my-SSID"
#define WIFI_PASSWORD       "my-Password"

#define MQTT_SERVER         "my-MQTT-IP"
#define MQTT_CLIENT_NAME    "my-MQTT-name"
#define MQTT_TOPIC_TEMP     "sensor/d1mini/temperature"
#define MQTT_TOPIC_HUMID    "sensor/d1mini/humidity"

#define PUBLISH_RATE        10*60      // publishing rate in seconds
#define DEEP_SLEEP          true       // deep sleep then reset or idling (D0 must be connected to RST) https://github.com/nodemcu/nodemcu-devkit-v1.0

#define DEBUG               false      // debug to serial port

// --- LIBRARIES INIT ---
WiFiClient    wifi;
PubSubClient  mqtt(MQTT_SERVER, 1883, wifi);
SHT3X         sht30(0x45);

// --- SETUP ---
void setup() {
  if (DEBUG) Serial.begin(9600);

  // wifi connexion
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while ( WiFi.status() != WL_CONNECTED ) {
    if (DEBUG) Serial.println("connecting Wifi");
    delay(500);
  }
}

// --- MAIN LOOP ---
void loop() {

  // reading SHT30 sensors
  sht30.get();
  float t = sht30.cTemp;
  float h = sht30.humidity;

  // debug
  if (DEBUG) {
    Serial.print("temp: ");
    Serial.print(t);
    Serial.print(" | humid: ");
    Serial.println(h);
  }

  // mqtt (re)connexion
  while ( !mqtt.connected() ) {
    if (DEBUG) Serial.println("connecting MQTT");
    if ( !mqtt.connect(MQTT_CLIENT_NAME) ) delay(500);
  }

  // publish to mqtt
  mqtt.publish(MQTT_TOPIC_TEMP,  String(t).c_str(), true);
  mqtt.publish(MQTT_TOPIC_HUMID, String(h).c_str(), true);

  // deep sleep (then reset) or just wait (then loop)
  if (DEEP_SLEEP) {
    if (DEBUG) {
      Serial.print("sleeping ");
      Serial.print(PUBLISH_RATE);
      Serial.print(" s");
    }
    ESP.deepSleep(PUBLISH_RATE * 1e6);
  } else {
    if (DEBUG) {
      Serial.print("waiting ");
      Serial.print(PUBLISH_RATE);
      Serial.print(" s");
    }
    delay(PUBLISH_RATE * 1e3);
  }
}
