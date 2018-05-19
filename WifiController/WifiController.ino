
#include "Arduino.h"

#include "ESP8266.h"
#include <SoftwareSerial.h>
#define SSID "nccudct"
#define PASSWORD "nccudctproject"
//#define SSID "usar_chen"
//#define PASSWORD "27044360"

#define RELAY_CONTROL_PIN 12

SoftwareSerial eSerial(10, 11); // (RX:D10, TX:D11)
ESP8266 wifi(eSerial);
void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  eSerial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  wifiConnector();
  enableMUX();
  tcpSetup();
  
}

// the loop function runs over and over again forever
void loop() {
  String request = httpReciver();

  if (request.length() > 0)
  {
    Serial.println("\r\n=====    request    =====");
    Serial.println(request);
    Serial.println("=====  request end  =====");

    if (request.indexOf("STATUS=OPEN") != -1)
    {
      Serial.println("LIGHT THE LIGHT");
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
      delay(1000);
    } else if (request.indexOf("STATUS=CLOSE") != -1)
    {
      Serial.println("CLOSE THE LIGHT");
      digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW
      delay(1000);
    }
  }
}
void wifiConnector(void)
{

    if (wifi.kick())
    {
        Serial.println(F("ESP8266 Live Check ... OK"));
    }
    else
    {
        Serial.println(F("ESP8266 Live Check ... Error"));
    }

    if (wifi.restart())
    {
        Serial.println(F("ESP8266 Restart ... Ok"));
    }
    else
    {
        Serial.println(F("ESP8266 Restart ... Error"));
    }

    if (wifi.setOprToStationSoftAP())
    {
        Serial.print(F("Set Operation Mode = STATION + AP ... OK\r\n"));
    }
    else
    {
        Serial.print(F("Set Operation Mode = STATION + AP ... Error\r\n"));
    }

    if (wifi.joinAP(SSID, PASSWORD))
    {
        Serial.print(F("Join AP ... OK\r\n"));
        Serial.print(F("IP ... "));
        Serial.println(wifi.getLocalIP().c_str());
    }
    else
    {
        Serial.print(F("Join AP ... Error\r\n"));
    }
}

void enableMUX(void)
{
    if (wifi.enableMUX())
    {
        Serial.print(F("Setup Multiple Connection ... OK\r\n"));
    }
    else
    {
        Serial.print(F("Setup Multiple Connection ... Error\r\n"));
    }
}

void tcpSetup(void)
{
    if (wifi.startTCPServer(80))
    {
        Serial.print(F("Start Tcp Server ... OK\r\n"));
    }
    else
    {
        Serial.print(F("Start Tcp Server ... Error\r\n"));
    }

    if (wifi.setTCPServerTimeout(10))
    {
        Serial.print(F("Set Tcp Server Timout 10 Seconds ... OK\r\n"));
    }
    else
    {
        Serial.print(F("Set Tcp Server Timout ... Error\r\n"));
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
            Serial.print(F("Release tcp "));
            Serial.print(mux_id);
            Serial.println(F(" ... OK"));
        }
        else
        {
            Serial.print(F("Release tcp"));
            Serial.print(mux_id);
            Serial.println(F("... Error"));
        }

        Serial.print(F("Status:["));
        Serial.print(wifi.getIPStatus().c_str());
        Serial.println(F("]"));
        return request;
    }
    return "";
}


