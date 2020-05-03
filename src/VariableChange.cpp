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
#include "SerialCom.h"

VariableChange::VariableChange() : initialized (false)
{ }

void VariableChange::init ()
{
	// Initializing to default values
	changeLightOn     = light.on;
	changeLightMode   = light.mode;
	changeRgbs        = light.rgbs;
	changeLightPowers = light.powers;
	changeLightSpeeds = light.speeds;
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

	// TODO : simplify and remove debug
	for (LightMode mode = LightMode::MIN; mode <= LightMode::MAX; mode++)
	{
		if (changeRgbs[mode] != light.rgbs[mode])
		{
			verb << "\"Rgb\" of " << mode.toString() << " mode changed from 0x" << setfill ('0') << setw (6) << hex << changeRgbs[mode] << " to 0x" << light.rgbs[mode] << dendl;

			changeRgbs[mode] = light.rgbs[mode];
			flagSendInfo     = true;
			flagWriteEeprom  = true;
		}

		if (changeLightPowers[mode] != light.powers[mode])
		{
			verb << "\"Light Power\" of " << mode.toString() << " mode changed from " << changeLightPowers[mode] << " (" << changeLightPowers[mode].toPercent() << "%) to " << light.powers[mode] << " (" << light.getPowerPercent() << "%)" << dendl;

			changeLightPowers[mode] = light.powers[mode];
			flagSendInfo            = true;
			flagWriteEeprom         = true;
		}

		if (changeLightSpeeds[mode] != light.speeds[mode])
		{
			verb << "\"Light Speed\" of " << mode.toString() << " mode changed from " << changeLightSpeeds[mode] << " to " << light.speeds[mode] << dendl;

			changeLightSpeeds[mode] = light.speeds[mode];
			flagSendInfo            = true;
			flagWriteEeprom         = true;
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
		sendData();

	if (flagWriteEeprom)
		memory.writeAll();
} // VariableChange::check

void VariableChange::sendData ()
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
				case RequestType::lightModeRgb:
					message += complement;
					message += utils.ltos (light.getRgb (complement).value(), HEX);
					break;

				case RequestType::lightOnOff:
					message += light.isOn();
					break;

				case RequestType::lightModePower:
					message += complement;
					message += light.getPowerPercent (complement).value();
					break;

				case RequestType::lightMode:
					message += light.getMode();
					break;

				case RequestType::lightModeSpeed:
					message += complement;
					message += light.getSpeedPercent (complement).value();
					break;

				case RequestType::soundMode:
					message += sound.getMode();
					break;

				case RequestType::soundVolume:
					message += sound.getVolume().value();
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
			serial.comSerialTx.print (message);
		}
	}

	verb << dendl;
} // VariableChange::sendData

VariableChange variableChange = VariableChange();

#endif // if defined(LUMOS_ARDUINO_MEGA)