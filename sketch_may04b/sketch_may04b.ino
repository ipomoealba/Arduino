#include"rfid1.h"
RFID1 rfid;
char serNum[5];
//bool checker = false;
bool hasSendtoServer = false;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 3; i++) {
    checkRFID(i);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
for (int i = 0; i < 3; i++) {
    checkRFID(i);
  }
}
void checkRFID(int i) {
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
    rfid.begin(2, 4, 5, 10, 3, 6);
  }
  if (i == 4) {
    rfid.begin(2, 4, 5, 11, 3, 6);
  }
  if (i == 5) {
    rfid.begin(2, 4, 5, 12, 3, 6);
  }
  if (i == 6) {
    rfid.begin(2, 4, 5, 13, 3, 6);
  }
  delay(100);
  rfid.init();
  uchar status;
  uchar str[MAX_LEN];
  // Search card, return card types
  status = rfid.request(PICC_REQIDL, str);
  if (status != MI_OK)
  {
    return;
  }
  // Show card type
  rfid.showCardType(str);
  //Prevent conflict, return the 4 bytes Serial number of the card
  status = rfid.anticoll(str);
  if (status == MI_OK)
  {
    memcpy(serNum, str, 5);
    rfid.showCardID(serNum);//show the card ID
    Serial.println();
    Serial.println();
  }
  delay(100);
  rfid.halt(); //command the card into sleep mode
} 
