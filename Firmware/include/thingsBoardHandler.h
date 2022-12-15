

#include "ThingsBoard.h"
#include <WiFi.h>
void processProvisionResponse(const Provision_Data &data);
void loopThingsBoard(String topic, String Data);
void setupTbProvision();
void publishAnnounce();
void publishInfo();

float cu_amp = 0;
float cu_amp_B = 0;
float cu_volt = 0;
double kWh = 0;
double kWh15 = 0;
double kWh24 = 0;
double kWh_2 = 0;
double kWh15_2 = 0;

double total_energy_A = 0;
double total_energy_B = 0;
double total_energy_export = 0;
double total_energy_24h_import = 0;
double total_energy_24h_export = 0;
double total_energy_15m_import = 0;
double total_energy_15m_export = 0;

String ACCESS_TOKEN = "";
String chipId;
String firmVer = "BDEM-1.0";
#define THINGSBOARD_SERVER "thingsboard.cloud"
#define THINGSBOARD_PORT 1883

const char *provisionDeviceKey = "0ug60fv4tbz3m7vgqkvv";
const char *provisionDeviceSecret = "lgej69557ryg8fmvor1z";

String topic_announce = "devices/bdemono-";
String topic_info = "devices/bdemono-";
String devID = "bdemono-";

// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD 115200

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard client provision instance
ThingsBoard tb_provision(espClient);

// Initialize Thingsboard instance
ThingsBoard tb(espClient);

// the Wifi radio's status
int status = WL_IDLE_STATUS;

unsigned long previous_processing_time;

// Statuses for provisioning
bool provisionRequestSent = false;
volatile bool provisionResponseProcessed = false;

// Struct for client connecting after provisioning
struct Credentials
{
    String client_id;
    String username;
    String password;
};

Credentials credentials;

void processProvisionResponse(const Provision_Data &data)
{
    Serial.println("Received device provision response");
    int jsonSize = measureJson(data) + 1;
    char buffer[jsonSize];
    serializeJson(data, buffer, jsonSize);
    Serial.println(buffer);
    String msg = (String)buffer;

    StaticJsonDocument<300> doc;

    // Parse JSON object
    DeserializationError error = deserializeJson(doc, msg);
    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    const char *credentialsValue = doc["credentialsValue"];
    const char *status = doc["status"];
    String STATUS = status;
    if (STATUS.indexOf("SUCCESS") > -1)
    {
        ACCESS_TOKEN = (String)credentialsValue;
        Serial.print("Access Token Received: ");
        // Print values.
        Serial.println(ACCESS_TOKEN);
        Serial.println(status);
        preferences.begin("my-app", false);
        preferences.putString("ACCESS_TOKEN", credentialsValue);
        preferences.end();
        Serial.println("Preferences Value Saved");
    }

    if (strncmp(data["status"], "SUCCESS", strlen("SUCCESS")) != 0)
    {
        Serial.print("Provision response contains the error: ");
        Serial.println(data["errorMsg"].as<String>());
        provisionResponseProcessed = true;
        return;
    }
    if (strncmp(data["credentialsType"], "ACCESS_TOKEN", strlen("ACCESS_TOKEN")) == 0)
    {
        credentials.client_id = "";
        credentials.username = data["credentialsValue"].as<String>();
        credentials.password = "";
    }
    if (strncmp(data["credentialsType"], "MQTT_BASIC", strlen("MQTT_BASIC")) == 0)
    {
        JsonObjectConst credentials_value = data["credentialsValue"].as<JsonObjectConst>();
        credentials.client_id = credentials_value["clientId"].as<String>();
        credentials.username = credentials_value["userName"].as<String>();
        credentials.password = credentials_value["password"].as<String>();
    }
    if (tb_provision.connected())
    {
        tb_provision.disconnect();
    }
    provisionResponseProcessed = true;
}

const Provision_Callback provisionCallback(processProvisionResponse);

void loopThingsBoard(String topic, String Data)
{

    if (!tb.connected())
    {
        // Connect to the ThingsBoard
        Serial.print("Connecting to: ");
        Serial.print(THINGSBOARD_SERVER);
        Serial.print(" with token ");
        Serial.println(ACCESS_TOKEN);
        if (!tb.connect(THINGSBOARD_SERVER, ACCESS_TOKEN.c_str()))
        {
            Serial.println("Failed to connect");
            return;
        }
    }
    if (tb.connected() == false)
    {
        /* code */
        delay(5000);
        if (tb.connected() == false)
            delay(5000);
    }

    Serial.println("Sending data...");

    // Uploads new telemetry to ThingsBoard using MQTT.
    // See https://thingsboard.io/docs/reference/mqtt-api/#telemetry-upload-api
    // for more details
    if (tb.sendTelemetryString(topic.c_str(), Data.c_str()))
    {
        Serial.println("Published Successfully");
        tb.loop();
    }
}

void setupTbProvision()
{

    topic_announce = "devices/bdemono-" + ss.getDeviceID() + "/announce";
    topic_info = "devices/bdemono-" + ss.getDeviceID() + "/info";
    devID = "bdemono-" + ss.getDeviceID();
    preferences.begin("my-app", false);
    ACCESS_TOKEN = preferences.getString("ACCESS_TOKEN");
    Serial.print("Access Token: ");
    Serial.println(ACCESS_TOKEN);
    if (ACCESS_TOKEN.length() > 15)
    {
        Serial.println("ALREADY HAVE VALID ACCESS TOKEN " + (String)ACCESS_TOKEN);
    }
    else
    {
        while (millis() - previous_processing_time < 20000)
        {
            previous_processing_time = millis();
            if (!provisionRequestSent)
            {
                if (!tb_provision.connected())
                {
                    // Connect to the ThingsBoard
                    Serial.print("Connecting to: ");
                    Serial.print(THINGSBOARD_SERVER);
                    if (!tb_provision.connect(THINGSBOARD_SERVER, "provision", THINGSBOARD_PORT))
                    {
                        Serial.println("Failed to connect");
                        return;
                    }
                    if (tb_provision.Provision_Subscribe(provisionCallback))
                    {
                        if (tb_provision.sendProvisionRequest(devID.c_str(), provisionDeviceKey, provisionDeviceSecret))
                        {
                            provisionRequestSent = true;
                            Serial.println("Provision request was sent!");
                        }
                    }
                }
            }
            else if (provisionResponseProcessed)
            {
                if (!tb.connected())
                {
                    // Connect to the ThingsBoard
                    Serial.print("Connecting to: ");
                    Serial.println(THINGSBOARD_SERVER);
                    if (!tb.connect(THINGSBOARD_SERVER, credentials.username.c_str(), THINGSBOARD_PORT, credentials.client_id.c_str(), credentials.password.c_str()))
                    {
                        Serial.println("Failed to connect");
                        Serial.println(credentials.client_id.c_str());
                        Serial.println(credentials.username.c_str());
                        Serial.println(credentials.password.c_str());
                        return;
                    }
                    else
                    {
                        Serial.println("Connected!");
                    }
                }
                else
                {

                    ESP.restart();
                    break;
                }
            }

            if (!provisionResponseProcessed)
            {
                tb_provision.loop();
            }
            else
            {
                tb.loop();
            }
        }
    }
}

void publishAnnounce()
{
    Serial.println("##### PUBLISHING ON ANNOUNCE ####");
    String device_annou = "{\"id\":\"" + ss.getMACAddress() + "\",\"model\":\"MONO\",\"mac\":\"" + ss.getDeviceID() + "\",\"firmwareversion\":\"" + firmVer + "\",\"cups\":\"" + CUPS + "\",\"Power_supplier_secret_key\":\"" + powerSupplierNumber + "\"}";
    // publishMQTT(topic_announce.c_str(), device_info.c_str());
    Serial.println(device_annou);
    loopThingsBoard(topic_announce.c_str(), device_annou.c_str());

    publishMQTT(topic_announce.c_str(), device_annou.c_str());
}

void publishInfo()
{

    Serial.println("##### PUBLISHING ON INFO ####");

    if (!tb.connected())
    {
        // Connect to the ThingsBoard
        Serial.print("Connecting to: ");
        Serial.print(THINGSBOARD_SERVER);
        Serial.print(" with token ");
        Serial.println(ACCESS_TOKEN);
        if (!tb.connect(THINGSBOARD_SERVER, ACCESS_TOKEN.c_str()))
        {
            Serial.println("Failed to connect");
            return;
        }
    }
    if (tb.connected() == false)
    {
        /* code */
        delay(5000);
        if (tb.connected() == false)
            delay(5000);
    }

    Serial.println("Sending data...");

    tb.sendTelemetryInt("total_energy_kwh_A_15", total_energy_15m_import);
    tb.sendTelemetryInt("total_energy_kwh_A", total_energy_A);
    tb.sendTelemetryFloat("power_A", emon1.apparentPower);
    tb.sendTelemetryFloat("pf_A", emon1.powerFactor);
    tb.sendTelemetryFloat("voltage", cu_volt);
    tb.sendTelemetryFloat("current_A", cu_amp);

    tb.sendTelemetryInt("total_energy_kwh_B_15", total_energy_15m_import);
    tb.sendTelemetryInt("total_energy_kwh_B", total_energy_B);
    tb.sendTelemetryFloat("power_B", emon2.apparentPower);
    tb.sendTelemetryFloat("pf_B", emon2.powerFactor);
    tb.sendTelemetryFloat("current_B", cu_amp_B);
}
