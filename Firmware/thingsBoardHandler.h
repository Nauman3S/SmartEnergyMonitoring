#include "ThingsBoard.h"

#define THINGSBOARD_SERVER "thingsboard.cloud"
#define THINGSBOARD_PORT 1883

const char *provisionDeviceKey = "yvpu40czv41xurrraj5d";
const char *provisionDeviceSecret = "pi02ksnlbcq5zv9ta0f9";
const char *deviceName = "BDE_Monophase_v_1";

ThingsBoard tb_provision(wclient);

// Initialize Thingsboard instance
ThingsBoard tb(wclient);

// the Wifi radio's status
int status = WL_IDLE_STATUS;

unsigned long previous_processing_time;

// Statuses for provisioning
bool provisionRequestSent = false;
volatile bool provisionResponseProcessed = false;
uint8_t prov_status = 0;
// Struct for client connecting after provisioning
struct Credentials
{
    String client_id;
    String username;
    String password;
};

Credentials credentials;

void setupThingsBoardProvisioning()
{
    // initialize serial for debugging
    prov_status = readProvisionStatus();
    if (prov_status == 0)
    {
        previous_processing_time = millis();
    }
    else
    {
        String creds = readmqttCreds();
        clientID_mqtt = ss.StringSeparator(creds, ';', 0);
        user_mqtt = ss.StringSeparator(creds, ';', 1);
        pass_mqtt = ss.StringSeparator(creds, ';', 2);
    }
}

void processProvisionResponse(const Provision_Data &data)
{
    Serial.println("Received device provision response");
    int jsonSize = JSON_STRING_SIZE(measureJson(data));
    char buffer[jsonSize];
    serializeJson(data, buffer, jsonSize);
    Serial.println(buffer);
    if (strncmp(data["status"], "SUCCESS", strlen("SUCCESS")) != 0)
    {
        Serial.print("Provision response contains the error: ");
        Serial.println(data["errorMsg"].as<String>());
        writeProvisionStatus(0);
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
        // JsonObject credentials_value = data["credentialsValue"].as<JsonObject>();
        JsonObjectConst credentials_value = data["credentialsValue"].as<JsonObjectConst>();
        credentials.client_id = credentials_value["clientId"].as<String>();
        credentials.username = credentials_value["userName"].as<String>();
        credentials.password = credentials_value["password"].as<String>();

        thingsBoardProvData(credentials.client_id, credentials.username, credentials.password);

        writeProvisionStatus(1);
    }
    if (tb_provision.connected())
    {
        tb_provision.disconnect();
    }
    provisionResponseProcessed = true;
}

const Provision_Callback provisionCallback(processProvisionResponse);

void loopThingsBoardProvisioning()
{
    if (prov_status == 0)
    {
        if (millis() - previous_processing_time >= 1000)
        {
            previous_processing_time = millis();
            if (WiFi.status() != WL_CONNECTED)
            {
                reconnect();
            }

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
                        if (tb_provision.sendProvisionRequest(deviceName, provisionDeviceKey, provisionDeviceSecret))
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
                    // Serial.println("Sending telemetry...");
                    // tb.sendTelemetryInt("temperature", 22);
                    // tb.sendTelemetryFloat("humidity", 42.5);
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