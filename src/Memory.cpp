#include "Memory.h"
#include <EEPROM.h>
#include "Light.h"
#include "Logger.h"

Memory::Memory()
{ }

void Memory::dump (unsigned int start, unsigned int limit)
{
	byte value;

	if (limit > EEPROM.length())
		limit = EEPROM.length();

	Log.trace ("EEPROM dump from %X to %X" dendl, start, limit);

	for (unsigned int index = start; index <= limit; index++)
	{
		value = EEPROM.read (index);

		Log.trace ("0x%s%x: ", index <= 0xF ? "000" : index <= 0xFF ? "00" : index <= 0xFFF ? "0" : "", index);

		Log.tracenp ("0b%s%b", value <= 0x1 ? "0000000" : value <= 0x3 ? "000000" : value <= 0x7 ? "00000" : value <= 0xF ? "0000" : value <= 0x1F ? "000" : value <= 0x3F ? "00" : value <= 0x7F ? "0" : "" endl, value);

		Log.trace (" (0x%s%x)", value <= 0xF ? "000" : value <= 0xFF ? "00" : value <= 0xFFF ? "0" : "", value);
	}

	Log.trace (endl);
}

void Memory::write ()
{
	unsigned int address, n;

	address = EEPROM_START;
	n       = 0;

	Log.trace ("Writing EEPROM... ");

	if (EEPROM.read (address) != (byte) EEPROM_TEST_BYTE)
	{
		EEPROM.write (address, (byte) EEPROM_TEST_BYTE);
		n++;
	}

	address++;

	for (unsigned int i = LIGHT_MOD_MIN; i < LIGHT_N_MOD; i++)
	{
		for (unsigned int j = 0; j < sizeof(long); j++)
			if (EEPROM.read (address + j) != (byte) (light.getRgb (i) >> (j * 8)))
			{
				EEPROM.write (address + j, (byte) (light.getRgb (i) >> (j * 8)));
				n++;
			}

		address += sizeof(long);
	}

	for (unsigned int j = LIGHT_MOD_MIN; j < LIGHT_N_MOD; j++)
	{
		if (EEPROM.read (address) != (byte) (light.getPower (j)))
		{
			EEPROM.write (address, (byte) (light.getPower (j)));
			n++;
		}

		address += sizeof(char);
	}

	for (unsigned int j = LIGHT_MOD_MIN; j < LIGHT_N_MOD; j++)
	{
		for (unsigned int i = 0; i < sizeof(int); i++)
			if (EEPROM.read (address + i) != (byte) (light.getSpeed (j) >> (i * 8)))
			{
				EEPROM.write (address + i, (byte) (light.getSpeed (j) >> (i * 8)));
				n++;
			}

		address += sizeof(int);
	}

	Log.tracenp ("Done ! (%d byte%s written)" dendl, n, n > 1 ? "s" : "");
} // Memory::write

boolean Memory::read ()
{
	unsigned int address;

	address = EEPROM_START;

	Log.trace ("Reading EEPROM... ");

	if (EEPROM.read (address) != (byte) EEPROM_TEST_BYTE)
	{
		Log.tracenp (dendl);
		return true; // Returns true to say that variables needs to be initialized
	}

	address++;

	for (unsigned int i = LIGHT_MOD_MIN; i < LIGHT_N_MOD; i++)
	{
		unsigned long rgb;

		rgb = 0;
		for (unsigned int j = 0; j < sizeof(long); j++)
			rgb += ((long) EEPROM.read (address + j)) << (j * 8);

		light.setRgb (rgb, i);

		address += sizeof(long);
	}

	for (unsigned int i = LIGHT_MOD_MIN; i < LIGHT_N_MOD; i++)
	{
		light.setPower (EEPROM.read (address), i);

		address += sizeof(char);
	}

	for (unsigned int i = LIGHT_MOD_MIN; i < LIGHT_N_MOD; i++)
	{
		unsigned int speed;

		speed = 0;
		for (unsigned int j = 0; j < sizeof(int); j++)
			speed += ((int) EEPROM.read (address + j)) << (j * 8);

		light.setSpeed (speed, i);

		address += sizeof(int);
	}

	Log.tracenp ("Done ! (%d byte%s read)" dendl, address, address > 1 ? "s" : "");

	return false;
} // writeData

Memory memory = Memory();
