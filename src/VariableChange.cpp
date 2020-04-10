#if defined(LUMOS_ARDUINO_MEGA)

#include <stdlib.h>
#include <string.h>
#include "VariableChange.h"
#include "Light.h"
#include "Logger.h"
#include "Memory.h"
#include "Sound.h"
#include "Request.h"
#include "Utils.h"
#include "Alarms.h"
#include "ArduinoSerial.h"

VariableChange::VariableChange() : initialized (false)
{ }

void VariableChange::init ()
{
	// Initializing to default values
	changeOn       = light.isOn();
	changeRgb      = light.getRgb (LIGHT_MOD_CONTINUOUS);
	changeLightMod = light.getMode();
	changeSoundMod = sound.getMode();
	for (uint8_t i = LIGHT_MOD_MIN; i < LIGHT_MOD_N; i++)
		changePower[i] = light.getPowerRaw (i);
	for (uint8_t i = LIGHT_MOD_MIN; i < LIGHT_MOD_N; i++)
		changeSpeed[i] = light.getSpeed (i);
	changeDawnTime = alarms.getDawnTime();

	initialized = true;
}

void VariableChange::check ()
{
	if (!initialized)
		return;

	boolean flagSendInfo    = false;
	boolean flagWriteEeprom = false;

	if (changeOn != light.isOn())
	{
		verb << "\"On\" changed from " << boolalpha << changeOn << " to " << light.isOn() << dendl;

		changeOn     = light.isOn();
		flagSendInfo = true;
	}

	if (changeRgb != light.getRgb (LIGHT_MOD_CONTINUOUS))
	{
		verb << "\"RGB\" of default mode changed from " << changeRgb << " to " << light.getRgb (LIGHT_MOD_CONTINUOUS) << dendl;

		changeRgb       = light.getRgb (LIGHT_MOD_CONTINUOUS);
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	for (uint8_t i = LIGHT_MOD_MIN; i < LIGHT_MOD_N; i++)
	{
		if (changePower[i] != light.getPowerRaw (i))
		{
			verb << "\"Power\" of " << utils.getLightModeName (i, CAPS_NONE) << " mode changed from " << changePower[i] << " (" << utils.map (changePower[i], LIGHT_MIN_POWER, LIGHT_MAX_POWER, SEEKBAR_MIN, SEEKBAR_MAX) << "%) to " << light.getPowerRaw (i) << " (" << light.getPowerPercent() << "%)" << dendl;

			changePower[i]  = light.getPowerRaw (i);
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}

		if (changeSpeed[i] != light.getSpeed (i))
		{
			verb << "\"Speed\" of " << utils.getLightModeName (i, CAPS_NONE) << " mode changed from " << changeSpeed[i] << " to " << light.getSpeed (i) << dendl;

			changeSpeed[i]  = light.getSpeed (i);
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}
	}

	if (changeLightMod != light.getMode())
	{
		verb << "\"Light mode\" changed from " << utils.getLightModeName (changeLightMod, CAPS_NONE) << " (" << changeLightMod << ") to " << utils.getLightModeName (light.getMode(), CAPS_NONE) << " (" << light.getMode() << ")" << dendl;

		changeLightMod  = light.getMode();
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	if (changeSoundMod != sound.getMode())
	{
		verb << "\"Sound mode\" changed from " << utils.getSoundModeName (changeSoundMod, CAPS_NONE) << " (" << changeSoundMod << ") to " << utils.getSoundModeName (sound.getMode(), CAPS_NONE) << " (" << sound.getMode() << ")" << dendl;

		changeSoundMod  = sound.getMode();
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	if (changeDawnTime != alarms.getDawnTime())
	{
		verb << "\"Dawn time\" changed from " << changeDawnTime / 60 << ":" << changeDawnTime % 60 << " (" << changeDawnTime << ") to " << alarms.getDawnTime() / 60 << ":" << alarms.getDawnTime() % 60 << " (" << alarms.getDawnTime() << ")" << dendl;

		changeDawnTime  = alarms.getDawnTime();
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	if (flagSendInfo)
		sendInfo();

	if (flagWriteEeprom)
		memory.writeForAll();
} // VariableChange::check

void VariableChange::sendInfo ()
{
	trace << "Sending variables infos to the ESP8266" << dendl;

	for (RequestMessageType i = SEND_MIN; i <= SEND_MAX; i++)
	{
		for (uint8_t j = utils.messageTypeComplementBounds (i, COMPLEMENT_MIN); j <= utils.messageTypeComplementBounds (i, COMPLEMENT_MAX); j++)
		{
			char information[15] = "\n"; // TODO : Change to a String

			sprintf (information, "%s", utils.getMessageTypeName (i)); // Prefix

			switch (i) // info
			{
				case RGB:
					sprintf (information + strlen (information), "%d%lX", j, light.getRgb (j));
					break;

				case LON:
					sprintf (information + strlen (information), "%d", light.isOn());
					break;

				case POW:
					sprintf (information + strlen (information), "%d%d", j, light.getPowerPercent (j));
					break;

				case LMO:
					sprintf (information + strlen (information), "%d", light.getMode());
					break;

				case SPEED:
					sprintf (information + strlen (information), "%d%d", j, (uint16_t) utils.map (light.getSpeed (j), LIGHT_MIN_SPEED[j], LIGHT_MAX_SPEED[j], SEEKBAR_MIN, SEEKBAR_MAX));
					break;

				case SMO:
					sprintf (information + strlen (information), "%d", sound.getMode());
					break;

				case VOL:
					sprintf (information + strlen (information), "%d", sound.getVolume());
					break;

				case SON:
					sprintf (information + strlen (information), "%d", sound.isOn());
					break;

				case DTM:
					sprintf (information + strlen (information), "%d", alarms.getDawnTime());
					break;

				default:
					break;
			}
			sprintf (information + strlen (information), "z"); // Suffix

			verb << information;
			serial.comSerial.print (information);
		}
	}

	verb << dendl;
} // VariableChange::sendInfo

VariableChange variableChange = VariableChange();

#endif // if defined(LUMOS_ARDUINO_MEGA)