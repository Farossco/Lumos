#include <TimeLib.h>
#include <string.h>
#include "Request.h"
#include "Utils.h"
#include "ArduinoLogger.h"
#include "light.h"
#include "Alarms.h"
#include "Sound.h"

Request::Request(String inputString)
{
	while ((inputString[0] < 'A' || inputString[0] > 'Z') && inputString.length() > 0)
		inputString = inputString.substring(1);

	if (inputString.length() == 0)
		error = RequestError::emptyString;
	else {
		error = RequestError::none;

		type = inputString.substring(0, 3);

		/* [DEBUG] Printing full word, world length and information type */
		trace << "Word: " << inputString << endl;
		verb << "Length: " << inputString.length() << endl;
		verb << "Type: " << type << endl;

		if (type == RequestType::unknown)
			error = RequestError::incorrectType;
		else {
			Bounds infoBounds = type.getInformationBounds();

			inputString = inputString.substring(3); /* Remove 3 first caracteres of the array (The prefix) */

			/* If the data type needs a complement */
			if (type.needsComplement()) {
				Bounds compBounds = type.getComplementBounds();

				complement  = inputString.charAt(0) - '0'; /* Store the complement */
				inputString = inputString.substring(1);    /* Remove first char of the array (the complement) */

				if (type.getComplementType() == ComplementCategory::lightMode)
					verb << "Light mode: " << LightMode(complement) << endl;
				else if (type.getComplementType() == ComplementCategory::soundCommand)
					verb << "Command type: " << SoundCommand(complement) << endl;

				if (complement < compBounds.low || complement > compBounds.high)
					error = RequestError::incorrectComplement;
			}

			information = strtol(inputString.c_str(), NULL, type == RequestType::lightModeRgb ? 16 : 10);

			verb << type.toString() << " (String) : " << inputString << endl;
			verb << type.toString() << " (Decoded): " << ((type == RequestType::lightModeRgb) ? hex : dec) << showbase << uppercase << information << endl;

			if (information < infoBounds.low || information > infoBounds.high)
				error = RequestError::incorrectValue;
		}
		if (error)
			err << "Error : " << error << endl;
	}
}

Request::Request(const String & prefixString, const String & complementString, const String & informationString)
{
	if (prefixString.length() == 0)
		error = RequestError::emptyString;
	else {
		type = prefixString;

		if (type == RequestType::unknown)
			error = RequestError::incorrectType;
		else {
			Bounds infoBounds = type.getInformationBounds();
			Bounds compBounds = type.getComplementBounds();

			if (type.needsComplement()) {
				complement = complementString.toInt();

				if (complementString == "" || complement < compBounds.low || complement > compBounds.high)
					error = RequestError::incorrectComplement;
			}

			information = strtol(informationString.c_str(), NULL, type == RequestType::lightModeRgb ? 16 : 10);

			if (type != RequestType::requestData && (informationString == "" || information < infoBounds.low || information > infoBounds.high))
				error = RequestError::incorrectValue;
		}
	}
}

void Request::decodeInput()
{} /* Request::decode */

void Request::decodeSeparate()
{}

void Request::process()
{
	if (!error) {
		switch (type) {
		case RequestType::provideTime:
			setTime(information);
			break;

		case RequestType::soundCommand:
			sound.command(complement, information); /* TODO : Do this in Sound.cpp (Sound mode action) */
			break;

		case RequestType::lightOnOff:
			(information) ? light_switch_on() : light_switch_off();
			break;

		case RequestType::lightModeRgb:
			light_color_set(information, complement);
			break;

		case RequestType::lightModePower:
			light_power_set(information, complement);
			break;

		case RequestType::lightMode:
			light_mode_set(information);
			break;

		case RequestType::lightModeSpeed:
			light_speed_set(information, complement);
			break;

		case RequestType::soundOnOff:
			information ? sound.switchOn() : sound.switchOff();
			break;

		case RequestType::soundMode:
			sound.setMode(information);
			break;

		case RequestType::soundVolume:
			sound.setVolume(information);
			break;

		case RequestType::alarmDawnVolume:
			alarms.setDawnVolume(information);
			break;

		case RequestType::alarmDawnTime:
			alarms.setDawnTime(information);
			break;
		case RequestType::alarmDawnDuration:
			alarms.setDawnDuration(information);
			break;

		case RequestType::alarmSunsetDuration:
			alarms.setSunsetDuration(information);
			break;

		case RequestType::alarmSunsetDecreaseTime:
			alarms.setSunsetDecreaseTime(information);
			break;
		}

		displayDebug();
	} else if (error != RequestError::emptyString)
		trace << np << endl;
} /* Request::process */

void Request::displayDebug()
{
	trace << type.toString();

	if (type.getComplementType() == ComplementCategory::lightMode)
		trace << " of " << LightMode(complement);
	else if (type.getComplementType() == ComplementCategory::soundCommand)
		trace << " of " << SoundCommand(complement);

	trace << " (Current value): ";

	switch (type) {
	case RequestType::provideTime:
		trace << now() << " (" << utils.getClock() << ")";
		break;

	case RequestType::lightOnOff:
		trace << boolalpha << light_is_on();
		break;

	case RequestType::lightModeRgb:
		trace << light_color_get(complement);
		break;

	case RequestType::lightModePower:
		trace << light_power_raw_get(complement);
		break;

	case RequestType::lightMode:
		trace << light_mode_get();
		break;

	case RequestType::lightModeSpeed:
		trace << light_speed_raw_get(complement);
		break;

	case RequestType::soundOnOff:
		trace << boolalpha << sound.isOn();
		break;

	case RequestType::soundMode:
		verb << sound.getMode();
		break;

	case RequestType::soundVolume:
		trace << sound.getVolume();
		break;

	case RequestType::alarmDawnVolume:
		trace << alarms.getDawnVolume();
		break;

	case RequestType::alarmDawnTime:
		trace << alarms.getDawnTime();
		break;

	case RequestType::alarmDawnDuration:
		trace << alarms.getDawnDuration();
		break;

	case RequestType::alarmSunsetDuration:
		trace << alarms.getSunsetDuration();
		break;

	case RequestType::alarmSunsetDecreaseTime:
		trace << alarms.getSunsetDecreaseTime();
		break;
	}

	trace << dendl;
} /* Request::displayDebug */

RequestError Request::getError()
{
	return error;
}

RequestType Request::getType()
{
	return type;
}

uint8_t Request::getComplement()
{
	return complement;
}

uint32_t Request::getInformation()
{
	return information;
}

String Request::getComplementString()
{
	if (type.needsComplement())
		return String(complement);

	return "";
}

String Request::getInformationString()
{
	return utils.ltos(information, type == RequestType::lightModeRgb ? HEX : DEC);
}
