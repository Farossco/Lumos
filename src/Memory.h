#if 0

#ifndef MEMORY_H
#define MEMORY_H

#include <Arduino.h>
#include <EEPROM.h>

const uint16_t EEPROM_START  = 0x0000;
const uint16_t EEPROM_OFFSET = 0x0200;

const uint16_t EEPROM_LIGHT_START = EEPROM_START;                       /* EEPROM start address */
const uint16_t EEPROM_SOUND_START = EEPROM_LIGHT_START + EEPROM_OFFSET; /* EEPROM start address */
const uint16_t EEPROM_ALARM_START = EEPROM_SOUND_START + EEPROM_OFFSET; /* EEPROM start address */
const uint8_t EEPROM_TEST_BYTE    = 0xB9;                               /* Byte used to know if EEPROM has been initialized or not */

class Memory {
public:
	Memory();
	void dump(uint16_t start, uint16_t limit);

	void writeAll();
	bool readAll();
	void writeLight();
	bool readLight();
	void writeSound();
	bool readSound();
	void writeAlarms();
	bool readAlarms();

private:
	uint16_t _address, _n;

	void startWrite(uint16_t address);
	bool startRead(uint16_t address);

	template <class T> void put(T value)
	{
		uint8_t *pValue = (uint8_t *)&value;

		for (int count = sizeof(T); count; --count) {
			if (EEPROM.read(_address) != *pValue) {
				EEPROM.write(_address, *pValue);
				_n++;
			}

			pValue++;
			_address++;
		}
	}

	template <class T> T get()
	{
		T value;

		uint8_t *pValue = (uint8_t *)&value;

		for (int count = sizeof(T); count; --count) {
			*pValue = EEPROM.read(_address);

			pValue++;
			_address++;
			_n++;
		}

		return value;
	}
};

extern Memory memory;

#endif // ifndef MEMORY_H

#endif /* if 0 */
