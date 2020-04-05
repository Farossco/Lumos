#if defined(LUMOS_ARDUINO_MEGA)

#include "Memory.h"
#include <EEPROM.h>
#include "Light.h"
#include "Logger.h"
#include "Alarms.h"

Memory::Memory()
{ }

void Memory::dump (uint16_t start, uint16_t limit)
{
	uint8_t value;

	if (limit > EEPROM.length())
		limit = EEPROM.length();

	trace << "EEPROM dump from " << hex << start << " to " << limit << dendl;

	for (uint16_t index = start; index <= limit; index++)
	{
		value = EEPROM.read (index);

		trace << setfill ('0') << setw (4) << hex << index << " : ";

		trace << setfill ('0') << setw (4) << bin << value;

		trace << setfill ('0') << setw (4) << hex << " (" << value << ")" << endl;
	}

	trace << endl;
}

void Memory::writeForAll ()
{
	writeForLight();
	writeForSound();
	writeForAlarms();
}

bool Memory::readForAll ()
{
	return readForLight() || readForSound() || readForAlarms();
}

void Memory::writeForLight ()
{
	uint16_t address, n;

	address = EEPROM_LIGHT_START;
	n       = 0;

	trace << "Writing EEPROM for light variables... ";

	if (EEPROM.read (address) != (byte) EEPROM_TEST_BYTE)
	{
		EEPROM.write (address, (byte) EEPROM_TEST_BYTE);
		n++;
	}

	address++;

	// All RGB values
	for (uint8_t i = LIGHT_MOD_MIN; i < LIGHT_MOD_N; i++)
	{
		for (unsigned int j = 0; j < sizeof(uint32_t); j++)
			if (EEPROM.read (address + j) != (uint8_t) (light.getRgb (i) >> (j * 8)))
			{
				EEPROM.write (address + j, (uint8_t) (light.getRgb (i) >> (j * 8)));
				n++;
			}

		address += sizeof(uint32_t);
	}

	// All Light values
	for (uint8_t j = LIGHT_MOD_MIN; j < LIGHT_MOD_N; j++)
	{
		if (EEPROM.read (address) != light.getPower (j))
		{
			EEPROM.write (address, light.getPower (j));
			n++;
		}

		address += sizeof(uint8_t);
	}

	// All speed values
	for (uint8_t j = LIGHT_MOD_MIN; j < LIGHT_MOD_N; j++)
	{
		for (unsigned int i = 0; i < sizeof(uint16_t); i++)
			if (EEPROM.read (address + i) != (uint8_t) (light.getSpeed (j) >> (i * 8)))
			{
				EEPROM.write (address + i, (uint8_t) (light.getSpeed (j) >> (i * 8)));
				n++;
			}

		address += sizeof(uint16_t);
	}

	trace << "Done ! (" << n << " byte" << ((n > 1) ? "s" : "") << " written)" << dendl;
} // Memory::writeForLight

bool Memory::readForLight ()
{
	uint16_t address;

	address = EEPROM_LIGHT_START;

	trace << "Reading EEPROM for light variables... ";

	if (EEPROM.read (address) != EEPROM_TEST_BYTE)
	{
		trace << dendl;
		return true; // Returns true to say that variables needs to be initialized
	}

	address++;

	for (uint8_t i = LIGHT_MOD_MIN; i < LIGHT_MOD_N; i++)
	{
		uint32_t rgb;

		rgb = 0;
		for (unsigned int j = 0; j < sizeof(uint32_t); j++)
			rgb += ((uint32_t) EEPROM.read (address + j)) << (j * 8);

		light.setRgb (rgb, i);

		address += sizeof(uint32_t);
	}

	for (uint8_t i = LIGHT_MOD_MIN; i < LIGHT_MOD_N; i++)
	{
		light.setPower (EEPROM.read (address), i);

		address += sizeof(uint8_t);
	}

	for (uint8_t i = LIGHT_MOD_MIN; i < LIGHT_MOD_N; i++)
	{
		uint16_t speed;

		speed = 0;
		for (unsigned int j = 0; j < sizeof(int); j++)
			speed += ((uint16_t) EEPROM.read (address + j)) << (j * 8);

		light.setSpeed (speed, i);

		address += sizeof(uint16_t);
	}

	trace << "Done ! (" << address - EEPROM_LIGHT_START << " byte" << ((address > 1) ? "s" : "") << " read)" << dendl;

	return false;
} // Memory::readForLight

void Memory::writeForSound ()
{
	uint16_t address, n;

	address = EEPROM_SOUND_START;
	n       = 0;

	trace << "Writing EEPROM for sound variables... ";

	if (EEPROM.read (address) != EEPROM_TEST_BYTE)
	{
		EEPROM.write (address, EEPROM_TEST_BYTE);
		n++;
	}

	address++;

	trace << "Done ! (" << n << " byte" << ((n > 1) ? "s" : "") << " written)" << dendl;
}

bool Memory::readForSound ()
{
	uint16_t address;

	address = EEPROM_SOUND_START;

	trace << "Reading EEPROM for sound variables... ";

	if (EEPROM.read (address) != EEPROM_TEST_BYTE)
	{
		trace << dendl;
		return true; // Returns true to say that variables needs to be initialized
	}

	address++;

	trace << "Done ! (" << address - EEPROM_SOUND_START << " byte" << ((address > 1) ? "s" : "") << " read)" << dendl;

	return false;
}

void Memory::writeForAlarms ()
{
	uint16_t address, n;

	address = EEPROM_ALARM_START;
	n       = 0;

	trace << "Writing EEPROM for alarm variables... ";

	if (EEPROM.read (address) != EEPROM_TEST_BYTE)
	{
		EEPROM.write (address, EEPROM_TEST_BYTE);
		n++;
	}

	address++;

	for (unsigned int i = 0; i < sizeof(uint16_t); i++)
		if (EEPROM.read (address + i) != (uint8_t) (alarms.getDawnTime() >> (i * 8)))
		{
			EEPROM.write (address + i, (uint8_t) (alarms.getDawnTime() >> (i * 8)));
			n++;
		}

	address += sizeof(uint16_t);


	trace << "Done ! (" << n << " byte" << ((n > 1) ? "s" : "") << " written)" << dendl;
}

bool Memory::readForAlarms ()
{
	unsigned int address;

	address = EEPROM_ALARM_START;

	trace << "Reading EEPROM for alarm variables... ";

	if (EEPROM.read (address) != EEPROM_TEST_BYTE)
	{
		trace << endl;
		return true; // Returns true to say that variables needs to be initialized
	}

	address++;

	uint16_t dawnTime;

	dawnTime = 0;
	for (unsigned int i = 0; i < sizeof(uint16_t); i++)
		dawnTime += ((uint16_t) EEPROM.read (address + i)) << (i * 8);

	alarms.setDawnTime (dawnTime);

	address += sizeof(uint16_t);

	trace << "Done ! (" << address - EEPROM_ALARM_START << " byte" << ((address > 1) ? "s" : "") << " read)" << dendl;

	return false;
}

Memory memory = Memory();

#endif // if defined(LUMOS_ARDUINO_MEGA)