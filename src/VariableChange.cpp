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
	changeLightOn     = light.on;
	changeLightMode   = light.mode;
	changeRed         = light.red;
	changeGreen       = light.green;
	changeBlue        = light.blue;
	changeLightPower  = light.power;
	changeLightSpeed  = light.speed;
	changeSoundMode   = sound.mode;
	changeSoundVolume = sound.volume;
	changeSoundOn     = sound.on;

	changeDawnTime = alarms.getDawnTime();

	initialized = true;
}

void VariableChange::check ()
{
	if (!initialized)
		return;

	boolean flagSendInfo    = false;
	boolean flagWriteEeprom = false;

	if (changeLightOn != light.on)
	{
		verb << "\"Light On\" changed from " << boolalpha << changeLightOn << " to " << light.on << dendl;

		changeLightOn = light.on;
		flagSendInfo  = true;
	}

	for (LightMode mode = LightMode::MIN; mode <= LightMode::MAX; mode++)
	{
		if (changeRed[mode] != light.red[mode])
		{
			verb << "\"Red\" of " << mode.toString() << " mode changed from " << changeRed[mode] << " to " << light.red[mode] << dendl;

			changeRed[mode] = light.red[mode];
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}

		if (changeGreen[mode] != light.green[mode])
		{
			verb << "\"Green\" of " << mode.toString() << " mode changed from " << changeGreen[mode] << " to " << light.green[mode] << dendl;

			changeGreen[mode] = light.green[mode];
			flagSendInfo      = true;
			flagWriteEeprom   = true;
		}

		if (changeBlue[mode] != light.blue[mode])
		{
			verb << "\"Blue\" of " << mode.toString() << " mode changed from " << changeBlue[mode] << " to " << light.blue[mode] << dendl;

			changeBlue[mode] = light.blue[mode];
			flagSendInfo     = true;
			flagWriteEeprom  = true;
		}

		if (changeLightPower[mode] != light.power[mode])
		{
			verb << "\"Light Power\" of " << mode.toString() << " mode changed from " << changeLightPower[mode] << " (" << utils.map (changeLightPower[mode], LightSetting::MIN_POWER, LightSetting::MAX_POWER, LightSetting::MIN_PERCENT, LightSetting::MAX_PERCENT) << "%) to " << light.power[mode] << " (" << light.getPowerPercent() << "%)" << dendl;

			changeLightPower[mode] = light.power[mode];
			flagSendInfo           = true;
			flagWriteEeprom        = true;
		}

		if (changeLightSpeed[mode] != light.speed[mode])
		{
			verb << "\"Light Speed\" of " << mode.toString() << " mode changed from " << changeLightSpeed[mode] << " to " << light.speed[mode] << dendl;

			changeLightSpeed[mode] = light.speed[mode];
			flagSendInfo           = true;
			flagWriteEeprom        = true;
		}
	}

	if (changeLightMode != light.mode)
	{
		verb << "\"Light mode\" changed from " << changeLightMode.toString() << " (" << changeLightMode << ") to " << light.mode.toString() << " (" << light.mode << ")" << dendl;

		changeLightMode = light.mode;
		flagSendInfo    = true;
	}

	if (changeSoundMode != sound.mode)
	{
		verb << "\"Sound mode\" changed from " << changeSoundMode.toString() << " (" << changeSoundMode << ") to " << sound.mode.toString() << " (" << sound.mode << ")" << dendl;

		changeSoundMode = sound.mode;
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	if (changeSoundVolume != sound.volume)
	{
		verb << "\"Sound Volume\" changed from " << changeSoundVolume << " to " << sound.volume << dendl;

		changeSoundVolume = sound.volume;
		flagSendInfo      = true;
		flagWriteEeprom   = true;
	}

	if (changeSoundOn != sound.on)
	{
		verb << "\"Sound On\" changed from " << changeSoundOn << " to " << sound.on << dendl;

		changeSoundOn   = sound.on;
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
			String message = type.toString (true);

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