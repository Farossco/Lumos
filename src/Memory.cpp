#if defined(LUMOS_ARDUINO_MEGA)

#include "Memory.h"
#include "Light.h"
#include "ArduinoLogger.h"
#include "Alarms.h"
#include "SdCard.h"

Memory::Memory()
{ }


void Memory::dump (uint16_t start, uint16_t limit)
{
	limit = constrain (limit, 0, EEPROM.length());

	trace << "EEPROM dump from " << hex << start << " to " << limit << dendl;

	for (uint16_t index = start; index <= limit; index++)
	{
		uint8_t value = EEPROM.read (index);

		trace << dsb (sd.file) << "0x" << noshowbase << setfill ('0') << setw (4) << hex << index << " : "
		      << "0b" << setw (8) << bin << value
		      << " (0x" << setw (2) << hex << value << ")" << endl;
	}

	trace << dsb (sd.file) << np << endl;
}

void Memory::writeAll ()
{
	writeLight();
	writeSound();
	writeAlarms();
}

bool Memory::readAll ()
{
	return readLight() || readSound() || readAlarms();
}

void Memory::writeLight ()
{
	startWrite (EEPROM_LIGHT_START);

	trace << "Writing EEPROM for light variables... ";

	// All RGB values
	for (uint8_t i = LIGHT_MODE_MIN; i <= LIGHT_MODE_MAX; i++)
		put (light.getRgb (i));

	// All Light values
	for (uint8_t i = LIGHT_MODE_MIN; i <= LIGHT_MODE_MAX; i++)
		put (light.getPowerRaw (i));

	// All speed values
	for (uint8_t i = LIGHT_MODE_MIN; i <= LIGHT_MODE_MAX; i++)
		put (light.getSpeedRaw (i));

	trace << "Done ! (" << _n << " byte" << ((_n > 1) ? "s" : "") << " written)" << dendl;
}

bool Memory::readLight ()
{
	if (startRead (EEPROM_LIGHT_START))
		return true;  // Returns true to say that variables needs to be initialized

	trace << "Reading EEPROM for light variables... ";

	for (uint8_t i = LIGHT_MODE_MIN; i <= LIGHT_MODE_MAX; i++)
		light.setRgb (get<uint32_t>(), i);

	for (uint8_t i = LIGHT_MODE_MIN; i <= LIGHT_MODE_MAX; i++)
		light.setPowerRaw (get<uint8_t>(), i);

	for (uint8_t i = LIGHT_MODE_MIN; i <= LIGHT_MODE_MAX; i++)
		light.setSpeedRaw (get<uint16_t>(), i);

	trace << "Done ! (" << _address - EEPROM_LIGHT_START << " byte" << ((_address > 1) ? "s" : "") << " read)" << dendl;

	return false;
}

void Memory::writeSound ()
{
	startWrite (EEPROM_SOUND_START);

	trace << "Writing EEPROM for sound variables... ";

	trace << "Done ! (" << _n << " byte" << ((_n > 1) ? "s" : "") << " written)" << dendl;
}

bool Memory::readSound ()
{
	if (startRead (EEPROM_SOUND_START))
		return true;

	trace << "Reading EEPROM for sound variables... ";

	trace << "Done ! (" << _address - EEPROM_SOUND_START << " byte" << ((_address > 1) ? "s" : "") << " read)" << dendl;

	return false;
}

void Memory::writeAlarms ()
{
	startWrite (EEPROM_ALARM_START);

	trace << "Writing EEPROM for alarm variables... ";

	put (alarms.getDawnTime());

	trace << "Done ! (" << _n << " byte" << ((_n > 1) ? "s" : "") << " written)" << dendl;
}

bool Memory::readAlarms ()
{
	if (startRead (EEPROM_ALARM_START))
		return true;

	trace << "Reading EEPROM for alarm variables... ";

	alarms.setDawnTime (get<uint16_t>());

	trace << "Done ! (" << _address - EEPROM_ALARM_START << " byte" << ((_address > 1) ? "s" : "") << " read)" << dendl;

	return false;
}

void Memory::startWrite (uint16_t address)
{
	_address = address;
	_n       = 0;

	if (EEPROM.read (_address) != EEPROM_TEST_BYTE)
	{
		EEPROM.write (_address, EEPROM_TEST_BYTE);
		_n++;
	}

	_address++;
}

bool Memory::startRead (uint16_t address)
{
	if (EEPROM.read (address) != EEPROM_TEST_BYTE) // Returns true if variables needs to be initialized
		return true;

	_n = 1; // We've just read 1 byte

	_address = address + 1;

	return false;
}

Memory memory = Memory();

#endif // if defined(LUMOS_ARDUINO_MEGA)