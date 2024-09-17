#define ESP32_BT_NAME  "ESP32_BT_TFT"   // 藍牙顯示名稱
#define ESP32_BT_PIN   "1234"   // 藍牙 Pin code (0000 or 1234)

#include "BluetoothSerial.h"
#include "ELMduino.h"

BluetoothSerial SerialBT;
#define ELM_PORT   SerialBT
#define DEBUG_PORT Serial

// byte clientMAC[6] = {0xA0, 0xB7, 0x65, 0x58, 0xF9, 0x8E };  // ESP32_BT_EZStartKit
// byte clientMAC[6] = {0x00, 0x10, 0xCC, 0x4F, 0x36, 0x03 };  // ODB2 (Phone)
// byte clientMAC[6] = {0x00, 0x1D, 0xA5, 0x68, 0x98, 0x8A };  // ODB2 (ESP32)
byte clientMAC[6] = {0x00, 0x10, 0xCC, 0x4F, 0x36, 0x03 };  // ODB2 (Class)

bool connected;

ELM327 myELM327;

uint32_t rpm = 0;
uint32_t kph = 0;
// const uint8_t FUEL_TANK_LEVEL_INPUT = 47;

int cnt1 = 0;
int cnt2 = 0;
int cnt3 = 0;
int cnt4 = 0;

#include "SPI.h"
#include "TFT_eSPI.h"
#include "Free_Fonts.h" // Include the header file attached to this sketch

#define TFT_GREY 0x7BEF
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

void setup() {
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLUE);
  
  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setTextSize(1);
  tft.println("Init!");

  DEBUG_PORT.begin(115200);
  //SerialBT.setPin("1234");
  // ELM_PORT.begin("ArduHUD", true);

  // BT
  ELM_PORT.begin(ESP32_BT_NAME, true);  // set master
  Serial.println("BT connect prepare...");
  tft.print("BT connect prepare...");
  connected = ELM_PORT.connect(clientMAC);

  if(connected) {
    Serial.println("connected!!!");
    tft.println("connected!!!");
  }
  else {
    tft.println(" NOT connected!!!");
  }

  // ELM327
  /*
  Serial.println("Attempting to connect to ELM327...");
  tft.println("Attempting to connect to ELM327...");
  if (!ELM_PORT.connect("OBD2")) {
    DEBUG_PORT.println("Couldn't connect to OBD scanner - Phase 1");
    tft.println("Couldn't connect to OBD scanner - Phase 1");
    while(1);
  }
  */

  tft.print("Attempting ELM327 begin...");  
  if (!myELM327.begin(ELM_PORT)) {
    Serial.println("Couldn't connect to OBD scanner - Phase 2");
    tft.println("Fail!!!");
    myELM327.printError();
    while (1);
  }
  else {
    tft.println("Success!!!");
  }


  tft.println("setup end. Connected to ELM327");

}


void loop() {
  float fuelLevel = 0; // in %

  float value1 = random(0, 1000) / 10;
  int value2 = random(0, 100);
  String msg = String(fuelLevel);

  // show title
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setFreeFont(FF18);
  tft.drawCentreString("ELM327 Running!", 160, 10, 2);
    
  // check 1
  /*
  float tempRPM = myELM327.rpm();

  
    tft.print("myELM327.nb_rx_state:");
    tft.println(myELM327.nb_rx_state);

  if (myELM327.nb_rx_state == ELM_SUCCESS) {
    rpm = (uint32_t)tempRPM;
    Serial.print("RPM: "); Serial.println(rpm);

    tft.print("RPM: ");
    tft.println(rpm);
      
    msg = String(rpm);

    tft.setTextColor(TFT_WHITE, TFT_RED);
    tft.drawCentreString(" RPM ", 160, 23, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setFreeFont(FF24);
    tft.drawCentreString(msg, 160, 80, 8);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawCentreString(" rounds/min ", 160, 190, 4);
  }
  else if (myELM327.nb_rx_state != ELM_GETTING_MSG) {
    myELM327.printError();
    tft.println("error(1a)!");
  }
  else {
    tft.println("error(1b)!");
  }
  */

  // check 2
  int32_t tempKPH = myELM327.kph();
  cnt1++;
  if (myELM327.nb_rx_state == ELM_SUCCESS) {
    cnt2++;
    kph = (uint32_t)tempKPH;
    // Serial.print("KPH: "); Serial.println(kph);

    tft.print("KPH: ");
    tft.println(kph);
      
    msg = " " + String(kph) + " ";

    tft.setTextColor(TFT_WHITE, TFT_RED);
    tft.drawCentreString(" KPH ", 160, 30, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setFreeFont(FF24);
    tft.drawCentreString(msg, 160, 80, 8);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawCentreString(" KM/h ", 160, 160, 4);
  }
  else if (myELM327.nb_rx_state != ELM_GETTING_MSG) {
    cnt3++;
    myELM327.printError();
    tft.println("error(2a)!");
  }
  else {
    cnt4++;
  }


  // check
  /*
  if(myELM327.queryPID(SERVICE_01, FUEL_TANK_LEVEL_INPUT)) {
    fuelLevel = myELM327.findResponse() * 100.0 / 255.0;
    Serial.print("FuelLevel: ");
    Serial.println(fuelLevel);  
    tft.print("FuelLevel: ");
    tft.println(fuelLevel);

    msg = String(fuelLevel);
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_RED);
    tft.drawCentreString(" Fuel Level ", 160, 23, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    //tft.setFreeFont(FF18);                 // Select the font
    //tft.drawString(sFF24, 160, 60, GFXFF);// Print the string name of the font
    tft.setFreeFont(FF24);
    // tft.drawString(msg, 160, 80, GFXFF);
    tft.drawCentreString(msg, 160, 80, 8);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawCentreString(" KM/sec ", 160, 190, 4);
    tft.drawCentreString(String(millis()), 160, 220, 2);  
  }
  else {
    myELM327.printError();
    tft.println("error(3)!");
  }
  */


  // Show TFT message
  String msg2 = "state: "
  + String(myELM327.nb_rx_state)
  + "  ,"
  + String(millis())
  + "[" + String(cnt1) + " "
  + String(cnt2) + " "
  + String(cnt3) + " "
  + String(cnt4) + "]";

  tft.drawCentreString(msg2, 160, 200, 2);
}



void printError()
{
  /*
  Serial.print("Received: ");
  for (byte i = 0; i < myELM327.recBytes; i++)
    Serial.write(myELM327.payload[i]);
  Serial.println();
  
  if (myELM327.status == ELM_SUCCESS)
    Serial.println(F("\tELM_SUCCESS"));
  else if (myELM327.status == ELM_NO_RESPONSE)
    Serial.println(F("\tERROR: ELM_NO_RESPONSE"));
  else if (myELM327.status == ELM_BUFFER_OVERFLOW)
    Serial.println(F("\tERROR: ELM_BUFFER_OVERFLOW"));
  else if (myELM327.status == ELM_UNABLE_TO_CONNECT)
    Serial.println(F("\tERROR: ELM_UNABLE_TO_CONNECT"));
  else if (myELM327.status == ELM_NO_DATA)
    Serial.println(F("\tERROR: ELM_NO_DATA"));
  else if (myELM327.status == ELM_STOPPED)
    Serial.println(F("\tERROR: ELM_STOPPED"));
  else if (myELM327.status == ELM_TIMEOUT)
    Serial.println(F("\tERROR: ELM_TIMEOUT"));
  else if (myELM327.status == ELM_TIMEOUT)
    Serial.println(F("\tERROR: ELM_GENERAL_ERROR"));

  delay(100);
  */
}
