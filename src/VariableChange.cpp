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
	changeOn = light.isOn();

	changeLightMod = light.getMode();
	changeSoundMod = sound.getMode();

	for (uint8_t i = LIGHT_MODE_MIN; i <= LIGHT_MODE_MAX; i++)
	{
		changeRgb[i]   = light.getRgb (i);
		changePower[i] = light.getPowerRaw (i);
		changeSpeed[i] = light.getSpeedRaw (i);
	}

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


	for (uint8_t i = LIGHT_MODE_MIN; i < LIGHT_MODE_N; i++)
	{
		if (changeRgb[i] != light.getRgb (i))
		{
			verb << "\"RGB\" of " << utils.getLightModeName (i) << " mode changed from " << changeRgb << " to " << light.getRgb (LIGHT_MODE_CONTINUOUS) << dendl;

			changeRgb[i]    = light.getRgb (i);
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}

		if (changePower[i] != light.getPowerRaw (i))
		{
			verb << "\"Power\" of " << utils.getLightModeName (i) << " mode changed from " << changePower[i] << " (" << utils.map (changePower[i], LIGHT_MIN_POWER, LIGHT_MAX_POWER, SEEKBAR_MIN, SEEKBAR_MAX) << "%) to " << light.getPowerRaw (i) << " (" << light.getPowerPercent() << "%)" << dendl;

			changePower[i]  = light.getPowerRaw (i);
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}

		if (changeSpeed[i] != light.getSpeedRaw (i))
		{
			verb << "\"Speed\" of " << utils.getLightModeName (i) << " mode changed from " << changeSpeed[i] << " to " << light.getSpeedRaw (i) << dendl;

			changeSpeed[i]  = light.getSpeedRaw (i);
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}
	}

	if (changeLightMod != light.getMode())
	{
		verb << "\"Light mode\" changed from " << utils.getLightModeName (changeLightMod) << " (" << changeLightMod << ") to " << utils.getLightModeName (light.getMode()) << " (" << light.getMode() << ")" << dendl;

		changeLightMod = light.getMode();
		flagSendInfo   = true;
	}

	if (changeSoundMod != sound.getMode())
	{
		verb << "\"Sound mode\" changed from " << utils.getSoundModeName (changeSoundMod) << " (" << changeSoundMod << ") to " << utils.getSoundModeName (sound.getMode()) << " (" << sound.getMode() << ")" << dendl;

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
		memory.writeAll();
} // VariableChange::check

void VariableChange::sendInfo ()
{
	trace << "Sending variables infos to the ESP8266" << dendl;

	for (RequestType i = SEND_MIN; i <= SEND_MAX; i++)
	{
		Bounds bounds = utils.getComplementBounds (i);

		for (uint8_t j = bounds.low; j <= bounds.high; j++)
		{
			String message = utils.getMessageTypeName (i);

			switch (i) // info
			{
				case lightRgb:
					message += j;
					message += utils.ltos(light.getRgb (j), HEX);
					break;

				case lightOnOff:
					message += light.isOn();
					break;

				case lightPower:
					message += j;
					message += light.getPowerPercent (j);
					break;

				case lightMode:
					message += light.getMode();
					break;

				case lightModeSpeed:
					message += j;
					message += light.getSpeedPercent (j);
					break;

				case soundMode:
					message += sound.getMode();
					break;

				case soundVolume:
					message += sound.getVolume();
					break;

				case soundOnOff:
					message += sound.isOn();
					break;

				case dawnTime:
					message += alarms.getDawnTime();
					break;

				default:
					break;
			}

			message += 'z';

			verb << message;
			serial.comSerial.print (message);
		}
	}

	verb << dendl;
} // VariableChange::sendInfo

VariableChange variableChange = VariableChange();

#endif // if defined(LUMOS_ARDUINO_MEGA)