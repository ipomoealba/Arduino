#include"rfid1.h"
#include "Arduino.h"

#include "ESP8266.h"
#define SSID "nccudct"
#define PASSWORD "nccudctproject"
#define HOST_NAME "10.0.1.13"
#define HOST_PORT (8000)
/*
   #define SSID "usar_chen"
   #define PASSWORD "27044360"
*/

#define DEBUGPIN 12
#define DEBUG true

//SoftwareSerial eSerial(55, 56); // (RX:D10, TX:D11)
ESP8266 wifi(Serial1);

RFID1 rfid;
char serNum[5];
//bool checker = false;
bool hasSendtoServer = false;
bool endPoint = false;
bool connectToWifi = false;
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  if (connectToWifi) {
    wifiConnector();
    if (wifi.disableMUX()) {
      Serial.print("single ok\r\n");
    } else {
      Serial.print("single err\r\n");
    }
  }


}
bool checker = false;
void loop() {
  String request = httpReciver();
  if (request.length() > 0)
  {
    Serial.println("\r\n=====    request    =====");
    Serial.println(request);
    Serial.println("=====  request end  =====");
  }
  int right = 0;
  if (!checker) {
    for (int i = 0; i < 9; i++) {
      if (checkRFID(i)) {
        right ++;
      } else {
        checker = false;
      }

    }
    if (right >= 5) {
      checker = true;

    }
  } else {
    if (!hasSendtoServer) {
      if (connectToWifi) {
        getRequest();
      }
      Serial.println("Done");
      hasSendtoServer = true;
    }
  }


}
//rfid.begin(irq,sck,mosi,miso,nss,rst);

bool checkRFID(int i) {
  if (i == 0) {
    rfid.begin(2, 4, 5, 7, 3, 6);
  }
  if (i == 1) {
    rfid.begin(2, 4, 5, 8, 3, 6);
  }
  if (i == 2) {
    rfid.begin(2, 4, 5, 9, 3, 6);
  }
  if (i == 3) {
    rfid.begin(2, 4, 5, 31, 3, 6);
  }
  if (i == 4) {
    rfid.begin(2, 4, 5, 11, 3, 6);
  }
  if (i == 5) {
    rfid.begin(2, 4, 5, 12, 3, 6);
  }
  if (i == 6) {
    rfid.begin(2, 4, 5, 10, 3, 6);
  }
  if (i == 7) {
    rfid.begin(2, 4, 5, 33, 3, 6);
  }
  if (i == 8) {
    rfid.begin(2, 4, 5, 35, 3, 6);
  }
  rfid.init();

  uchar status;
  uchar str[MAX_LEN];
  // Search card, return card types
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
    Serial.println();
    Serial.println(i);
    Serial.println();
    memcpy(serNum, str, 5);
    rfid.showCardID(serNum);
    //show the card ID
    uchar* id = serNum;
    //    8F0F7729
    //    this code depreciate
    if ( id[0] == 0x7F && id[1] == 0xFA && id[2] == 0xC4 && id[3] == 0x29 && i == 0)
    {
      //      REMOVE THE RC522 TO ANOTHER ONE
      Serial.println();
      Serial.println("ISIS");
      Serial.println();
      //      return true;
    }
    //    A9BF56A3
    else if (id[0] == 0xA9 && id[1] == 0xBF && id[2] == 0x56 && id[3] == 0xA3 && i == 1)
    {
      Serial.println();
      Serial.println("1");
      Serial.println();
      return true;
    }
    // A3BE58A3
    else if (id[0] == 0xA3 && id[1] == 0xBE && id[2] == 0x58 && id[3] == 0xA3 && i == 2)
    {
      Serial.println();
      Serial.println("2");
      Serial.println();
      return true;
    }
    //    551258A3
    else if (id[0] == 0x55 && id[1] == 0x12 && id[2] == 0x58 && id[3] == 0xA3 && i == 3)
    {
      Serial.println();
      Serial.println("3");
      Serial.println();
      return true;
    }
    else if (id[0] == 0xC5 && id[1] == 0xB1 && id[2] == 0xB1 && id[3] == 0x73 && i == 4)
    {
      Serial.println();
      Serial.println("4");
      Serial.println();
      return true;
    }
    //    690B57A3
    else if (id[0] == 0x69 && id[1] == 0x0B && id[2] == 0x57 && id[3] == 0xA3 && i == 5)
    {
      Serial.println();
      Serial.println("5");
      Serial.println();
      return true;
    }
    //    60AA1FD9
    else if (id[0] == 0x60 && id[1] == 0xAA && id[2] == 0x1F && id[3] == 0xD9 && i == 6)
    {
      Serial.println();
      Serial.println("6");
      Serial.println();
      return true;
    }
    //    4C7F57A3
    else if (id[0] == 0x4C && id[1] == 0x7F && id[2] == 0x57 && id[3] == 0xA3 && i == 7)
    {
      Serial.println();
      Serial.println("7");
      Serial.println();
      return true;
    }
    //    D914B273
    else if (id[0] == 0xD9 && id[1] == 0x14 && id[2] == 0xB2 && id[3] == 0x73 && i == 8)
    {
      Serial.println();
      Serial.println("8");
      Serial.println();
      return true;
    }
    else {
      Serial.println(F("Not Correct!"));
      Serial.println();
      return false;
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
  //    while (true)
  //    {
  //         if (wifi.disableMUX())
  //         {
  //             Serial.print("single ok\r\n");
  //             break;
  //         }
  //        else
  //        {
  //             Serial.print("single err\r\n");
  //             delay(1000);
  //         }
  //
  //     }
  uint8_t buffer[128] = {0};

  if (wifi.createTCP(HOST_NAME, HOST_PORT))
  {
    Serial.print("Create Tcp ... ok\r\n");
  }
  else
  {
    Serial.print("Create Tcp ... err\r\n");
  }
  const char *data = "GET /?STATUS=COMPLETE\r\n\r\n";
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



