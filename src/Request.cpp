#include <TimeLib.h>
#include <string.h>
#include "Request.h"
#include "Utils.h"
#include "ArduinoLogger.h"
#include "Light.h"
#include "Alarms.h"
#include "Sound.h"

Request::Request(String inputString)
{
	while ((inputString[0] < 'A' || inputString[0] > 'Z') && inputString.length() > 0)
		inputString = inputString.substring (1);

	if (inputString.length() == 0)
		error = RequestError::emptyString;
	else
	{
		error = RequestError::none;

		type = inputString.substring (0, 3);

		// [DEBUG] Printing full word, world length and information type
		trace << "Word: " << inputString << endl;
		verb << "Length: " << inputString.length() << endl;
		verb << "Type: " << type.toString() << " (" << type << ")" << endl;

		if (type == RequestType::unknown)
			error = RequestError::incorrectType;
		else
		{
			Bounds infoBounds = type.getInformationBounds();

			inputString = inputString.substring (3); // Remove 3 first caracteres of the array (The prefix)

			// If the data type needs a complement
			if (type.needsComplement())
			{
				Bounds compBounds = type.getComplementBounds();

				complement  = inputString.charAt (0) - '0'; // Store the complement
				inputString = inputString.substring (1);    // Remove first char of the array (the complement)

				if (type.getComplementType() == ComplementCategory::lightMode)
					verb << "Light mode: " << LightMode (complement).toString() << " (" << complement << ")" << endl;
				else if (type.getComplementType() == ComplementCategory::soundCommand)
					verb << "Command type: " << SoundCommand (complement).toString() << " (" << complement << ")" << endl;

				if (complement < compBounds.low || complement > compBounds.high)
					error = RequestError::incorrectComplement;
			}

			information = strtol (inputString.c_str(), NULL, type == RequestType::lightModeRgb ? 16 : 10);

			verb << type.toString() << ": " << inputString << endl;
			verb << type.toString() << " (decoded): " << ((type == RequestType::lightModeRgb) ? hex : dec) << showbase << uppercase << information << endl;

			if (information < infoBounds.low || information > infoBounds.high)
				error = RequestError::incorrectValue;
		}
		if (error)
			err << "Error : " << error.toString() << " (" << error << ")" << endl;
	}
}

Request::Request(const String & prefixString, const String & complementString, const String & informationString)
{
	if (prefixString.length() == 0)
		error = RequestError::emptyString;
	else
	{
		type = prefixString;

		if (type == RequestType::unknown)
			error = RequestError::incorrectType;
		else
		{
			Bounds infoBounds = type.getInformationBounds();
			Bounds compBounds = type.getComplementBounds();

			if (type.needsComplement())
			{
				complement = complementString.toInt();

				if (complementString == "" || complement < compBounds.low || complement > compBounds.high)
					error = RequestError::incorrectComplement;
			}

			information = strtol (informationString.c_str(), NULL, type == RequestType::lightModeRgb ? 16 : 10);

			if (type != RequestType::requestData && (informationString == "" || information < infoBounds.low || information > infoBounds.high))
				error = RequestError::incorrectValue;
		}
	}
}

void Request::decodeInput ()
{ } // Request::decode

void Request::decodeSeparate ()
{ }

void Request::process ()
{
	if (!error)
	{
		switch (type)
		{
			case RequestType::provideTime:
				setTime (information);
				break;

			case RequestType::soundCommand:
				#if defined(LUMOS_ARDUINO_MEGA)
				sound.command (complement, information); // TODO : Do this in Sound.cpp (Sound mode action)
				#endif
				break;

			case RequestType::lightOnOff:
				(information) ? light.switchOn() : light.switchOff();
				break;

			case RequestType::lightModeRgb:
				light.setRgb (information, complement);
				break;

			case RequestType::lightModePower:
				light.setPower (information, complement);
				break;

			case RequestType::lightMode:
				light.setMode (information);
				break;

			case RequestType::lightModeSpeed:
				light.setSpeed (information, complement);
				break;

			case RequestType::soundOnOff:
				information ? sound.switchOn() : sound.switchOff();
				break;

			case RequestType::soundMode:
				sound.setMode (information);
				break;

			case RequestType::soundVolume:
				sound.setVolume (information);
				break;

			case RequestType::alarmDawnVolume:
				alarms.setDawnVolume (information);
				break;

			case RequestType::alarmDawnTime:
				alarms.setDawnTime (information);
				break;
			case RequestType::alarmDawnDuration:
				alarms.setDawnDuration (information);
				break;

			case RequestType::alarmSunsetDuration:
				alarms.setSunsetDuration (information);
				break;

			case RequestType::alarmSunsetDecreaseTime:
				alarms.setSunsetDecreaseTime (information);
				break;
		}

		displayDebug();
	}
	else if (error != RequestError::emptyString)
		trace << np << endl;
} // Request::process

void Request::displayDebug ()
{
	switch (type)
	{
		case RequestType::provideTime:
			verb << "Time (Current value): " << now() << endl;
			trace << "Time (Current value) (readable): " << utils.getClock() << dendl;
			break;

		case RequestType::soundCommand:
			verb << "Command data: (" << information << ")" << dendl;
			break;

		case RequestType::lightOnOff:
			trace << "Light On/Off (Current value): " << boolalpha << light.isOn() << dendl;
			break;

		case RequestType::lightModeRgb:
			trace << "RGB of " << LightMode (complement).toString() << " (Current value): " << light.getRgb (complement) << dendl;
			break;

		case RequestType::lightModePower:
			trace << "Power of " << LightMode (complement).toString() << " (Current value): " << light.getPowerRaw (complement) << dendl;
			break;

		case RequestType::lightMode:
			verb << "Light mode (Text): " << LightMode (information).toString() << " (" << information << ")" << endl;
			trace << "Light mode (Current value): " << light.getMode().toString() << " (" << light.getMode() << ")" << dendl;
			break;

		case RequestType::lightModeSpeed:
			verb << "Min Speed: " << LightSpeed::MIN << endl;
			verb << "Max Speed: " << LightSpeed::MAX << endl;
			trace << "Speed of " << LightMode (complement).toString() << " (Current value): " << light.getSpeedRaw (complement) << dendl;
			break;

		case RequestType::soundOnOff:
			trace << "On/Off (Current value): " << boolalpha << sound.isOn() << dendl;
			break;

		case RequestType::soundMode:
			verb << "Sound mode (Text): " << SoundMode (information).toString() << " (" << information << ")" << endl;
			trace << "Sound mode (Current value): " << sound.getMode().toString() << " (" << sound.getMode() << ")" << dendl;
			break;

		case RequestType::soundVolume:
			trace << "Sound Volume (Current value): " << sound.getVolume() << dendl;
			break;

		case RequestType::alarmDawnVolume:
			trace << "Dawn Volume (Current value): " << alarms.getDawnVolume() << dendl;
			break;

		case RequestType::alarmDawnTime:
			trace << "Dawn Time (Current value): " << alarms.getDawnTime() << dendl;
			break;

		case RequestType::alarmDawnDuration:
			trace << "Dawn Duration (Current value): " << alarms.getDawnDuration() << dendl;
			break;

		case RequestType::alarmSunsetDuration:
			trace << "Sunset Duration (Current value): " << alarms.getSunsetDuration() << dendl;
			break;

		case RequestType::alarmSunsetDecreaseTime:
			trace << "Sunset Decrease Time (Current value): " << alarms.getSunsetDecreaseTime() << dendl;
			break;
	}
} // displayDebug

RequestError Request::getError (){ return error; }

RequestType Request::getType (){ return type; }

uint8_t Request::getComplement (){ return complement; }

uint32_t Request::getInformation (){ return information; }

String Request::getComplementString ()
{
	if (type.needsComplement())
		return String (complement);

	return "";
}

String Request::getInformationString ()
{
	return utils.ltos (information, type == RequestType::lightModeRgb ? HEX : DEC);
}