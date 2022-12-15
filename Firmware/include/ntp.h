#include <NTPClient.h>
// change next line to use with another board/shield
#include <WiFi.h>
// #include <WiFi.h> // for WiFi shield
// #include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>
byte hour = 0;
byte minu = 0;
byte sec = 0;

WiFiUDP ntpUDP;

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 2        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void setupNTP();
void loopNTP();
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600 * 1, 60000 * 2);
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 0*60*60, 60*60*1000);

void setupNTP()
{
    timeClient.begin();
}

void loopNTP()
{

    if (timeClient.forceUpdate() == false)
    {
        delay(1000);
    }

    Serial.println(timeClient.getFormattedTime());
    Serial.println(timeClient.getEpochTime());
    hour = timeClient.getHours();
    minu = timeClient.getMinutes();
    sec = timeClient.getSeconds();
    Serial.println("Hour: " + (String)hour);
    Serial.println("Min: " + (String)minu);
    Serial.println("Sec: " + (String)sec);
    delay(1000);
}
