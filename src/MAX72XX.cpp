//library for using MAX7129 to drive 7-segment numerical displays

#include "Arduino.h"
#include "MAX72XX.hpp"

// MAX72XX

MAX72XX::MAX72XX(const uint8_t pin_data, const uint8_t pin_clock,
								 const uint8_t pin_latch, const uint8_t num_devices) :
	_pin_data(pin_data), _pin_clock(pin_clock), _pin_latch(pin_latch),
  _num_devices(num_devices), decodeReg(new uint8_t[_num_devices]()) {
	pinMode(_pin_data,OUTPUT);
	pinMode(_pin_clock,OUTPUT);
	pinMode(_pin_latch,OUTPUT);

	writeRegister(Device::ALL, Address::DISPLAYTEST, 0x00);
	writeRegister(Device::ALL, Address::SCANLIMIT, digitsPerDevice - 1);
	setIntensity(0x0F / 2);
	shutdown(false);
}

MAX72XX::Error MAX72XX::writeRegister(const uint8_t deviceNum,
																			const uint8_t address,
														 					const uint8_t data) const
{
	if (deviceNum >= _num_devices && deviceNum != (uint8_t)Device::ALL)
		return Error::INVALID_DEVICE;
	digitalWrite(_pin_latch, LOW);
		for (uint8_t d = 0; d < _num_devices; d++) {
			uint8_t addr = (d == deviceNum || deviceNum == (uint8_t)Device::ALL)
				? address : (uint8_t)Address::NOOP;
			writeToDevice(addr, data);
		}
	digitalWrite(_pin_latch, HIGH);
	return Error::OK;
}

MAX72XX::Error MAX72XX::writeRegister(const Address address, const uint8_t* data) const
{
	digitalWrite(_pin_latch, LOW);
		for (uint8_t d = 0; d < _num_devices; d++)
			writeToDevice((uint8_t)address, data[d]);
	digitalWrite(_pin_latch, HIGH);
	return Error::OK;
}

MAX72XX::Error MAX72XX::writeRegister(const Device deviceNum,
																			const Address address,
																			const Character data) const
{
	return writeRegister((uint8_t)deviceNum, (uint8_t)address, (uint8_t)data);
}

MAX72XX::Error MAX72XX::writeRegister(const Device deviceNum,
																			const Address address,
																			const uint8_t data) const
{
	return writeRegister((uint8_t)deviceNum, (uint8_t)address, data);
}

MAX72XX::Error MAX72XX::writeRegister(const uint8_t deviceNum,
																		 	const Address address,
																			const uint8_t data) const
{
	return writeRegister(deviceNum, (uint8_t)address, data);
}

void MAX72XX::writeToDevice(const uint8_t addr, const uint8_t data) const
{
	shiftOut(_pin_data, _pin_clock, MSBFIRST, addr);
	shiftOut(_pin_data, _pin_clock, MSBFIRST, data);
}

MAX72XX::Error MAX72XX::setIntensity(const uint8_t deviceNum,
																		 const uint8_t brightness) const
{
	return writeRegister(deviceNum, Address::INTENSITY, brightness);
}

MAX72XX::Error MAX72XX::setIntensity(const uint8_t brightness) const
{
	return writeRegister(Device::ALL, Address::INTENSITY, brightness);
}

MAX72XX::Error MAX72XX::shutdown(const uint8_t deviceNum, const bool status) const {
	return writeRegister(deviceNum, Address::SHUTDOWN, (status) ? 0x00 : 0x01);
}

MAX72XX::Error MAX72XX::shutdown(const bool status) const {
	return writeRegister(Device::ALL, Address::SHUTDOWN, (status) ? 0x00 : 0x01);
}

MAX72XX::Error MAX72XX::setDecodeReg(const uint8_t digit, const bool decode) {
	if (digit >= (_num_devices * digitsPerDevice)) return Error::INVALID_DIGIT;
	if (decode) decodeReg[digit / digitsPerDevice] |= 0x01 << (digit % digitsPerDevice);
	else decodeReg[digit / digitsPerDevice] &= ~(0x01 << (digit % digitsPerDevice));
	return Error::OK;
}

MAX72XX::Error MAX72XX::update(const uint8_t start, const uint8_t numDigits,
										 					 const uint16_t* vram) {
	for(uint8_t digit = 0; digit < numDigits; digit++) {
		Error err = setDecodeReg(start + digit, (vram[digit] & 0x10) == 0x10);
		if (err != Error::OK) return err;
	}
	writeRegister(Address::DECODE, decodeReg);

	shutdown(true);
		for(uint8_t digit = 0; digit < numDigits; digit++) {
			uint8_t device = (digit + start) / digitsPerDevice;
			uint8_t offset = (digit + start) % digitsPerDevice;
			writeRegister(device, (uint8_t)Address::DIGIT0 + offset, vram[digit]);
		}
	shutdown(false);

	return Error::OK;
}

// MAX72XXDisplay

MAX72XXDisplay::MAX72XXDisplay(MAX72XX* max72xx, const uint8_t start,
															 const uint8_t num_digits) :
  _max72xx(max72xx), _start(start), _num_digits(num_digits),
	vram(new uint16_t[_num_digits]()) {
	update();
}

MAX72XX::Error MAX72XXDisplay::update() const
{
	return _max72xx->update(_start, _num_digits, vram);
}

void MAX72XXDisplay::fill(const uint8_t data, const bool decode)
{
	for(uint8_t d = 0; d < _num_digits; d++) writeChar(d, data, decode);
}

void MAX72XXDisplay::fill(const MAX72XX::Character data)
{
	fill((uint8_t)data, true);
}

MAX72XX::Error MAX72XXDisplay::writeChar(const uint8_t digit, const uint8_t data,
																				 const bool decode)
{
	if (digit >= _num_digits) return MAX72XX::Error::INVALID_DIGIT;
	vram[digit] = (decode) ? data | 0x10 : data & 0x0F;
	return MAX72XX::Error::OK;
}

MAX72XX::Error MAX72XXDisplay::writeChar(const uint8_t digit,
																				 const MAX72XX::Character data)
{
	return writeChar(digit, (uint8_t)data, true);
}

MAX72XX::Error MAX72XXDisplay::setDecimalPoint(const uint8_t digit)
{
	if (digit >= _num_digits) return MAX72XX::Error::INVALID_DIGIT;
	vram[digit] |= (uint8_t)MAX72XX::Character::DECIMALPOINT;
	return MAX72XX::Error::OK;
}

uint16_t MAX72XXDisplay::readChar(const uint8_t digit) const
{
	return (digit < _num_digits) ? vram[digit] : 0x20;
}

MAX72XX::Error MAX72XXDisplay::doWriteNumber(const long num,
																						 const uint8_t padding_char,
									 								 					 const bool decodePadding,
																	 				 	 const uint8_t decimalPlaces,
									 							 	 		 			 const bool autoUpdate)
{
	fill(padding_char, decodePadding);

	uint8_t digit;
	unsigned long number = abs(num);
	for (digit = 0; digit < _num_digits && number != 0; digit++, number /= 10) {
		MAX72XX::Error err = writeChar(digit, number % 10, true);
		if (err != MAX72XX::Error::OK) return(err);
	}

	while (digit <= decimalPlaces && digit < _num_digits) {
		MAX72XX::Error err = writeChar(digit++, 0, true);
		if (err != MAX72XX::Error::OK) return(err);
	}

	if (num < 0) {
		MAX72XX::Error err = writeChar(digit, MAX72XX::Character::DASH);
		if (err != MAX72XX::Error::OK) return(err);
	}

	if (decimalPlaces > 0) {
		MAX72XX::Error err = setDecimalPoint(decimalPlaces);
		if (err != MAX72XX::Error::OK) return(err);
	}

  if (autoUpdate) return update();
	return MAX72XX::Error::OK;
}

MAX72XX::Error MAX72XXDisplay::writeNumber(const long num,
																					 const uint8_t padding_char,
																 					 const uint8_t decimalPlaces,
																					 const bool autoUpdate)
{
	return doWriteNumber(num, padding_char, false, decimalPlaces, autoUpdate);
}

MAX72XX::Error MAX72XXDisplay::writeNumber(const long num,
																					 const MAX72XX::Character padding_char,
																 					 const uint8_t decimalPlaces,
																					 const bool autoUpdate)
{
	return doWriteNumber(num, (uint8_t)padding_char, true, decimalPlaces, autoUpdate);
}

MAX72XX::Error MAX72XXDisplay::writeNumber(const float num,
																					 const uint8_t padding_char,
												 				 					 const uint8_t decimalPlaces,
																					 const bool autoUpdate)
{
	return doWriteNumber(long(num * pow(10.0, decimalPlaces)), padding_char, false,
						  				 decimalPlaces,	autoUpdate);
}

MAX72XX::Error MAX72XXDisplay::writeNumber(const float num,
																					 const MAX72XX::Character padding_char,
												 				 					 const uint8_t decimalPlaces,
																					 const bool autoUpdate)
{
	return doWriteNumber(long(num * pow(10.0, decimalPlaces)),
											 (uint8_t)padding_char, true,
											 decimalPlaces, autoUpdate);
}

MAX72XX::Error MAX72XXDisplay::shutdown(const bool status) const {
	if (status) {
		uint16_t blanks[_num_digits] = {0};
		MAX72XX::Error err = _max72xx->update(_start, _num_digits, blanks);
		if (err != MAX72XX::Error::OK) return(err);
	}
	else return update();
}
