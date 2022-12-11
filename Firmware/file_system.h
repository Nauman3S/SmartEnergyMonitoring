#include "SPIFFS.h"

void configureFilesSystem()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
}

uint8_t readProvisionStatus()
{

    File file = SPIFFS.open("/prov.data");
    if (!file)
    {
        Serial.println("Failed to open file for reading");
    }

    Serial.println("File Content:");
    String file_content;
    while (file.available())
    {
        file_content += String((char)file.read());
    }
    file.close();
    Serial.println(file_content);
    if (file_content.indexOf("0") >= 0)
    {
        return 0;
    }
    else if (file_content.indexOf("1") >= 0)
    {
        return 1;
    }
}

void writeProvisionStatus(uint8_t status)
{
    File file = SPIFFS.open("/prov.data", FILE_WRITE);
    String status_value = String(status);
    if (!file)
    {
        Serial.println("There was an error opening the file for writing");
    }

    if (file.print(status_value))
    {
        Serial.println("File was written");
    }
    else
    {
        Serial.println("File write failed");
    }

    file.close();
}

void thingsBoardProvData(String mqttclientID, String mqttUsername, String mqttPassword)
{
    File file = SPIFFS.open("/thingsboard.data", FILE_WRITE);
    String mqttCreds = mqttclientID + String(";") + mqttUsername + String(";") + mqttPassword;
    if (!file)
    {
        Serial.println("There was an error opening the file for writing");
    }

    if (file.print(mqttCreds))
    {
        Serial.println("File was written");
    }
    else
    {
        Serial.println("File write failed");
    }

    file.close();
}

String readmqttCreds()
{

    File file = SPIFFS.open("/thingsboard.data");
    if (!file)
    {
        Serial.println("Failed to open file for reading");
    }

    Serial.println("File Content:");
    String file_content;
    while (file.available())
    {
        file_content += String((char)file.read());
    }
    file.close();
    Serial.println(file_content);
    return file_content;
}
