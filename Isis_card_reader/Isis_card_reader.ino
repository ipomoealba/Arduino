#include"rfid1.h"
#include "Arduino.h"

#include "ESP8266.h"
//#include <SoftwareSerial.h>
#define SSID "nccudct"
#define PASSWORD "nccudctproject"
#define HOST_NAME "10.0.1.18"
#define HOST_PORT (8000)
char serNum[5];
ESP8266 wifi(Serial1);
RFID1 rfid;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);

  wifiConnector();
  if (wifi.disableMUX()) {
    Serial.print("single ok\r\n");
  } else {
    Serial.print("single err\r\n");
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  if (checkRFID()) {
    getRequest();
  }
}


bool checkRFID() {
  rfid.begin(2, 4, 5, 7, 3, 6);
  rfid.init();

  uchar status;
  uchar str[MAX_LEN];
  status = rfid.request(PICC_REQIDL, str);
  if (status != MI_OK)
  {
    return false;
  }

  // Show card type
  rfid.showCardType(str);
  //Prevent conflict, return the 4 bytes Serial number of the card
  status = rfid.anticoll(str);
  if (status == MI_OK)
  {

    memcpy(serNum, str, 5);
    rfid.showCardID(serNum);
    uchar* id = serNum;
    //    8F0F7729
    if ( id[0] == 0x7F && id[1] == 0xFA && id[2] == 0xC4 && id[3] == 0x29)
    {
      Serial.println();
      Serial.println("ISIS");
      Serial.println();
      return true;
    }

  }
  Serial.println();
  Serial.println();

  delay(100);
  rfid.halt();
  //command the card into sleep mode
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

void getRequest(void)

{
  uint8_t buffer[128] = {0};

  if (wifi.createTCP(HOST_NAME, HOST_PORT))
  {
    Serial.print("Create Tcp ... ok\r\n");
  }
  else
  {
    Serial.print("Create Tcp ... err\r\n");
  }
  const char *data = "GET /?name=Isis&STATUS=COMPLETE HTTP/1.1\r\n\r\n";
  wifi.send((const uint8_t *)data, strlen(data));

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
    Serial.print("Release Tcp ... Ok\r\n");
  }
  else
  {
    Serial.print("Release Tcp ... Error\r\n");
  }
}



