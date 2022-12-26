#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// #include <Fonts/FreeSerif12pt7b.h> //https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts#step-2

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
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Solar(W)");
    display.setCursor(50, 0);
    display.print("W");
    display.setCursor(52, 0);
    display.print("M");
    display.setCursor(100, 0);
    display.print("RED(W)");
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.print(0.0);
    display.setCursor(100, 16);
    display.print(0.0);
    display.setTextSize(2);
    display.setCursor(0, 32);
    display.print("Energy Today");
    display.setCursor(100, 32);
    display.print("Energy Today");
    display.setTextSize(1);
    display.setCursor(0, 48);
    display.print(0);
    display.setCursor(100, 48);
    display.print(0);
    display.setTextSize(2);
    display.setCursor(50, 64);
    display.print("ES- XXXXX -0F");
    display.display();
}

void updateValue(uint8_t param, String text)
{
    display.setTextSize(1);
    if (param == SOLAR)
    {

        display.setCursor(0, 16);
        display.setTextColor(BLACK);
        display.print(text);
        display.setTextColor(WHITE);
        display.print(text);
    }
    else if (param == RED)
    {
        display.setCursor(100, 16);
        display.setTextColor(BLACK);
        display.print(text);
        display.setTextColor(WHITE);
        display.print(text);
    }
    else if (param == ENERGY_TODAY1)
    {
        display.setCursor(0, 48);
        display.setTextColor(BLACK);
        display.print(text);
        display.setTextColor(WHITE);
        display.print(text);
    }
    else if (param == ENERGY_TODAY2)
    {
        display.setCursor(100, 48);
        display.setTextColor(BLACK);
        display.print(text);
        display.setTextColor(WHITE);
        display.print(text);
    }
    display.display();
}

void updateWiFiStatus(bool connected)
{
    display.setTextSize(1);
    display.setCursor(50, 0);
    display.setTextColor(BLACK);
    display.print("W");
    if (connected)
    {

        display.setTextColor(WHITE);
        display.print("W");
    }
    display.display();
}
void updateMQTTStatus(bool connected)
{
    display.setTextSize(1);
    display.setCursor(52, 0);
    display.setTextColor(BLACK);
     display.print("M");
    if (connected)
    {

        display.setTextColor(WHITE);
        display.print("M");
    }
    display.display();
}
void updateCUPS(String cups)
{

    display.setTextSize(2);
    display.setCursor(50, 64);
    display.setTextColor(BLACK);
    display.print(cups);
    display.setTextColor(WHITE);
    display.print(cups);
    display.display();
}
void displayOLED(String msg)
{
    display.clearDisplay();
    display.print(msg);
    display.display();
}
