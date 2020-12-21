# MAX72XX 7-segment display library

This is a library for driving a 7-segment display driven with a MAX7219 or MAX7221 IC. It is originally based on the MAX7219 library by Frost Sheridan (https://github.com/Frostion/MAX7219-arduino-library).

## Using the library

There are two objects which must be created to use the library. The MAX72XX object
defines the interface to the MAX7219/MAX7221 driver chip. Only one MAX72XX object is
required for each chain of devices. The MAX72XXDisplay object defines range of
digits. There can be multiple MAX72XXDisplay objects based on how you want to
divide up the displays. The order of the devices and digits is dependent on the
construction of the devices, but usually device 0 is at the end of the chain
and digit 0 is on the righthand side of the display.

### Declaring the MAX72XX object

`MAX7219 object_name(const uint8_t pin_data, const uint8_t pin_clock, const uint8_t pin_latch, const uint8_t num_devices);`

 - `object_name` the name of the object
 - `data_pin` the pin that is connected to the MAX72XX data/DIN pin
 - `clock_pin` the pin that is connected to the MAX72XX clock pin
 - `latch_pin` the pin that is connected to the MAX7XX latch/CS pin
 - `num_devices` the number of devices chained together

### Declaring the MAX72XX display object

`MAX72XXDisplay object_name(MAX72XX* max7219, const uint8_t start, const uint8_t num_digits);`

- `object_name` the name of the object
- `max7219` the MAX72XX object the display
- `start` the first digit of the display
- `num_digits` the number of digits in the display

### Characters

Characters maybe written using either the MAX72XX::Character constants
(MAX72XX Code B Font) or as uint8_t values defining the segments to be lit. As
the Code B Fonts for 0 - 9 are BCD, these can be passed and decoded if the decoded
function parameter is passed as true.

In the display buffer characters a stored as uint16_t with the lower 8 bits
containing the character value. If bit 8 (0x10) is set the character value is
decoded as a Code B Font character, otherwise the value is used to determine
the segments to be lit.

### Functions available

#### MAX72XXDisplay

`MAX72XX::Error update()`\
Update the display. Note that only the digits of the given display are updated.

`void fill(const uint8_t data, const bool decode = false)`\
`void fill(const MAX72XX::Character data)`\
Fill the display with the given character

`MAX72XX::Error writeChar(const uint8_t digit, const uint8_t data,
                          const bool decode = false)`\
`MAX72XX::Error writeChar(const uint8_t digit,
                          const MAX72XX::Character data)`\
Write a single character at the given digit position.

`MAX72XX::Error setDecimalPoint(const uint8_t digit)`\
Set the decimal point at given digit position.

`uint16_t readChar(const uint8_t digit)`\
Read the character at the given digit position. If the digit position passed is
invalid the returned value has the 0x20 bit set.

`MAX72XX::Error writeNumber(const long num, const uint8_t padding_char,
                            const uint8_t decimalPlaces = 0,
                            const bool autoUpdate = true)`\
`MAX72XX::Error writeNumber(const long num,
                           const MAX72XX::Character padding_char,
                           const uint8_t decimalPlaces = 0,
                           const bool autoUpdate = true)`\
`MAX72XX::Error writeNumber(const float num, const uint8_t padding_char,
                           const uint8_t decimalPlaces,
                           const bool autoUpdate = true)`\
`MAX72XX::Error writeNumber(const float num,
                           const MAX72XX::Character padding_char,
                           const uint8_t decimalPlaces,
                           const bool autoUpdate = true)`\
Write a number to the display. Both long and float numbers are supported. If
autoUpdate is true the display is updated before return.

`MAX72XX::Error shutdown(const bool status)`\
Shutdown the display. Only the given display is shutdown.

#### MAX72XX

`MAX72XX::Error writeRegister(const uint8_t deviceNum, const uint8_t address,
                   const uint8_t value)`\
`MAX72XX::Error writeRegister(const Device deviceNum, const Address address,
                   const Character value)`\
`MAX72XX::Error writeRegister(const Device deviceNum, const Address address,
                   const uint8_t value)`\
`MAX72XX::Error writeRegister(const uint8_t deviceNum, const Address address,
                   const uint8_t value)`\
Write an address and data value to the given device.

`MAX72XX::Error writeRegister(const Address address, const uint8_t* data)`\
Write an address and data value to all devices. The data value must be large
enough for all devices and in the correct order.

`MAX72XX::Error setIntensity(const uint8_t deviceNum, const uint8_t brightness)`\
`MAX72XX::Error setIntensity(const uint8_t brightness)`\
Set the intensity for the given device or all devices.

`MAX72XX::Error shutdown(const uint8_t deviceNum, const bool status)`\
`MAX72XX::Error shutdown(const bool status)`\
Shutdown the given device or all devices.

`MAX72XX::Error update(const uint8_t start, const uint8_t numDigits,
             const uint16_t* vram)`\
Update a range of digits. See the Character section for the format of vram.

### Constants available

##### Devices

- `MAX72XX::Device::ALL` - All devices

##### Segment

- `MAX72XX::Segment::A` - A Segment
- `MAX72XX::Segment::B` - B Segment
- `MAX72XX::Segment::C` - C Segment
- `MAX72XX::Segment::D` - D Segment
- `MAX72XX::Segment::E` - E Segment
- `MAX72XX::Segment::F` - F Segment
- `MAX72XX::Segment::G` - G Segment
- `MAX72XX::Segment::DP` - Decimal Point

##### Characters (MAX72XX Code B Font)

- `MAX72XX::Character::ZERO` - 0
- `MAX72XX::Character::ONE` - 1
- `MAX72XX::Character::TWO` - 2
- `MAX72XX::Character::THREE` - 3
- `MAX72XX::Character::FOUR` - 4
- `MAX72XX::Character::FIVE` - 5
- `MAX72XX::Character::SIX` - 6
- `MAX72XX::Character::SEVEN` - 7
- `MAX72XX::Character::EIGHT` - 8
- `MAX72XX::Character::NINE` - 9
- `MAX72XX::Character::DASH` - Dash
- `MAX72XX::Character::E` - E
- `MAX72XX::Character::H` - H
- `MAX72XX::Character::L` - L
- `MAX72XX::Character::P` - P
- `MAX72XX::Character::BLANK` - Blank
- `MAX72XX::Character::DECIMALPOINT` - Decimal Point

##### Address

- `MAX72XX::Address::NOOP` - No-Op
- `MAX72XX::Address::DIGIT0` - Digit 0
- `MAX72XX::Address::DIGIT1` - Digit 1
- `MAX72XX::Address::DIGIT2` - Digit 2
- `MAX72XX::Address::DIGIT3` - Digit 3
- `MAX72XX::Address::DIGIT4` - Digit 4
- `MAX72XX::Address::DIGIT5` - Digit 5
- `MAX72XX::Address::DIGIT6` - Digit 6
- `MAX72XX::Address::DIGIT7` - Digit 7
- `MAX72XX::Address::DECODE` - Decode Mode
- `MAX72XX::Address::INTENSITY` - Intensity
- `MAX72XX::Address::SCANLIMIT` - Scan Limit
- `MAX72XX::Address::SHUTDOWN` - Shutdown
- `MAX72XX::Address::DISPLAYTEST` - Display Test

##### Error

- `MAX72XX::ERROR::OK` - No Error
- `MAX72XX::ERROR::INVALID_DIGIT` - Invalid Digit
- `MAX72XX::ERROR::INVALID_DEVICE` - Invalid Device

### How the segments map to bits
Each bit in that byte controls
one of the digit's 8 segments:
![Chart of segments](https://i.imgur.com/IkFAebV.png)
