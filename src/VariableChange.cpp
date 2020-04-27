#if defined(LUMOS_ARDUINO_MEGA)

#include <stdlib.h>
#include <string.h>
#include "VariableChange.h"
#include "Light.h"
#include "ArduinoLogger.h"
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
	changeOn        = light.on;
	changeLightMode = light.mode;
	changeSoundMode = sound.mode;
	changeRed       = light.red;
	changeGreen     = light.green;
	changeBlue      = light.blue;
	changePower     = light.power;
	changeSpeed     = light.speed;

	changeDawnTime = alarms.getDawnTime();

	initialized = true;
}

void VariableChange::check ()
{
	if (!initialized)
		return;

	boolean flagSendInfo    = false;
	boolean flagWriteEeprom = false;

	if (changeOn != light.on)
	{
		verb << "\"On\" changed from " << boolalpha << changeOn << " to " << light.on << dendl;

		changeOn     = light.on;
		flagSendInfo = true;
	}

	for (LightMode i = LightMode::MIN; i <= LightMode::MAX; i++)
	{
		if (changeRed[i] != light.red[i])
		{
			verb << "\"Red\" of " << utils.getLightModeName (i) << " mode changed from " << changeRed[i] << " to " << light.red[i] << dendl;

			changeRed[i]    = light.red[i];
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}

		if (changeGreen[i] != light.green[i])
		{
			verb << "\"Green\" of " << utils.getLightModeName (i) << " mode changed from " << changeGreen[i] << " to " << light.green[i] << dendl;

			changeGreen[i]  = light.green[i];
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}

		if (changeBlue[i] != light.blue[i])
		{
			verb << "\"Blue\" of " << utils.getLightModeName (i) << " mode changed from " << changeBlue[i] << " to " << light.blue[i] << dendl;

			changeBlue[i]   = light.blue[i];
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}

		if (changePower[i] != light.power[i])
		{
			verb << "\"Power\" of " << utils.getLightModeName (i) << " mode changed from " << changePower[i] << " (" << utils.map (changePower[i], LightSetting::MIN_POWER, LightSetting::MAX_POWER, LightSetting::MIN_PERCENT, LightSetting::MAX_PERCENT) << "%) to " << light.power[i] << " (" << light.getPowerPercent() << "%)" << dendl;

			changePower[i]  = light.power[i];
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}

		if (changeSpeed[i] != light.speed[i])
		{
			verb << "\"Speed\" of " << utils.getLightModeName (i) << " mode changed from " << changeSpeed[i] << " to " << light.speed[i] << dendl;

			changeSpeed[i]  = light.speed[i];
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}
	}

	if (changeLightMode != light.mode)
	{
		verb << "\"Light mode\" changed from " << utils.getLightModeName (changeLightMode) << " (" << changeLightMode << ") to " << utils.getLightModeName (light.mode) << " (" << light.mode << ")" << dendl;

		changeLightMode = light.mode;
		flagSendInfo    = true;
	}

	if (changeSoundMode != sound.mode)
	{
		verb << "\"Sound mode\" changed from " << utils.getSoundModeName (changeSoundMode) << " (" << changeSoundMode << ") to " << utils.getSoundModeName (sound.mode) << " (" << sound.mode << ")" << dendl;

		changeSoundMode = sound.mode;
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	if (changeDawnTime != alarms.dawnTime)
	{
		verb << "\"Dawn time\" changed from " << changeDawnTime / 60 << ":" << changeDawnTime % 60 << " (" << changeDawnTime << ") to " << alarms.dawnTime / 60 << ":" << alarms.dawnTime % 60 << " (" << alarms.dawnTime << ")" << dendl;

		changeDawnTime  = alarms.dawnTime;
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

	for (RequestType type = RequestType::SEND_MIN; type <= RequestType::SEND_MAX; type++)
	{
		Bounds bounds = type.getComplementBounds();

		for (uint8_t complement = bounds.low; complement <= bounds.high; complement++)
		{
			String message = utils.getMessageTypeName (type);

			switch (type) // info
			{
				case RequestType::lightRgb:
					message += complement;
					message += utils.ltos (light.getRgb (complement), HEX);
					break;

				case RequestType::lightOnOff:
					message += light.isOn();
					break;

				case RequestType::lightPower:
					message += complement;
					message += light.getPowerPercent (complement);
					break;

				case RequestType::lightMode:
					message += light.getMode();
					break;

				case RequestType::lightModeSpeed:
					message += complement;
					message += light.getSpeedPercent (complement);
					break;

				case RequestType::soundMode:
					message += sound.getMode();
					break;

				case RequestType::soundVolume:
					message += sound.getVolume();
					break;

				case RequestType::soundOnOff:
					message += sound.isOn();
					break;

				case RequestType::dawnTime:
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