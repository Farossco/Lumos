#include "arduino.h"

void eepromDump (unsigned int start, unsigned int limit)
{
	byte value;

	if (limit > EEPROM.length())
		limit = EEPROM.length();

	printlnNoPrefix();
	print ("EEPROM dump from ");
	printNoPrefix (start, HEX);
	printNoPrefix (" to ");
	printlnNoPrefix (limit, HEX);

	for (unsigned int index = start; index <= limit; index++)
	{
		if (index <= 0xF)
			print ("000");
		else if (index <= 0xFF)
			print ("00");
		else if (index <= 0xFFF)
			print ("0");
		else
			print ("");

		printNoPrefix (index, HEX);

		printNoPrefix (": ");

		value = EEPROM.read (index);

		if (value <= 0x1)
			printNoPrefix ("0000000");
		else if (value <= 0x3)
			printNoPrefix ("000000");
		else if (value <= 0x7)
			printNoPrefix ("00000");
		else if (value <= 0xF)
			printNoPrefix ("0000");
		else if (value <= 0x1F)
			printNoPrefix ("000");
		else if (value <= 0x3F)
			printNoPrefix ("00");
		else if (value <= 0x7F)
			printNoPrefix ("0");

		printlnNoPrefix (value, BIN);
	}
} // eepromDump

void eepromWrite ()
{
	unsigned int address, n;

	address = EEPROM_START;
	n       = 0;

	printlnNoPrefix();
	println ("Writing EEPROM...");

	EEPROM.write (address, (byte) 42);

	address++;

	for (unsigned int i = 0; i < sizeof(long); i++)
		if (EEPROM.read (address + i) != (byte) (rgb[MODE_DEFAULT] >> (i * 8)))
		{
			EEPROM.write (address + i, (byte) (rgb[MODE_DEFAULT] >> (i * 8)));
			n++;
		}

	address += sizeof(long);

	for (unsigned int j = 0; j <= MODE_MAX; j++)
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


	for (unsigned int j = 0; j <= MODE_MAX; j++)
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

	print ("Done ! ");
	printNoPrefix (n, DEC);
	printNoPrefix (" byte");
	if (n > 1)
		printNoPrefix ("s");
	printlnNoPrefix (" written");
} // writeData

boolean eepromRead ()
{
	unsigned int address;

	address = EEPROM_START;

	if (EEPROM.read (address) != (byte) 42)
	{
		printlnNoPrefix();
		println ("First launch !");
		return true;
	}

	address++;

	printlnNoPrefix();
	println ("Reading EEPROM...");

	rgb[MODE_DEFAULT] = 0;
	for (unsigned int i = 0; i < sizeof(long); i++)
		rgb[MODE_DEFAULT] += ((long) EEPROM.read (address + i)) << (i * 8);

	address += sizeof(long);

	for (unsigned int j = 0; j <= MODE_MAX; j++)
	{
		power[j] = 0;
		if (j != MODE_WAKEUP)
			for (unsigned int i = 0; i < sizeof(int); i++)
				power[j] += ((int) EEPROM.read (address + i)) << (i * 8);

		address += sizeof(int);
	}


	for (unsigned int j = 0; j <= MODE_MAX; j++)
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

	print ("Done ! ");
	printNoPrefix (address, DEC);
	printNoPrefix (" byte");
	if (address > 1)
		printNoPrefix ("s");
	printlnNoPrefix (" read");

	return false;
} // writeData
