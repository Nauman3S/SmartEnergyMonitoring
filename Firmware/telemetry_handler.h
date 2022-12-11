#include <ArduinoJson.h>

void generateJSONandPublish(double ct1_powerw, double ct1_pf, double ct1_v, double ct1_c, double ct2_powerw, double ct2_pf, double ct2_v, double ct2_c )
{
    StaticJsonDocument<512> doc;

    JsonObject wifi_sta = doc.createNestedObject("wifi_sta");
    if (WiFi.status() == WL_CONNECTED)
    {
        wifi_sta["connected"] = 1;
        wifi_sta["ssid"] = WiFi.SSID();
        wifi_sta["rssi"] = WiFi.RSSI();
    }
    else
    {
        wifi_sta["connected"] = 1;
        wifi_sta["ssid"] = "NA";
        wifi_sta["rssi"] = 0;
    }

    JsonArray emeters = doc.createNestedArray("emeters");

    JsonObject emeters_0 = emeters.createNestedObject();
    emeters_0["ctl"] = "production";
    emeters_0["powerW"] = ct1_powerw;
    emeters_0["pf"] = ct1_pf;
    emeters_0["voltageV"] = ct1_v;
    emeters_0["currentA"] = ct1_c;
    emeters_0["is valid"] = true;
    emeters_0["total energy"] = ct1_c * ct1_v;

    JsonObject emeters_1 = emeters.createNestedObject();
    emeters_1["ct2"] = "consumption";
    emeters_1["power"] = ct2_powerw;
    emeters_1["pf"] = ct2_pf;
    emeters_1["voltageV"] = ct2_v;
    emeters_1["currentA"] = ct2_c;
    emeters_1["is valid"] = true;
    emeters_1["total_energy"] = ct2_v * ct2_v;

    JsonObject stats = doc.createNestedObject("stats");
    stats["ram_total"] = 50256;
    stats["ram_free"] = 33796;
    stats["fs size"] = 233681;
    stats["fs _free"] = 155369;
    stats["uptime"] = millis();
    stats["unixtime"] = getTimestamp();
    char buffer[512];
    serializeJson(doc, buffer);
    String path = String("v1/devices/bedmono-") + ss.getDeviceID() + String("/telemetry");
    mqttClient.publish(path.c_str(), buffer);
}