#ifndef MEMORY_H
#define MEMORY_H

#include <Arduino.h>

const int EEPROM_START      = 0x0000; // EEPROM start address
const byte EEPROM_TEST_BYTE = 0xAB;   // Byte I use to know if EEPROM has been initialized or not

class Memory
{
public:
	Memory();
	void dump (unsigned int start, unsigned int limit);
	void write ();
	boolean read ();
};

extern Memory memory;

#endif // ifndef MEMORY_H
