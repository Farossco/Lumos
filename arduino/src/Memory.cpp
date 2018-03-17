#include "Memory.h"
#include <EEPROM.h>
#include "Global.h"
#include "Logger.h"
#include "Mods.h"

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

		Log.trace ("0x%s%b", value <= 0x1 ? "0000000" : value <= 0x3 ? "000000" : value <= 0x7 ? "00000" : value <= 0xF ? "0000" : value <= 0x1F ? "000" : value <= 0x3F ? "00" : value <= 0x7F ? "0" : "" endl, value);
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

	for (unsigned int i = 0; i < sizeof(long); i++)
		if (EEPROM.read (address + i) != (byte) (global.rgb[MOD_DEFAULT] >> (i * 8)))
		{
			EEPROM.write (address + i, (byte) (global.rgb[MOD_DEFAULT] >> (i * 8)));
			n++;
		}

	address += sizeof(long);

	for (unsigned int j = MOD_MIN; j < N_MOD; j++)
	{
		if (j != MOD_DAWN)
		{
			for (unsigned int i = 0; i < sizeof(int); i++)
				if (EEPROM.read (address + i) != (byte) ((int) global.power[j] >> (i * 8)))
				{
					EEPROM.write (address + i, (byte) ((int) global.power[j] >> (i * 8)));
					n++;
				}
		}
		else
		{
			for (unsigned int i = 0; i < sizeof(int); i++)
				if (EEPROM.read (address + i) != (byte) 0)
				{
					EEPROM.write (address + i, (byte) 0);
					n++;
				}
		}
		address += sizeof(int);
	}

	for (unsigned int j = MOD_MIN; j < N_MOD; j++)
	{
		for (unsigned int i = 0; i < sizeof(int); i++)
			if (EEPROM.read (address + i) != (byte) ((int) global.speed[j] >> (i * 8)))
			{
				EEPROM.write (address + i, (byte) (global.speed[j] >> (i * 8)));
				n++;
			}

		address += sizeof(int);
	}

	for (unsigned int i = 0; i < sizeof(int); i++)
		if (EEPROM.read (address + i) != (byte) (global.mod >> (i * 8)))
		{
			EEPROM.write (address + i, (byte) (global.mod >> (i * 8)));
			n++;
		}

	address += sizeof(int);

	if (EEPROM.read (address) != (byte) global.on ? 1 : 0)
	{
		EEPROM.write (address, (byte) global.on ? 1 : 0);
		n++;
	}

	address += sizeof(boolean);

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

	global.rgb[MOD_DEFAULT] = 0;
	for (unsigned int i = 0; i < sizeof(long); i++)
		global.rgb[MOD_DEFAULT] += ((long) EEPROM.read (address + i)) << (i * 8);

	address += sizeof(long);

	for (unsigned int j = MOD_MIN; j < N_MOD; j++)
	{
		global.power[j] = 0;
		for (unsigned int i = 0; i < sizeof(int); i++)
			global.power[j] += ((int) EEPROM.read (address + i)) << (i * 8);

		address += sizeof(int);
	}


	for (unsigned int j = MOD_MIN; j < N_MOD; j++)
	{
		global.speed[j] = 0;
		for (unsigned int i = 0; i < sizeof(int); i++)
			global.speed[j] += ((int) EEPROM.read (address + i)) << (i * 8);

		address += sizeof(int);
	}

	global.mod = 0;
	for (unsigned int i = 0; i < sizeof(int); i++)
		global.mod += ((int) EEPROM.read (address + i)) << (i * 8);

	address += sizeof(int);

	global.on = (EEPROM.read (address) != 0);

	address += sizeof(boolean);

	Log.tracenp ("Done ! (%d byte%s read)" dendl, address, address > 1 ? "s" : "");

	return false;
} // writeData

Memory memory = Memory();
