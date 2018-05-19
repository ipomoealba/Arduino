#include"rfid1.h"
#include "Arduino.h"

#include "ESP8266.h"

#define SSID "nccudct"
#define PASSWORD "nccudctproject"

#define HOST_NAME "10.0.1.18"
#define HOST_PORT (8000)

SoftwareSerial eSerial(10, 11); // (RX:D10, TX:D11)
ESP8266 wifi(eSerial);

int trigPin1 = 6;                  //Trig Pin
int echoPin1 = 5;//Echo Pin
int trigPin2 = 4;
int echoPin2 = 3;

long duration, cm, inches;


void setup() {
  Serial.begin (9600);
  pinMode(trigPin1, OUTPUT);        // Define inputs and outputs
  pinMode(echoPin1, INPUT);        // Serial Port begin
  pinMode(trigPin2, OUTPUT);        // Define inputs and outputs
  pinMode(echoPin2, INPUT);
  eSerial.begin(9600);
  wifiConnector();
  if (wifi.disableMUX()) {
    Serial.print(F("single ok\r\n"));
  } else {
    Serial.print(F("single err\r\n"));
  }

}

void loop()
{
  Serial.println("1");
  long sensor1 = getDistance(trigPin1, echoPin1);
  Serial.println("2");
  long sensor2 = getDistance(trigPin1, echoPin1);
  Serial.println(sensor1);
  Serial.println(sensor2);
  getDistance(trigPin2, echoPin2);
  if (sensor1 < 5 && sensor2 < 5) {
    getRequest();
  }
  delay(250);
}

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);     // 給 Trig 高電位，持續 10微秒
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  pinMode(echoPin, INPUT);             // 讀取 echo 的電位
  duration = pulseIn(echoPin, HIGH);   // 收到高電位時的時間

  cm = (duration / 2) / 29.1;       // 將時間換算成距離 cm 或 inch
  inches = (duration / 2) / 74;

  Serial.print("Distance : ");
  //  Serial.print(inches);
  //  Serial.print("in,   ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  return cm;
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
  const char *data = "GET /?name=TreasureBox&STATUS=COMPLETE HTTP/1.1\r\n\r\n";
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



