// A simple NTP based clock for ESP8266
//
// This uses two displays to show the time and data.
//

#include "Arduino.h"
#include "MAX72XX.h"

#include <sys/time.h>   // struct timeval
#include <time.h>       // time() ctime()
#include <ESP8266WiFiMulti.h>
#include <Ticker.h>

constexpr auto STASSID = "<<WIFI SSID>";
constexpr auto STAPSK = "<<WIFI SECRET>>";

#define TZ PSTR("ICT-7") // change to your timezone

constexpr uint8_t numDevices = 2;
constexpr uint8_t latchPin = D1; // Change to reflect your latchPin

MAX72XX dispChain(latchPin, numDevices);

MAX72XXDisplay  hourDisp(&dispChain, 14, 2);
MAX72XXDisplay  minDisp(&dispChain, 12, 2);
MAX72XXDisplay  secDisp(&dispChain, 10, 2);
MAX72XXDisplay  meridDisp(&dispChain, 8, 2);

MAX72XXDisplay  dayDisp(&dispChain, 6, 2);
MAX72XXDisplay  monDisp(&dispChain, 4, 2);
MAX72XXDisplay  yearDisp(&dispChain, 0, 4);

ESP8266WiFiMulti wifiMulti;

Ticker updateTicker;

void setup() {
  configTime(TZ, "pool.ntp.org");

  wifiMulti.addAP(STASSID,STAPSK);

  WiFi.mode(WIFI_STA);

  dispChain.setIntensity(6);

  updateTicker.attach_ms(500, []() {
    auto now = time(nullptr);
    const tm* tm = localtime(&now);

    static int currHour = -1;
    if (currHour != tm->tm_hour) {
      currHour = tm->tm_hour;
      int32_t hour = ((tm->tm_hour % 12) != 0) ? tm->tm_hour % 12 : 12;
      hourDisp.writeNumber(hour, MAX72XX::Character::BLANK, 0);

      meridDisp.writeChar(1, (tm->tm_hour < 12) ? 0b01110111 : 0b01100111);
      meridDisp.update();
    }

    static int currMin = -1;
    if (currMin != tm->tm_min) {
      currMin != tm->tm_min;
      minDisp.writeNumber(tm->tm_min, MAX72XX::Character::ZERO, 0);
    }

    secDisp.writeNumber(tm->tm_sec, MAX72XX::Character::ZERO);

    static int currDay = -1;
    static int currYear = -1;
    if (currDay != tm->tm_mday || currYear != tm->tm_year) {
      currDay = tm->tm_mday;
      currYear = tm->tm_year;
      dayDisp.writeNumber(tm->tm_mday, MAX72XX::Character::ZERO, 0);
      monDisp.writeNumber(tm->tm_mon + 1, MAX72XX::Character::ZERO, 0);
      yearDisp.writeNumber(1900 + tm->tm_year, MAX72XX::Character::BLANK);
    }
  });
}

void loop() {
  wifiMulti.run();
}
