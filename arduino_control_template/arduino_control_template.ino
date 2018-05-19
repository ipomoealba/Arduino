#include "ESP8266.h"

#define SSID "test123"
#define PASSWORD "00000000"
#define HOST_NAME "172.20.10.4"
#define HOST_PORT (8000)
#define DEBUGPIN 13
#define DEBUG true

bool startHttpServer = false;

ESP8266 wifi(Serial1);

void setup(void)
{
    Serial1.begin(115200);
    Serial.begin(9600);
    wifiConnector();
    getRequest();
    // enableMUX();
    // tcpSetup();
    Serial.print(F("setup end\r\n"));
}

void loop(void)
{
    String request = httpReciver();
    if (request.length() > 0)
    {
        Serial.println("\r\n=====    request    =====");
        Serial.println(request);
        Serial.println("=====  request end  =====");
    }
    if (request.indexOf("/LED=ON") != -1)
    {
        digitalWrite(13, HIGH);
    }
    else if (request.indexOf("/LED=OFF") != -1)
    {
        digitalWrite(13, LOW);
    }
}

void wifiConnector(void)
{
    if (wifi.setOprToStationSoftAP())
    {
        Serial.print(F("to station + softap ok\r\n"));
    }
    else
    {
        Serial.print(F("to station + softap err\r\n"));
    }

    if (wifi.joinAP(SSID, PASSWORD))
    {
        Serial.print(F("Join AP success\r\n"));
        Serial.print(F("IP: "));
        Serial.println(wifi.getLocalIP().c_str());
    }
    else
    {
        Serial.print(F("Join AP failure\r\n"));
    }
}

void enableMUX(void)
{
    if (wifi.enableMUX())
    {
        Serial.print(F("multiple ok\r\n"));
    }
    else
    {
        Serial.print(F("multiple err\r\n"));
    }
}

void tcpSetup(void)
{
    if (wifi.startTCPServer(80))
    {
        Serial.print(F("start tcp server ok\r\n"));
    }
    else
    {
        Serial.print(F("start tcp server err\r\n"));
    }

    if (wifi.setTCPServerTimeout(10))
    {
        Serial.print(F("set tcp server timout 10 seconds\r\n"));
    }
    else
    {
        Serial.print(F("set tcp server timout err\r\n"));
    }
}

String httpReciver(void)
{
    uint8_t buffer[128] = {0};
    uint8_t mux_id;
    uint32_t len = wifi.recv(&mux_id, buffer, sizeof(buffer), 100);
    if (len > 0)
    {
        Serial.print(F("Status:["));
        Serial.print(wifi.getIPStatus().c_str());
        Serial.println(F("]"));

        Serial.print(F("Received from :"));
        Serial.print(mux_id);
        Serial.print(F("]\r\n"));

        String request = (char *)buffer;

        uint8_t header[] = "HTTP/1.1 200 OK\r\n"
                           "Content-Length: 24\r\n"
                           "Server: ESP8266\r\n"
                           "Content-Type: text/html\r\n"
                           "Connection: keep-alive\r\n\r\n";

        uint8_t response[] = "<p>yes your majesty</p>";
        wifi.send(mux_id, header, sizeof(header));
        wifi.send(mux_id, response, sizeof(response));

        if (wifi.releaseTCP(mux_id))
        {
            Serial.print(F("release tcp "));
            Serial.print(mux_id);
            Serial.println(F(" ok"));
        }
        else
        {
            Serial.print(F("release tcp"));
            Serial.print(mux_id);
            Serial.println(F("err"));
        }

        Serial.print(F("Status:["));
        Serial.print(wifi.getIPStatus().c_str());
        Serial.println(F("]"));
        return request;
    }
    return "";
}

void getRequest(void)
{

    uint8_t buffer[1024] = {0};

    if (wifi.createTCP(HOST_NAME, HOST_PORT))
    {
        Serial.print("create tcp ok\r\n");
    }
    else
    {
        Serial.print("create tcp err\r\n");
    }

    char *hello = "GET /device_name=<Central> HTTP/1.1\r\nConnection: close\r\n\r\n";
    wifi.send((const uint8_t *)hello, strlen(hello));

    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0)
    {
        Serial.print("Received:[");
        for (uint32_t i = 0; i < len; i++)
        {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }

    if (wifi.releaseTCP())
    {
        Serial.print("release tcp ok\r\n");
    }
    else
    {
        Serial.print("release tcp err\r\n");
    }
}
