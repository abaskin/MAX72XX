#include "Arduino.h"
#include "MAX72XX.hpp"

constexpr uint8_t decimalPlaces = 2;

MAX72XX max7219(7, 9, 8, 1);
MAX72XXDisplay display0(&max7219, 0, 4);
MAX72XXDisplay display1(&max7219, 4, 4);

void setup() {
  max7219.setIntensity(10);
}

void loop() {
  for (long n = -10; n < 10 ; n += 10) {
    display0.writeNumber(n, MAX72XX::Character::BLANK, decimalPlaces);
    display1.writeNumber(n, MAX72XX::Character::BLANK, decimalPlaces);
    delay(500);
  }
}
