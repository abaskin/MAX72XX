#ifndef MAX72XX_h
#define MAX72XX_h

//library for using MAX7129 to drive 7-segment numerical displays

#include "Arduino.h"

class MAX72XX
{
	public:
		enum class Device : uint8_t {
			ALL	= 0xFF,
		};

		enum class Segment : uint8_t {
			A 	= 0b01000000,
			B 	= 0b00100000,
			C 	= 0b00010000,
			D 	= 0b00001000,
			E 	= 0b00000100,
			F 	= 0b00000010,
			G 	= 0b00000001,
			DP	= 0b10000000,
		};

		enum class Address : uint8_t {
			NOOP 				= 0x00,
			DIGIT0 			= 0x01,
			DIGIT1 			= 0x02,
			DIGIT2 			= 0x03,
			DIGIT3 			= 0x04,
			DIGIT4 			= 0x05,
			DIGIT5 			= 0x06,
			DIGIT6 			= 0x07,
			DIGIT7 			= 0x08,
			DECODE 			= 0x09,
			INTENSITY		=	0x0A,
			SCANLIMIT  	= 0x0B,
			SHUTDOWN 		= 0x0C,
			DISPLAYTEST = 0x0F,
		};

		enum class Character : uint8_t {
			ZERO 					= 0x00,
			ONE 					= 0x01,
			TWO 					= 0x02,
			THREE 				= 0x03,
			FOUR 					= 0x04,
			FIVE 					= 0x05,
			SIX 					= 0x06,
			SEVEN 				= 0x07,
			EIGHT 				= 0x08,
			NINE 					= 0x09,
			DASH 					= 0x0A,
			E 						= 0x0B,
			H 						= 0x0C,
			L 						= 0x0D,
			P 						= 0x0E,
			BLANK 				= 0x0F,
			DECIMALPOINT	=	(uint8_t)Segment::DP,
		};

		enum class Error : uint8_t {
			OK,
			INVALID_DIGIT,
			INVALID_DEVICE,
		};

		MAX72XX(const uint8_t pin_data, const uint8_t pin_clock,
						const uint8_t pin_latch, const uint8_t num_devices = 1);

		Error writeRegister(const uint8_t deviceNum, const uint8_t address,
											 const uint8_t value) const;
	 	Error writeRegister(const Device deviceNum, const Address address,
											 const Character value) const;
	  Error writeRegister(const Device deviceNum, const Address address,
										   const uint8_t value) const;
	 	Error writeRegister(const uint8_t deviceNum, const Address address,
	 										 const uint8_t value) const;

		Error writeRegister(const Address address, const uint8_t* data) const;

		Error setIntensity(const uint8_t deviceNum, const uint8_t brightness) const;
		Error setIntensity(const uint8_t brightness) const;

		Error shutdown(const uint8_t deviceNum, const bool status) const;
		Error shutdown(const bool status) const;

		Error update(const uint8_t start, const uint8_t numDigits,
								 const uint16_t* vram);

	private:
		Error setDecodeReg(const uint8_t digit, const bool decode);
		void writeToDevice(const uint8_t addr, const uint8_t data) const;

		const uint8_t _pin_data, _pin_clock, _pin_latch, _num_devices;
		uint8_t* decodeReg;
		const uint8_t digitsPerDevice = 8;
};

class MAX72XXDisplay
{
	public:
		MAX72XXDisplay(MAX72XX* max7219, const uint8_t start,
									 const uint8_t num_digits);
		MAX72XX::Error update() const;
		void fill(const uint8_t data, const bool decode = false);
		void fill(const MAX72XX::Character data);

		MAX72XX::Error writeChar(const uint8_t digit, const uint8_t data,
														 const bool decode = false);
		MAX72XX::Error writeChar(const uint8_t digit,
														 const MAX72XX::Character data);

		uint16_t readChar(const uint8_t digit) const;

		MAX72XX::Error setDecimalPoint(const uint8_t digit);

		MAX72XX::Error writeNumber(const long num, const uint8_t padding_char,
									 	 					 const uint8_t decimalPlaces = 0,
															 const bool autoUpdate = true);
	  MAX72XX::Error writeNumber(const long num,
															 const MAX72XX::Character padding_char,
									 	 					 const uint8_t decimalPlaces = 0,
															 const bool autoUpdate = true);

		MAX72XX::Error writeNumber(const float num, const uint8_t padding_char,
										 					 const uint8_t decimalPlaces,
															 const bool autoUpdate = true);
		MAX72XX::Error writeNumber(const float num,
															 const MAX72XX::Character padding_char,
										 					 const uint8_t decimalPlaces,
															 const bool autoUpdate = true);

		MAX72XX::Error shutdown(const bool status) const;

	private:
		MAX72XX::Error doWriteNumber(const long num, const uint8_t padding_char,
											 					 const bool decodePadding,
																 const uint8_t decimalPlaces,
											 				 	 const bool autoUpdate);

		MAX72XX* _max7219;
		const uint8_t _start, _num_digits;
		uint16_t* vram;
};

#endif
