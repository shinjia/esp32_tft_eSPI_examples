/*******************************************
flag_AQI  by Shinjia 
- v1.3  2024/02/07 modify library TFT_eSPI
- v1.2  2023/12/12
- v1.1  2023/08/28
- v1.0  2023/07/27
*********************************************/

/****** WiFi Access Point ******/

#define WLAN_SSID   "Your_Wifi_SSID"
#define WLAN_PASS   "Your_Wifi_Password"

/****** OpenData API Key ***/

#define API_KEY    "Your_API_Key"
#define SITE_NAME  "***指定地區***"  // 新莊


/****** Timer Control (delay) ******/
int timer_delay_get   = 1800000;  // 取得 AQI 資料，每半小時 30*60*1000
int timer_delay_clock = 1000;     // 更新時鐘，每秒
int timer_delay_NTP   = 3600000;  // 每小時更新 NTP，60*60*1000，

unsigned long timer_next_get;
unsigned long timer_next_clock;
unsigned long timer_next_NTP;

// #include "graphics_0.h"
#include "graphics_1.h"
#include "graphics_2.h"
#include "graphics_3.h"
#include "graphics_4.h"
#include "graphics_5.h"
#include "graphics_6.h"
#include "graphics_gauge.h"

// wifi
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// RTC
#include <ESP32Time.h>
ESP32Time rtc;

// NTP
#include <WiFi.h>
#include "time.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 8 * 3600;   // for Taipei
const int   daylightOffset_sec = 0; // 3600?

boolean is_get_NTP = false;

// tft
#include "SPI.h"
#include "TFT_eSPI.h"

TFT_eSPI tft = TFT_eSPI();

int AQI;


void get_NTP_update_RTC() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)){      
    rtc.setTimeStruct(timeinfo); 
  }
  // Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
  is_get_NTP = true;
}


void wifi_connect_tft() {
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  Serial.println("Connecting");
  tft.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tft.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
  tft.println("");
  tft.print("Connected to WiFi network with IP Address: ");
  tft.println(WiFi.localIP());
}


void show_picture(int idx) {
    switch(idx) {
      case 0: 
        // tft.drawRGBBitmap( 0,  0, (const uint16_t *)bitmap_AQI_0, 134, 240); 
        // tft.pushImage(0, 0, 134, 240, bitmap_AQI_0);
        break;
      case 1: 
        // tft.drawRGBBitmap( 0,  0, (const uint16_t *)bitmap_AQI_1, 134, 240); 
        tft.pushImage(0, 0, 134, 240, bitmap_AQI_1);
        break;
      case 2: 
        // tft.drawRGBBitmap( 0,  0, (const uint16_t *)bitmap_AQI_2, 134, 240); 
        tft.pushImage(0, 0, 134, 240, bitmap_AQI_2);
        break;
      case 3: 
        // tft.drawRGBBitmap( 0,  0, (const uint16_t *)bitmap_AQI_3, 134, 240); 
        tft.pushImage(0, 0, 134, 240, bitmap_AQI_3);
        break;
      case 4: 
        // tft.drawRGBBitmap( 0,  0, (const uint16_t *)bitmap_AQI_4, 134, 240); 
        tft.pushImage(0, 0, 134, 240, bitmap_AQI_4);
        break;
      case 5: 
        // tft.drawRGBBitmap( 0,  0, (const uint16_t *)bitmap_AQI_5, 134, 240); 
        tft.pushImage(0, 0, 134, 240, bitmap_AQI_5);
        break;
      case 6: // note
        // tft.drawRGBBitmap( 0,  0, (const uint16_t *)bitmap_AQI_6, 134, 240); 
        tft.pushImage(0, 0, 134, 240, bitmap_AQI_6);
        break;
    }
}


void show_flag(int idx) {
  uint16_t color;
  switch(idx) {
    case 0:
      color = tft.color565(255, 255, 255);
      break;
    case 1: 
      color = tft.color565(0, 153, 68);
      break;
    case 2: 
      color = tft.color565(255, 241, 0);
      break;
    case 3: 
      color = tft.color565(243, 152, 0);
      break;
    case 4: 
      color = tft.color565(230, 0, 18);
      break;
    case 5: 
      color = tft.color565(126, 27, 118);
      break;
    case 6: 
      color = tft.color565(136, 28, 33);
      break;
  }

  // flags
  tft.fillTriangle(145, 14, 145, 64, 300, 37, color );
  tft.fillRect(140, 10, 8, 230, TFT_LIGHTGREY );
  tft.drawFastVLine(142, 10, 230, TFT_DARKGREY );
  tft.fillCircle(144, 8, 8,  TFT_OLIVE);
  tft.drawCircle(144, 8, 8,  TFT_DARKGREY);
}


void show_message(int val) {
  // Draw title
  
  // 以軸心的位置為基準
  int base_x = 235;
  int base_y = 140;
  int pointer_len = 60;
  
  int theta = map(val, 0, 300, -180, 0);
  int end_x = base_x + pointer_len * cos(theta*PI/180);
  int end_y = base_y + pointer_len * sin(theta*PI/180);

  int tri_x1 = base_x + 5 * cos((theta+90)*PI/180);
  int tri_y1 = base_y + 5 * sin((theta+90)*PI/180);
  int tri_x2 = base_x + 5 * cos((theta-90)*PI/180);
  int tri_y2 = base_y + 5 * sin((theta-90)*PI/180);
  
  // draw gauge
  tft.pushImage(base_x-70, base_y-72+3, 140, 70, bitmap_gauge);
  for(int i=1; i<7; i++) {
    tft.drawFastHLine(base_x-70, base_y+i, 140, TFT_BLACK);
  }

  // draw Pointer (Triangle)
  // tft.drawLine(base_x, base_y, end_x, end_y, TFT_WHITE);
  tft.fillTriangle(end_x, end_y, tri_x1, tri_y1, tri_x2, tri_y2, TFT_CYAN);

  // center circle
  tft.fillCircle(base_x, base_y, 6, TFT_WHITE);
  tft.fillCircle(base_x, base_y, 5, TFT_BLACK);

  // other
  tft.drawFastHLine(base_x-70, base_y+40, 140, TFT_CYAN);
  tft.drawFastHLine(base_x-70, base_y+42, 140, TFT_BLUE);

  // show value
  tft.setTextSize(3);

  // Show string

  String str = String(" ") + val + String(" ");
  int16_t get_x, get_y;
  uint16_t get_w, get_h;
  
  tft.getTextBounds(str.c_str(), 100, 100, &get_x, &get_y, &get_w, &get_h);

  tft.setCursor(base_x-get_w/2+1, base_y+14);  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print(str);  
}


void show_clock() {

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  // Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
  // Draw Text
  char StringBuff[50]; //50 chars should be enough
  
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  // Line 1
  strftime(StringBuff, sizeof(StringBuff), " %H:%M:%S ", &timeinfo);
  tft.setCursor(176, 200);  
  tft.println(StringBuff);

  // Line2
  strftime(StringBuff, sizeof(StringBuff), "%Y-%m-%d", &timeinfo);
  tft.setCursor(176, 220);  
  tft.println(StringBuff); 
}


void test_all_bitmap() {
  for(int i=0; i<7; i++) {
    show_message(50*i);
    show_flag(i);
    show_picture(i);
    show_clock();
    delay(2000);
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("AQI Infomation!"); 
 
  // 先啟動 tft 做 console
  tft.init();
  tft.setRotation(3);
  tft.setTextSize(2);
  tft.fillScreen(TFT_BLUE);  // Console
  tft.setSwapBytes(true);  // 修正位元順序，將 RGB 轉成 BGR (for pushImage)
  
  delay(60000);
}


  wifi_connect_tft();

  //init and get the time
  get_NTP_update_RTC();
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  is_get_NTP = false;
  
  tft.fillScreen(TFT_BLACK);  // Clear

  test_all_bitmap();
  
  // timer
  timer_next_get = millis();
  timer_next_clock = millis();
  timer_next_NTP = millis();
}


void loop(void) {
  String json_str;

  // 若尚未取得網路時間
  if(!is_get_NTP) {
    get_NTP_update_RTC();
  }

  // Send an HTTP GET request
  if(millis() > timer_next_get) {
    timer_next_get = millis() + timer_delay_get;
    
    // Check WiFi connection status
    if(WiFi.status()!=WL_CONNECTED){
      Serial.println("WiFi Disconnected");
    }
    else {      
      String url = "https://data.moenv.gov.tw/api/v2/aqx_p_432";
      url += "?format=json";
      url += "&limit=1";
      url += "&filters=SiteName,EQ," + String(SITE_NAME);
      url += "&api_key=" + String(API_KEY);
 
      Serial.println(url);

      HTTPClient http;
      //http.setInsecure(); // 避免 SSL 問題
      http.begin(url);
      int httpCode = http.GET();  // 取得資料      
      if (httpCode == HTTP_CODE_OK) {         // 如果取得資料成功 
        json_str = http.getString();  // 取得資料內容
        Serial.println(json_str);
        Serial.print("Data size: ");
        Serial.println(json_str.length());
      }

      // JSON
      DynamicJsonDocument json_doc(json_str.length()*1.5);
      DeserializationError json_error;

      json_error = deserializeJson(json_doc, json_str);
      if (json_error) {
        Serial.println("JSON deserializeJSON error!");
        Serial.println(json_error.c_str());
      }
      else {
        String AQI_str = json_doc["records"][0]["aqi"];  // 注意資料所在位置
        AQI = AQI_str.toInt();
        Serial.print("AQI: ");
        Serial.println(AQI);
      }    
    }

    // display
    int aqi = constrain(AQI, 0, 300);
    int degree = ((aqi-1)/50) + 1;
    degree = constrain(degree, 1, 6);

    show_message(aqi);
    show_flag(degree);
    show_picture(degree);
  }

  // 間隔一段時間要求重新取得網路時間 NTP
  if(millis()>timer_next_NTP) {
    timer_next_NTP = millis() + timer_delay_NTP;
    is_get_NTP = false;
  }
  
  // 間隔一段時間顯示時間 hh,mm,ss
  if(millis()>timer_next_clock) {
    timer_next_clock = millis() + timer_delay_clock;
    
    // Draw Digital Clock
    show_clock();
  }

  // Do something
}
