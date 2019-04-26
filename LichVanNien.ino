/*
 * Lịch vạn niên
 */
#include <Arduino.h>
#include "LichVanNien.h"
#include <ezTime.h>
#include <ESP8266WiFi.h>
#include "SSD1306Spi.h"

const char ssid[] = "Bao Tien";     //  your network SSID (name)
const char pass[] = "123456123456"; // your network password

SSD1306Spi display(D0, D2, D7);
int screenW = 128;
int screenH = 64;
int clockCenterX = screenW / 2;
int clockCenterY = ((screenH - 16) / 2) + 16; // top yellow part is 16 px height
int currentDay;
String lunarDate;

Timezone myTZ;

void digitalClockFrame()
{

  String timeNow = myTZ.dateTime("H:i:s");
  String dateNow = myTZ.dateTime("D d-m-Y");
  if (myTZ.dateTime("d").toInt() != currentDay)
  {
    currentDay = myTZ.dateTime("d").toInt();
    lunarDate = "AL: " + convertSolar2Lunar(currentDay, myTZ.dateTime("m").toInt(), myTZ.dateTime("Y").toInt());
  }

  display.clear();
  // Display date
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(clockCenterX, 0, dateNow);

  // Display time
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(clockCenterX, clockCenterY - 24, timeNow);
  // Display lunar date
  display.setFont(ArialMT_Plain_16);
  display.drawString(clockCenterX, clockCenterY + 4, lunarDate);
  display.display();
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // Needed for Leonardo only
  delay(250);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  // setup display
  display.init();
  display.flipScreenVertically();
  display.clear();
  
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(clockCenterX, 0, "LICH VAN NIEN");

  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 16, "Connecting to " + String(ssid));
  display.display();
  // Connect to wifi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  display.drawString(0, 35, "Get server time");
  display.display();
  Serial.println("Get server time");
  // Sync time from server
  waitForSync();
  delay(2000);
  myTZ.setLocation("Asia/Bangkok");

}

void loop()
{
  digitalClockFrame();
  delay(1000);
}
