/*
 * Lịch vạn niên
 */
#include <Arduino.h>
#include "font.h"
#include "LichVanNien.h"
#include <ezTime.h>
#include <ESP8266WiFi.h>
#include "SSD1306Spi.h"
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);


const char ssid[] = "Bao Tien";     //  your network SSID (name)
const char pass[] = "123456123456"; // your network password

// Initialize the OLED display using SPI
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS

SSD1306Spi display(D0, D4, D8);
int screenW = 128;
int screenH = 64;
int clockCenterX = screenW / 2;
int clockCenterY = ((screenH - 16) / 2) + 16; // top yellow part is 16 px height
int currentDay;
float degC = 0;
String lunarDate;

Timezone myTZ;

void digitalClockFrame()
{
  String timeNow = myTZ.dateTime("H:i");
  if (myTZ.dateTime("s").toInt() % 2 == 0)
  {
    timeNow = myTZ.dateTime("H i");
  }
  timeNow = timeNow + " (" + String(degC, 0) + "°C)";
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
  display.setFont(Arimo_Regular_20);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(clockCenterX, 20, timeNow);
  // Display lunar date
  display.setFont(ArialMT_Plain_16);
  display.drawString(clockCenterX, 48, lunarDate);
  display.display();
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // Needed for Leonardo only
  delay(250);
  Wire.begin(D2, D1); // due to limited pins, use pin 0 and 2 for SDA, SCL
  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  
  Serial.println();

  if (!Rtc.IsDateTimeValid())
  {
    if (Rtc.LastError() != 0)
    {
      // we have a communications error
      // see https://www.arduino.cc/en/Reference/WireEndTransmission for
      // what the number means
      Serial.print("RTC communications error = ");
      Serial.println(Rtc.LastError());
    }
    else
    {
      // Common Cuases:
      //    1) first time you ran and the device wasn't running yet
      //    2) the battery on the device is low or even missing

      Serial.println("RTC lost confidence in the DateTime!");

      // following line sets the RTC to the date & time this sketch was compiled
      // it will also reset the valid flag internally unless the Rtc device is
      // having an issue

      Rtc.SetDateTime(compiled);
    }
  }

  if (!Rtc.GetIsRunning())
  {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled)
  {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  }
  else if (now > compiled)
  {
    Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled)
  {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }

  // never assume the Rtc was last configured by you, so
  // just clear them to your needed state
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);


  Serial.print("Connecting to ");
  Serial.println(ssid);
  // setup display
  display.init();
  display.flipScreenVertically();
  display.clear();
  
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(clockCenterX, 0, "LICH VAN NIEN");

  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 16, "Wifi: " + String(ssid));
  display.display();
  // Connect to wifi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  display.drawString(0, 35, "Cap nhat thong tin");
  display.display();
  Serial.println("Get server time");
  // Sync time from server
  waitForSync();
  delay(2000);
  myTZ.setLocation("Asia/Bangkok");

}

void loop()
{
  RtcTemperature temp = Rtc.GetTemperature();
  degC = temp.AsFloatDegC();

  digitalClockFrame();

  //
  delay(1000);
}
