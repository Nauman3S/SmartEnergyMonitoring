
#include "consts.h"
#include "headers.h"   //all misc. headers and functions
#include "MQTTFuncs.h" //MQTT related functions

#include "webApp.h" //Captive Portal webpages
#include <FS.h>     //ESP32 File System
#include "energyHandler.h"
#include "thingsBoardHandler.h"
Neotimer ntpTimer = Neotimer(1000);    // 1 second timer
Neotimer energyTimer = Neotimer(3000); // 1 second timer

IPAddress ipV(192, 168, 4, 1);
String loadParams(AutoConnectAux &aux, PageArgument &args) // function to load saved settings
{
    (void)(args);
    File param = FlashFS.open(PARAM_FILE, "r");
    String v1 = "";
    String v2 = "";
    if (param)
    {
        Serial.println("load params func");
        aux.loadElement(param);
        param.close();
    }
    else
        Serial.println(PARAM_FILE " open failed");
    return String("");
}

String saveParams(AutoConnectAux &aux, PageArgument &args) // save the settings
{

    apPass = args.arg("apPass"); // ap pass
    apPass.trim();

    settingsPass = args.arg("settingsPass"); // ap pass
    settingsPass.trim();

    hostName = args.arg("hostname");
    hostName.trim();

    CUPS = args.arg("CUPSNumber");
    CUPS.trim();

    powerSupplierNumber = args.arg("PowerSupplier");
    powerSupplierNumber.trim();

    // The entered value is owned by AutoConnectAux of /mqtt_setting.
    // To retrieve the elements of /mqtt_setting, it is necessary to get
    // the AutoConnectAux object of /mqtt_setting.
    File param = FlashFS.open(PARAM_FILE, "w");
    portal.aux("/mqtt_setting")->saveElement(param, {"hostname", "apPass", "settingsPass", "CUPSNumber", "PowerSupplier"});
    param.close();

    // Echo back saved parameters to AutoConnectAux page.
    AutoConnectText &echo = aux["parameters"].as<AutoConnectText>();
    echo.value += "ESP host name: " + hostName + "<br>";
    echo.value += "AP Password: " + apPass + "<br>";
    echo.value += "Settings Page Password: " + settingsPass + "<br>";
    echo.value += "Cups Number: " + CUPS + "<br>";
    echo.value += "Power Supplier: " + powerSupplierNumber + "<br>";

    return String("");
}
bool loadAux(const String auxName) // load defaults from data/*.json
{
    bool rc = false;
    Serial.println("load aux func");
    String fn = auxName + ".json";
    File fs = FlashFS.open(fn.c_str(), "r");
    if (fs)
    {
        rc = portal.load(fs);
        fs.close();
    }
    else
        Serial.println("Filesystem open failed: " + fn);
    return rc;
}
uint8_t inAP = 0;

bool whileCP()
{
    // if (millis() - lastPub > updateInterval) // publish data to mqtt server
    // {

    //     ledState(ACTIVE_MODE);

    //     lastPub = millis();
    // }

    if (inAP == 0)
    {

        ledState(AP_MODE);
        inAP = 1;
    }
}

void setup() // main setup functions
{
    Serial.begin(115200);

    delay(1000);

    Serial.print("Device ID: ");
    Serial.println(ss.getMACAddress());
    setupOLED();

#if defined(ARDUINO_ARCH_ESP8266)
    FlashFS.begin();
#elif defined(ARDUINO_ARCH_ESP32)
    FlashFS.begin(true);
#endif
    loadAux(AUX_MQTTSETTING);
    loadAux(AUX_MQTTSAVE);
    AutoConnectAux *setting = portal.aux(AUX_MQTTSETTING);
    if (setting) // get all the settings parameters from setting page on esp32 boot
    {
        Serial.println("Setting loaded");
        PageArgument args;
        AutoConnectAux &mqtt_setting = *setting;
        loadParams(mqtt_setting, args);
        AutoConnectInput &hostnameElm = mqtt_setting["hostname"].as<AutoConnectInput>();
        AutoConnectInput &apPassElm = mqtt_setting["apPass"].as<AutoConnectInput>();
        AutoConnectInput &cupsNumElm = mqtt_setting["CUPSNumber"].as<AutoConnectInput>();
        AutoConnectInput &powerSupplierElm = mqtt_setting["PowerSupplier"].as<AutoConnectInput>();

        AutoConnectInput &settingsPassElm = mqtt_setting["settingsPass"].as<AutoConnectInput>();

        // vibSValueElm.value="VibS:11";

        hostName = String(hostnameElm.value);
        apPass = String(apPassElm.value);
        settingsPass = String(settingsPassElm.value);
        CUPS = String(cupsNumElm.value);
        powerSupplierNumber = String(powerSupplierElm.value);

        if (hostnameElm.value.length())
        {
            // hostName=hostName+ String("-") + String(GET_CHIPID(), HEX);
            //;
            // portal.config(hostName.c_str(), apPass.c_str());
            //  portal.config(hostName.c_str(), "123456789AP");
            config.apid = hostName + "-" + String(GET_CHIPID(), HEX);
            config.password = apPass;
            config.psk = apPass;
            // portal.config(hostName.c_str(), "123456789AP");
            Serial.println("(from hostELM) hostname set to " + hostName);
        }
        else
        {

            // hostName = String("OEE");;
            // portal.config(hostName.c_str(), "123456789AP");
            config.apid = hostName + "-" + String(GET_CHIPID(), HEX);
            config.password = apPass;
            config.psk = apPass;
            // config.hostName = hostName;//hostnameElm.value+ "-" + String(GET_CHIPID(), HEX);
            //  portal.config(hostName.c_str(), "123456789AP");
            Serial.println("hostname set to " + hostName);
        }
        config.homeUri = "/_ac";
        config.apip = ipV;

        portal.on(AUX_MQTTSETTING, loadParams);
        portal.on(AUX_MQTTSAVE, saveParams);
    }
    else
    {
        Serial.println("aux. load error");
    }
    // config.homeUri = "/_ac";
    config.apip = ipV;
    config.autoReconnect = true;
    config.reconnectInterval = 1;
    Serial.print("AP: ");
    Serial.println(hostName);
    Serial.print("Password: ");
    Serial.println(apPass);
    config.title = "BDEMono"; // set title of webapp
    Serial.print("Device Hostname: ");
    Serial.println(hostName);
    // add different tabs on homepage

    //  portal.disableMenu(AC_MENUITEM_DISCONNECT);
    server.on("/", handleRoot);
    // Starts user web site included the AutoConnect portal.

    config.auth = AC_AUTH_DIGEST;
    config.authScope = AC_AUTHSCOPE_PARTIAL;
    config.username = hostName;
    config.password = settingsPass;

    portal.config(config);
    // portal.whileCaptivePortal(whileCP);
    portal.onDetect(atDetect);
    portal.load(FPSTR(PAGE_AUTH));

    if (portal.begin())
    {
        if (!MDNS.begin("bdemono")) // starting mdns so that user can access webpage using url `esp32.local`(will not work on all devices)
        {
            Serial.println("Error setting up MDNS responder!");
            while (1)
            {
                delay(1000);
            }
        }
        MDNS.addService("http", "tcp", 80);
        Serial.println("Started, IP:" + WiFi.localIP().toString());
        ledState(AP_MODE);
    }
    else
    {
        Serial.println("Connection failed.");
        while (true)
        {
            yield();
            delay(100);
        }
    }
    setupEnergyHander();

    setupTbProvision();
    mqttConnect(); // start mqtt
    setupNTP();
    updateCUPS(CUPS);
    if (mqttClient.connected())
    {
        updateWiFiStatus(true);
        updateMQTTStatus(true);
    }
}

int k = 0;
String tempVal = "";
void loop()
{
    server.handleClient();
    portal.handleRequest();
    loopEmon();

    if (energyTimer.repeat(1))
    { // update display
        updateValue(SOLAR, realPower1);
        updateValue(RED, realPower2);
        updateValue(ENERGY_TODAY1, apparentPower1);
        updateValue(ENERGY_TODAY2, apparentPower2);
    }
    if (ntpTimer.repeat(1))
    {
        loopNTP();
    }

    if (millis() - lastPub > updateInterval) // publish data to mqtt server
    {

        ledState(ACTIVE_MODE);
        publishInfo();
        lastPub = millis();
    }
    if (!mqttClient.connected())
    {
        reconnect();
    }
    mqttClient.loop();
    if (WiFi.status() == WL_IDLE_STATUS)
    {
        ledState(IDLE_MODE);
        ESP.restart();

        delay(1000);
    }
}