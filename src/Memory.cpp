#include "Memory.h"
#include "light.h"
#include "ArduinoLogger.h"
#include "Alarms.h"
#include "SdCard.h"
#include "Sound.h"

Memory::Memory()
{}


void Memory::dump(uint16_t start, uint16_t limit)
{
	limit = constrain(limit, 0, EEPROM.length());

	inf << "EEPROM dump from " << hex << start << " to " << limit << dendl;

	for (uint16_t index = start; index <= limit; index++) {
		uint8_t value = EEPROM.read(index);

		inf << dsb(sd.file) << "0x" << noshowbase << setfill('0') << setw(4) << hex << index << " : "
		    << "0b" << setw(8) << bin << value
		    << " (0x" << setw(2) << hex << value << ")" << endl;
	}

	inf << dsb(sd.file) << np << endl;
}

void Memory::writeAll()
{
	writeLight();
	writeSound();
	writeAlarms();
}

bool Memory::readAll()
{
	return readLight() || readSound() || readAlarms();
}

void Memory::writeLight()
{
	startWrite(EEPROM_LIGHT_START);

	verb << "Writing EEPROM for light variables... ";

	/* put(light.rgbs);
	 * put(light.powers);
	 * put(light.speeds);
	 */

	verb << "Done ! (" << _n << " byte" << ((_n > 1) ? "s" : "") << " written)" << dendl;
}

bool Memory::readLight()
{
	if (startRead(EEPROM_LIGHT_START))
		return true;  /* Returns true to say that variables needs to be initialized */

	verb << "Reading EEPROM for light variables... ";

	/* light.rgbs   = get<LightRgbArray>();
	 * light.powers = get<LightPowerArray>();
	 * light.speeds = get<LightSpeedArray>();
	 */

	verb << "Done ! (" << _address - EEPROM_LIGHT_START << " byte" << ((_address > 1) ? "s" : "") << " read)" << dendl;

	return false;
}

void Memory::writeSound()
{
	startWrite(EEPROM_SOUND_START);

	verb << "Writing EEPROM for sound variables... ";

	put(sound.volume);

	verb << "Done ! (" << _n << " byte" << ((_n > 1) ? "s" : "") << " written)" << dendl;
}

bool Memory::readSound()
{
	if (startRead(EEPROM_SOUND_START))
		return true;

	verb << "Reading EEPROM for sound variables... ";

	sound.volume = get<SoundVolume>();

	verb << "Done ! (" << _address - EEPROM_SOUND_START << " byte" << ((_address > 1) ? "s" : "") << " read)" << dendl;

	return false;
}

void Memory::writeAlarms()
{
	startWrite(EEPROM_ALARM_START);

	verb << "Writing EEPROM for alarm variables... ";

	put(alarms.dawnTime);
	put(alarms.dawnDuration);
	put(alarms.sunsetDuration);
	put(alarms.sunsetDecreaseTime);
	put(alarms.dawnVolume);

	verb << "Done ! (" << _n << " byte" << ((_n > 1) ? "s" : "") << " written)" << dendl;
}

bool Memory::readAlarms()
{
	if (startRead(EEPROM_ALARM_START))
		return true;

	verb << "Reading EEPROM for alarm variables... ";

	alarms.dawnTime           = get<Time>();
	alarms.dawnDuration       = get<Timing>();
	alarms.sunsetDuration     = get<Timing>();
	alarms.sunsetDecreaseTime = get<Timing>();
	alarms.dawnVolume         = get<SoundVolume>();

	verb << "Done ! (" << _address - EEPROM_ALARM_START << " byte" << ((_address > 1) ? "s" : "") << " read)" << dendl;

	return false;
}

void Memory::startWrite(uint16_t address)
{
	_address = address;
	_n       = 0;

	if (EEPROM.read(_address) != EEPROM_TEST_BYTE) {
		EEPROM.write(_address, EEPROM_TEST_BYTE);
		_n++;
	}

	_address++;
}

bool Memory::startRead(uint16_t address)
{
	if (EEPROM.read(address) != EEPROM_TEST_BYTE) /* Returns true if variables needs to be initialized */
		return true;

	_n = 1; /* We've just read 1 byte */

	_address = address + 1;

	return false;
}

Memory memory = Memory();
