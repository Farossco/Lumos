#include <TimeLib.h>
#include <string.h>
#include "Request.h"
#include "Utils.h"
#include "ArduinoLogger.h"
#include "light.h"
#include "Alarms.h"
#include "Sound.h"

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

void Request::process()
{
	if (!error) {
		switch (type) {
		case RequestType::provideTime:
			setTime(information);
			break;

		case RequestType::soundCommand:
			/* sound.command(complement, information); / * TODO : Do this in Sound.cpp (Sound mode action) * / */
			break;

		case RequestType::lightOnOff:
			light_state_set(information != LIGHT_OFF);
			break;

		case RequestType::lightModeRgb:
			light_color_set(rgb_from_code(information), complement);
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
			/* information ? sound.switchOn() : sound.switchOff(); */
			break;

		case RequestType::soundMode:
			/* sound.setMode(information); */
			break;

		case RequestType::soundVolume:
			/* sound.setVolume(information); */
			break;

		case RequestType::alarmDawnVolume:
			/* alarms.setDawnVolume(information); */
			break;

		case RequestType::alarmDawnTime:
			/* alarms.setDawnTime(information); */
			break;
		case RequestType::alarmDawnDuration:
			/* alarms.setDawnDuration(information); */
			break;

		case RequestType::alarmSunsetDuration:
			/* alarms.setSunsetDuration(information); */
			break;

		case RequestType::alarmSunsetDecreaseTime:
			/* alarms.setSunsetDecreaseTime(information); */
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
		trace << " of " << complement;

	trace << " (Current value): ";

	switch (type) {
	case RequestType::provideTime:
		trace << now() << " (" << utils.getClock() << ")";
		break;

	case RequestType::lightOnOff:
		trace << boolalpha << light_state_get();
		break;

	case RequestType::lightModeRgb:
		trace << rgb_to_code(light_color_get(complement));
		break;

	case RequestType::lightModePower:
		trace << light_power_get(complement);
		break;

	case RequestType::lightMode:
		trace << light_mode_get();
		break;

	case RequestType::lightModeSpeed:
		trace << light_speed_get(complement);
		break;

	case RequestType::soundOnOff:
		/* trace << boolalpha << sound.isOn(); */
		break;

	case RequestType::soundMode:
		/* verb << sound.getMode(); */
		break;

	case RequestType::soundVolume:
		/* trace << sound.getVolume(); */
		break;

	case RequestType::alarmDawnVolume:
		/* trace << alarms.getDawnVolume(); */
		break;

	case RequestType::alarmDawnTime:
		/* trace << alarms.getDawnTime(); */
		break;

	case RequestType::alarmDawnDuration:
		/* trace << alarms.getDawnDuration(); */
		break;

	case RequestType::alarmSunsetDuration:
		/* trace << alarms.getSunsetDuration(); */
		break;

	case RequestType::alarmSunsetDecreaseTime:
		/* trace << alarms.getSunsetDecreaseTime(); */
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
