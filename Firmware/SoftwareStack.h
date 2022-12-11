// Software Stack

class SoftwareStack
{
public:
#define DebugPrintL(x) Serial.println(x)
#define DebugPrint(x) Serial.print(x)

    String StringSeparator(String data, char separator, int index);
    char *StrToCharArray(String data);
    String getMACAddress();
    String getTopicWithMAC(String before_mac, String after_mac);

    SoftwareStack();

private:
    char buf[100];

    int sensorVCounter = 0;
};
SoftwareStack::SoftwareStack()
{
}

char *SoftwareStack::StrToCharArray(String data)
{
    data.toCharArray(this->buf, data.length() + 1);
    return this->buf;
}

String SoftwareStack::StringSeparator(String data, char separator, int index)
{

    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        if (data.charAt(i) == separator || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }

    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

String SoftwareStack::getMACAddress()
{
    String TrackerID = String(WiFi.macAddress());
    TrackerID = StringSeparator(TrackerID, ':', 0) + StringSeparator(TrackerID, ':', 1) + StringSeparator(TrackerID, ':', 2) + StringSeparator(TrackerID, ':', 3) +
                StringSeparator(TrackerID, ':', 4) + StringSeparator(TrackerID, ':', 5);
    return TrackerID;
}
String SoftwareStack::getTopicWithMAC(String before_mac, String after_mac)
{

    String t = before_mac + getMACAddress() + after_mac;

    return t;
}