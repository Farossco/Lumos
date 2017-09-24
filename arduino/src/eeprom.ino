#include "arduino.h"

void eepromDump (unsigned int start, unsigned int limit)
{
	byte value;

	if (limit > EEPROM.length())
		limit = EEPROM.length();

	println (LEVEL_DEBUG, false);
	print (LEVEL_DEBUG, "EEPROM dump from ");
	print (LEVEL_DEBUG, start, HEX, false);
	print (LEVEL_DEBUG, " to ", false);
	println (LEVEL_DEBUG, limit, HEX, false);

	for (unsigned int index = start; index <= limit; index++)
	{
		value = EEPROM.read (index);

		print (LEVEL_DEBUG, index <= 0xF ? "000" : index <= 0xFF ? "00" : index <= 0xFFF ? "0" : "");
		print (LEVEL_DEBUG, index, HEX, false);
		print (LEVEL_DEBUG, ": ", false);

		print (LEVEL_DEBUG, value <= 0x1 ? "0000000" : value <= 0x3 ? "000000" : value <= 0x7 ? "00000" : value <= 0xF ? "0000" : value <= 0x1F ? "000" : value <= 0x3F ? "00" : value <= 0x7F ? "0" : "", false);
		println (LEVEL_DEBUG, value, BIN, false);
	}
} // eepromDump

void eepromWrite ()
{
	unsigned int address, n;

	address = EEPROM_START;
	n       = 0;

	println (LEVEL_DEBUG, false);
	println (LEVEL_DEBUG, "Writing EEPROM...");

	EEPROM.write (address, (byte) 42);

	address++;

	for (unsigned int i = 0; i < sizeof(long); i++)
		if (EEPROM.read (address + i) != (byte) (rgb[MODE_DEFAULT] >> (i * 8)))
		{
			EEPROM.write (address + i, (byte) (rgb[MODE_DEFAULT] >> (i * 8)));
			n++;
		}

	address += sizeof(long);

	for (unsigned int j = MODE_MIN; j < N_MODE; j++)
	{
		if (j != MODE_WAKEUP)
			for (unsigned int i = 0; i < sizeof(int); i++)
				if (EEPROM.read (address + i) != (byte) ((int) power[j] >> (i * 8)))
				{
					EEPROM.write (address + i, (byte) ((int) power[j] >> (i * 8)));
					n++;
				}

		address += sizeof(int);
	}


	for (unsigned int j = MODE_MIN; j < N_MODE; j++)
	{
		for (unsigned int i = 0; i < sizeof(int); i++)
			if (EEPROM.read (address + i) != (byte) ((int) speed[j] >> (i * 8)))
			{
				EEPROM.write (address + i, (byte) (speed[j] >> (i * 8)));
				n++;
			}

		address += sizeof(int);
	}

	for (unsigned int i = 0; i < sizeof(int); i++)
		if (EEPROM.read (address + i) != (byte) (mode >> (i * 8)))
		{
			EEPROM.write (address + i, (byte) (mode >> (i * 8)));
			n++;
		}

	address += sizeof(int);

	print (LEVEL_DEBUG, "Done ! ");
	print (LEVEL_DEBUG, n, DEC, false);
	print (LEVEL_DEBUG, " byte", false);
	if (n > 1)
		print (LEVEL_DEBUG, "s", false);
	println (LEVEL_DEBUG, " written", false);
} // writeData

boolean eepromRead ()
{
	unsigned int address;

	address = EEPROM_START;

	if (EEPROM.read (address) != (byte) EEPROM_TEST_BYTE)
		return true;

	address++;

	println (LEVEL_DEBUG, false);
	println (LEVEL_DEBUG, "Reading EEPROM...");

	rgb[MODE_DEFAULT] = 0;
	for (unsigned int i = 0; i < sizeof(long); i++)
		rgb[MODE_DEFAULT] += ((long) EEPROM.read (address + i)) << (i * 8);

	address += sizeof(long);

	for (unsigned int j = MODE_MIN; j < N_MODE; j++)
	{
		power[j] = 0;
		if (j != MODE_WAKEUP)
			for (unsigned int i = 0; i < sizeof(int); i++)
				power[j] += ((int) EEPROM.read (address + i)) << (i * 8);

		address += sizeof(int);
	}


	for (unsigned int j = MODE_MIN; j < N_MODE; j++)
	{
		speed[j] = 0;
		for (unsigned int i = 0; i < sizeof(int); i++)
			speed[j] += ((int) EEPROM.read (address + i)) << (i * 8);

		address += sizeof(int);
	}

	mode = 0;
	for (unsigned int i = 0; i < sizeof(int); i++)
		mode += ((int) EEPROM.read (address + i)) << (i * 8);

	address += sizeof(int);

	print (LEVEL_DEBUG, "Done ! ");
	print (LEVEL_DEBUG, address, DEC, false);
	print (LEVEL_DEBUG, " byte", false);
	if (address > 1)
		print (LEVEL_DEBUG, "s", false);
	println (LEVEL_DEBUG, " read", false);

	return false;
} // writeData
