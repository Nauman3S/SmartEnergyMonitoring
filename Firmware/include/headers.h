#include "Arduino.h"
String serverName;
String channelId;
String userKey;
String apiKey;
String apid;
String hostName = "bdemono";
String apPass;
String settingsPass;
String CUPS="0";
String powerSupplierNumber="0";

#include <Preferences.h>
Preferences preferences;
#include <ArduinoJson.h>
DynamicJsonDocument doc(2024);
String myMac = "";
String Cstatus = "NULL";

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#endif
#define GET_CHIPID() ((uint16_t)(ESP.getEfuseMac() >> 32))
#include <AutoConnect.h>

#if defined(ARDUINO_ARCH_ESP8266)
ESP8266WebServer server;
#elif defined(ARDUINO_ARCH_ESP32)
WebServer server;
#endif
#include <ESPmDNS.h>
#include <PubSubClient.h>
#include "SoftwareStack.h"
#ifndef BUILTIN_LED
#define BUILTIN_LED 2 // backward compatibility
#endif
#if defined(ARDUINO_ARCH_ESP8266)
#ifdef AUTOCONNECT_USE_SPIFFS
FS &FlashFS = SPIFFS;
#else
#include <LittleFS.h>
FS &FlashFS = LittleFS;
#endif
#elif defined(ARDUINO_ARCH_ESP32)
#include <SPIFFS.h>
fs::SPIFFSFS &FlashFS = SPIFFS;
#endif
#include "statusLED.h"
#include "neoTimer.h"
#include "ntp.h"
#include "oledHandler.h"

unsigned long lastPub = 0;
unsigned int updateInterval = 3000;

#define PARAM_FILE "/param.json"
#define AUX_MQTTSETTING "/mqtt_setting"
#define AUX_MQTTSAVE "/mqtt_save"
#define AUX_MQTTCLEAR "/mqtt_clear"
static const char PAGE_AUTH[] PROGMEM = R"(
{
  "uri": "/auth",
  "title": "Auth",
  "menu": false,
    "auth": "digest",
  "element": [
    {
      "name": "text",
      "type": "ACText",
      "value": "AutoConnect has authorized",
      "style": "font-family:Arial;font-size:18px;font-weight:400;color:#191970"
    }
  ]
}
)";

SoftwareStack ss; // SS instance
AutoConnectConfig config;
AutoConnect portal(server);

String loggedIn = "";

String mac = (WiFi.macAddress());
char __mac[sizeof(mac)];

const char *mqtt_server = "broker.hivemq.com";
// IPAddress mqttBroker(34,214,65,82);
const int mqtt_port = 1883;
const char *mqtt_user = "testUser";
const char *mqtt_pass = "testUser@123";
String clientID_mqtt = __mac;
String user_mqtt = "test";
String pass_mqtt = "test";
const char *mqtt_client_name = __mac; //"12312312312332212";// any random alphanumeric stirng
//////////////////////////////
#define BUFFER_SIZE 250
String incoming = "";
String incomingTopic = "";
WiFiClient wclient;
PubSubClient mqttClient(wclient);

String LastUpdated = "";
String internetStatus = "Not-Connected";
String connectionMode = "WiFi";

bool atDetect(IPAddress &softapIP)
{
  Serial.println("Captive portal started, SoftAP IP:" + softapIP.toString());
  setDisplay(WIFI_STATUS,"W");
  setDisplay(MQTT_STATUS,"");

  return true;
}