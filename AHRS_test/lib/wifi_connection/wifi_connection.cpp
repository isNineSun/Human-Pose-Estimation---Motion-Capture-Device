#include <wifi_connection.h>
#include <WiFi.h>
#include <WiFiUdp.h>

/* WiFi network name and password */
const char * ssid = "关注永雏塔菲谢谢喵";
const char * pwd = "sxlqxyj#1702";

// IP address to send UDP data to.
// it can be ip address of the server or 
// a network broadcast address
// here is broadcast address
const char * udpAddress = "192.168.31.243";
const int udpPort = 25666;
char packet[255];

//create UDP instance
WiFiUDP udp;
static char buffer[21] = {0};
static int count = 0;
static int buf_len = 0;

void wifi_communication_send_msg_loop(float roll, float pitch, float yaw)
{
    uint8_t buffer[50] = "";
    buf_len = snprintf((char*)buffer, 21, "%6.2f,%6.2f,%7.2f", roll, pitch, yaw);
    int packetSize = udp.parsePacket();
    if (packetSize)
    {
        Serial.printf("Received %d bytes from %s, port %d", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
        int len = udp.read(packet, 255);
        if (len > 0)
        {
            packet[len] = 0;
        }
        Serial.printf("UDP packet contents: %s", packet);
        Serial.println();
    }

    udp.beginPacket (udp.remoteIP(), udp.remotePort());
    udp.write(buffer, 21);
    udp.endPacket();
    memset(buffer, 0, 50);
}

char* get_buffer()
{
    return buffer;
}

void wifi_setup()
{
    Serial.begin(115200);

    pinMode(2, OUTPUT); // 设置 GPIO 2 为输出模式
    //Connect to the WiFi network
    WiFi.begin(ssid, pwd);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) 
    {
        digitalWrite(2, HIGH);
        delay(100);
        digitalWrite(2, LOW);
        delay(100);
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(2, HIGH);
    //This initializes udp and transfer buffer
    udp.begin(udpPort);
}