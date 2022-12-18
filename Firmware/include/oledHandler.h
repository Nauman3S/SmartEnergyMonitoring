#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    //
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
uint8_t ledConnectionTry = 0;
String solar = "0";
String red = "0";
String energyToday1 = "0";
String energyToday2 = "0";
String cupsNumber = "0";
String wifiS = "W";
String mqttS = "M";

void displayOLED(String msg)
{
    display.clearDisplay();
    display.print(msg);
    display.display();
}

void displayUI()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 1);
    display.print("Solar(W)");
    display.setCursor(90, 1);
    display.print("Red(W)");
    display.setCursor(55, 1);
    display.print(wifiS);
    display.setCursor(59, 1);
    display.print(mqttS);
    display.setCursor(0, 12);
    display.print(solar);
    display.setCursor(90, 12);
    display.print(red);
    display.setCursor(0, 25);
    display.print("EnergyToday");
    display.setCursor(90, 25);
    display.print("EnergyToday");

    display.setCursor(0, 42);
    display.print(energyToday1);
    display.setCursor(90, 42);
    display.print(energyToday2);
    display.setCursor(1, 55);
    display.print(cupsNumber);
    display.display();
}

void setDisplay(uint8_t param, String text)
{
    if (param == SOLAR)
    {
        solar = text;
    }
    else if (param == RED)
    {
        red = text;
    }
    else if (param == ENERGY_TODAY1)
    {
        energyToday1 = text;
    }
    else if (param == ENERGY_TODAY2)
    {
        energyToday2 = text;
    }
    else if (param == CUPS_NUMBER)
    {
        cupsNumber = text;
    }
    else if (param == WIFI_STATUS)
    {
        wifiS = text;
    }
    else if (param == MQTT_STATUS)
    {
        mqttS = text;
    }
    displayUI();
}
void setupOLED()
{
    Wire.begin(21, 22);
    while (1)
    {
        if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
        { // Address 0x3D for 128x64
            ledConnectionTry++;
            delay(300);
        }
        else
        {
            break;
        }

        if (ledConnectionTry >= 6)
        {
            Serial.println(F("SSD1306 allocation failed. Skipping OLED connection."));
            break;
        }
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    // Display static text
    display.println("BDE Mono");
    display.display();
}