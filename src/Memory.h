#ifndef MEMORY_H
#define MEMORY_H

#if defined(LUMOS_ARDUINO_MEGA)

# include <Arduino.h>

const uint16_t EEPROM_START  = 0x0000;
const uint16_t EEPROM_OFFSET = 0x0200;

const uint16_t EEPROM_LIGHT_START = EEPROM_START;                       // EEPROM start address
const uint16_t EEPROM_SOUND_START = EEPROM_LIGHT_START + EEPROM_OFFSET; // EEPROM start address
const uint16_t EEPROM_ALARM_START = EEPROM_SOUND_START + EEPROM_OFFSET; // EEPROM start address
const uint8_t EEPROM_TEST_BYTE    = 0xB6;                               // Byte I use to know if EEPROM has been initialized or not

class Memory
{
public:
	Memory();
	void dump (uint16_t start, uint16_t limit);

	void writeForAll ();
	bool readForAll ();
	void writeForLight ();
	bool readForLight ();
	void writeForSound ();
	bool readForSound ();
	void writeForAlarms ();
	bool readForAlarms ();
};

extern Memory memory;

#endif // if defined(LUMOS_ARDUINO_MEGA)

#endif // ifndef MEMORY_H