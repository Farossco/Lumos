/* TODO: Remove this file */

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
#include "serial_com.h"

VariableChange::VariableChange() : initialized(false) {}

void VariableChange::init()
{
	/* Initializing to default values */
	changeLightOn     = light.on;
	changeLightMode   = light.mode;
	changeRgbs        = light.rgbs;
	changeLightPowers = light.powers;
	changeLightSpeeds = light.speeds;

	changeSoundMode   = sound.mode;
	changeSoundVolume = sound.volume;
	changeSoundOn     = sound.on;

	changeDawnTime           = alarms.dawnTime;
	changeDawnDuration       = alarms.dawnDuration;
	changeSunsetDuration     = alarms.sunsetDuration;
	changeSunsetDecreaseTime = alarms.sunsetDecreaseTime;
	changeDawnVolume         = alarms.dawnVolume;

	initialized = true;
}

void VariableChange::check()
{
	if (!initialized)
		return;

	boolean flagSendInfo    = false;
	boolean flagWriteEeprom = false;

	if (changeLightOn != light.on) {
		verb << "\"Light On\" changed from " << boolalpha << changeLightOn << " to " << light.on << dendl;

		changeLightOn = light.on;
		flagSendInfo  = true;
	}

	/* TODO : simplify and remove debug */
	for (LightMode mode; mode < LightMode::N; mode++) {
		if (changeRgbs[mode] != light.rgbs[mode]) {
			verb << "\"RGB\" of " << mode << " mode changed from " << changeRgbs[mode] << " to " << light.rgbs[mode] << dendl;

			changeRgbs[mode] = light.rgbs[mode];
			flagSendInfo     = true;
			flagWriteEeprom  = true;
		}

		if (changeLightPowers[mode] != light.powers[mode]) {
			verb << "\"Light Power\" of " << mode << " mode changed from " << changeLightPowers[mode] << " to " << light.powers[mode] << dendl;

			changeLightPowers[mode] = light.powers[mode];
			flagSendInfo            = true;
			flagWriteEeprom         = true;
		}

		if (changeLightSpeeds[mode] != light.speeds[mode]) {
			verb << "\"Light Speed\" of " << mode << " mode changed from " << changeLightSpeeds[mode] << " to " << light.speeds[mode] << dendl;

			changeLightSpeeds[mode] = light.speeds[mode];
			flagSendInfo            = true;
			flagWriteEeprom         = true;
		}
	}

	if (changeLightMode != light.mode) {
		verb << "\"Light mode\" changed from " << changeLightMode << " to " << light.mode << dendl;

		changeLightMode = light.mode;
		flagSendInfo    = true;
	}

	if (changeSoundMode != sound.mode) {
		verb << "\"Sound mode\" changed from " << changeSoundMode << " to " << sound.mode << dendl;

		changeSoundMode = sound.mode;
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	if (changeSoundVolume != sound.volume) {
		verb << "\"Sound Volume\" changed from " << changeSoundVolume << " to " << sound.volume << dendl;

		changeSoundVolume = sound.volume;
		flagSendInfo      = true;
		flagWriteEeprom   = true;
	}

	if (changeSoundOn != sound.on) {
		verb << "\"Sound On\" changed from " << changeSoundOn << " to " << sound.on << dendl;

		changeSoundOn   = sound.on;
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	if (changeDawnTime != alarms.dawnTime) {
		verb << "\"Dawn time\" changed from " << changeDawnTime << " to " << alarms.dawnTime << dendl;

		changeDawnTime  = alarms.dawnTime;
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	if (changeDawnDuration != alarms.dawnDuration) {
		verb << "\"Dawn Duration\" changed from " << changeDawnDuration << " to " << alarms.dawnDuration << dendl;

		changeDawnDuration = alarms.dawnDuration;
		flagSendInfo       = true;
		flagWriteEeprom    = true;
	}

	if (changeSunsetDuration != alarms.sunsetDuration) {
		verb << "\"Sunset Duration\" changed from " << changeSunsetDuration << " to " << alarms.sunsetDuration << dendl;

		changeSunsetDuration = alarms.sunsetDuration;
		flagSendInfo         = true;
		flagWriteEeprom      = true;
	}

	if (changeSunsetDecreaseTime != alarms.sunsetDecreaseTime) {
		verb << "\"Sunset Decrease Time\" changed from " << changeSunsetDecreaseTime << " to " << alarms.sunsetDecreaseTime << dendl;

		changeSunsetDecreaseTime = alarms.sunsetDecreaseTime;
		flagSendInfo             = true;
		flagWriteEeprom          = true;
	}

	if (changeDawnVolume != alarms.dawnVolume) {
		verb << "\"Dawn Volume\" changed from " << changeDawnVolume << " to " << alarms.dawnVolume << dendl;

		changeDawnVolume = alarms.dawnVolume;
		flagSendInfo     = true;
		flagWriteEeprom  = true;
	}

	if (flagSendInfo) sendData();
	if (flagWriteEeprom) memory.writeAll();
} /* VariableChange::check */

void VariableChange::sendData()
{
	trace << "Sending variables infos to the ESP8266" << dendl;

	for (RequestType type = RequestType::SEND_START; type < RequestType::N; type++) {
		Bounds bounds = type.getComplementBounds();

		for (uint8_t complement = bounds.low; complement <= bounds.high; complement++) {
			String message = type.toString(true);

			if (type.needsComplement())
				message += complement;

			switch (type) { /* info */
			case RequestType::unknown:
			case RequestType::requestTime:
			case RequestType::requestData:
			case RequestType::soundCommand:
			case RequestType::provideTime:
				break;

			case RequestType::lightModeRgb:
				message += utils.ltos(light.getRgb(complement).value(), HEX);
				break;

			case RequestType::lightOnOff:
				message += light.isOn();
				break;

			case RequestType::lightModePower:
				message += light.getPowerPercent(complement).value();
				break;

			case RequestType::lightMode:
				message += light.getMode();
				break;

			case RequestType::lightModeSpeed:
				message += light.getSpeedPercent(complement).value();
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

			case RequestType::alarmDawnVolume:
				message += alarms.getDawnVolume().value();
				break;

			case RequestType::alarmDawnTime:
				message += alarms.getDawnTime().value();
				break;

			case RequestType::alarmDawnDuration:
				message += alarms.getDawnDuration().value();
				break;

			case RequestType::alarmSunsetDuration:
				message += alarms.getSunsetDuration().value();
				break;

			case RequestType::alarmSunsetDecreaseTime:
				message += alarms.getSunsetDecreaseTime().value();
				break;
			}

			message += 'z';

			verb << message;
			/* serial.comSerial.print(message); */
		}
	}

	verb << dendl;
} /* VariableChange::sendData */

VariableChange variableChange = VariableChange();
