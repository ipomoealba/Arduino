#include"rfid1.h"
#include "Arduino.h"

#include "ESP8266.h"
#include <SoftwareSerial.h>
#include <Servo.h>

#define SSID "nccudct"
#define PASSWORD "nccudctproject"
#define HOST_NAME "10.0.1.18"
#define HOST_PORT (8000)
#define SERVO_PIN 9

SoftwareSerial eSerial(10, 11); // (RX:D10, TX:D11)
ESP8266 wifi(eSerial);

RFID1 rfid;
char serNum[5];
//bool checker = false;
bool hasSendtoServer = false;
bool endPoint = false;
bool checker = false;

Servo servo1;
int nowAngle = 180;
int openAngle = 0;
int closeAngle = 0;
int vel = 10;
int ledPin = 12;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  servo1.attach(SERVO_PIN);
  servo1.write(nowAngle);
  delay(1000);
  servo1.detach();
  eSerial.begin(9600);
  wifiConnector();
  if (wifi.disableMUX()) {
    Serial.print("single ok\r\n");
  } else {
    Serial.print("single err\r\n");
  }

}

void loop() {
  digitalWrite(ledPin, HIGH);
  String request = httpReciver();
  if (request.length() > 0)
  {
    Serial.println("\r\n=====    request    =====");
    Serial.println(request);
    Serial.println("=====  request end  =====");
    if (request.indexOf("STATUS=OPEN") != -1)
    {

      servo1.attach(SERVO_PIN);
      while (nowAngle != openAngle)
      {
        if (nowAngle > openAngle)
        {

          nowAngle -= 1;
          servo1.write(nowAngle);
          delay(100 / vel);
        }

        if (nowAngle < openAngle)
        {
          nowAngle += 1;
          servo1.write(nowAngle);
          delay(100 / vel);
        }
      }
      servo1.detach();
      Serial.print("Serval_Write!!!");
    } else if (request.indexOf("STATUS=CLOSE") != -1)
    {
      servo1.attach(SERVO_PIN); //attach servo
      while (nowAngle != closeAngle)
      {
        if (nowAngle > closeAngle)
        {

          nowAngle -= 1;
          servo1.write(nowAngle);
          delay(100 / vel);
        }

        if (nowAngle < closeAngle)
        {
          nowAngle += 1;
          servo1.write(nowAngle);
          delay(100 / vel);
        }
      }
      servo1.detach();
      Serial.print("Serval_Write!!!");
    }

  }

  if (!checker) {

    if (checkRFID(0)) {
      checker = true;
      servo1.attach(SERVO_PIN);
      while (nowAngle != openAngle)
      {
        if (nowAngle > openAngle)
        {

          nowAngle -= 1;
          servo1.write(nowAngle);
          delay(100 / vel);
        }

        if (nowAngle < openAngle)
        {
          nowAngle += 1;
          servo1.write(nowAngle);
          delay(100 / vel);
        }
      }
      servo1.detach();
      Serial.print("Serval_Write!!!");
      getRequest();
      checker = false;
    }

    if (checkRFIDReset(0)) {
      servo1.attach(SERVO_PIN); //attach servo
      while (nowAngle != closeAngle)
      {
        if (nowAngle > closeAngle)
        {

          nowAngle -= 1;
          servo1.write(nowAngle);
          delay(100 / vel);
        }

        if (nowAngle < closeAngle)
        {
          nowAngle += 1;
          servo1.write(nowAngle);
          delay(100 / vel);
        }
      }
      servo1.detach();
      Serial.print("RST!!!");
      checker = false;
    }


  } else {
    //    if (!hasSendtoServer) {
    //      getRequest();
    //      Serial.println("Done");
    //      hasSendtoServer =  true;
    //    }
  }
}

bool checkRFIDReset(int i) {
  if (i == 0) {
    rfid.begin(2, 4, 5, 7, 3, 6);
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
    rfid.showCardID(serNum);//show the card ID
    uchar* id = serNum;
    //    8F0F7729
    if ( id[0] == 0x9D && id[1] == 0x59 && id[2] == 0xBE && id[3] == 0x2D && i == 0)
    {
      Serial.println();
      Serial.println("horus");
      Serial.println();
      return true;
    }

    else {
      Serial.println();
      Serial.println("Not Correct!");
      Serial.println();
      return false;
    }
  }
  Serial.println();
  Serial.println();

  rfid.halt(); //command the card into sleep mode
}
bool checkRFID(int i) {
  if (i == 0) {
    rfid.begin(2, 4, 5, 7, 3, 6);
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
    rfid.showCardID(serNum);//show the card ID
    uchar* id = serNum;
    //    8F0F7729
    if ( id[0] == 0x8F && id[1] == 0x0F && id[2] == 0x77 && id[3] == 0x29 && i == 0)
    {
      Serial.println();
      Serial.println("horus");
      Serial.println();
      return true;
    }

    else {
      Serial.println();
      Serial.println("Not Correct!");
      Serial.println();
      return false;
    }
  }
  Serial.println();
  Serial.println();

  rfid.halt(); //command the card into sleep mode
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
  // char *data = "GET /device_handshaker/?device_id=<Central> HTTP/1.1\r\n\r\n";
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


