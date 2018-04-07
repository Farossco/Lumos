#ifndef MEMORY_H
#define MEMORY_H

#include <Arduino.h>

const int EEPROM_LIGHT_START = 0x0000; // EEPROM start address
const int EEPROM_SOUND_START = 0x4000; // EEPROM start address
const int EEPROM_ALARM_START = 0x8000; // EEPROM start address
const byte EEPROM_TEST_BYTE  = 0xB1;   // Byte I use to know if EEPROM has been initialized or not

class Memory
{
public:
	Memory();
	void dump (unsigned int start, unsigned int limit);

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

#endif // ifndef MEMORY_H
