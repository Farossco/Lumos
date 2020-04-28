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
	if (inputString.length() == 0)
		error = RequestError::emptyString;
	else
	{
		error = RequestError::none;

		if (inputString.startsWith ("INFO") )
			type = RequestType::requestInfos;
		else if (inputString.startsWith ("TIME"))
			type = RequestType::requestTime;
		else
		{
			type = utils.getMessageTypeFromName (inputString.substring (0, 3));

			// [DEBUG] Printing full word, world length and information type
			trace << "Word: " << inputString << endl;
			verb << "Length: " << inputString.length() << endl;
			verb << "Type: " << type.toString() << " (" << type << ")" << endl;

			if (type == RequestType::unknown)
				error = RequestError::unknownType;
			else
			{
				inputString = inputString.substring (3); // Remove 3 first caracteres of the array (The prefix)

				// If the data type needs a complement
				if (type.getComplementType() != ComplementCategory::none)
				{
					Bounds bounds = type.getComplementBounds();

					complement  = inputString.charAt (0) - '0'; // Store the complement
					inputString = inputString.substring (1);    // Remove first char of the array (the complement)

					if (type.getComplementType() == ComplementCategory::lightMode)
						verb << "Light mode: " << LightMode (complement).toString() << " (" << complement << ")" << endl;
					else if (type.getComplementType() == ComplementCategory::soundCommand)
						verb << "Command type: " << SoundCommand (complement).toString() << " (" << complement << ")" << endl;

					if (complement < bounds.low || complement > bounds.high)
						error = RequestError::unknownComplement;
				}

				information = strtol (inputString.c_str(), NULL, type == RequestType::lightRgb ? 16 : 10);

				verb << type.toString() << ": " << inputString << endl;
				verb << type.toString() << " (decoded): " << ((type == RequestType::lightRgb) ? hex : dec) << showbase << uppercase << information << endl;
			}
			if (error)
				trace << np << endl;
		}
	}
}

Request::Request(const String & prefixString, const String & complementString, const String & informationString)
{
	if (prefixString.length() <= 0)
		error = RequestError::emptyString;
	else
	{
		if ((type = utils.getMessageTypeFromName (prefixString)) != RequestType::unknown)
			error = RequestError::none;

		if (prefixString == "INFO")
			type = RequestType::requestInfos;
		else
		{
			complement  = complementString.toInt();
			information = strtol (informationString.c_str(), NULL, type == RequestType::lightRgb ? 16 : 10);
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
		Bounds bounds = type.getInformationBounds();

		if (information < bounds.low || information > bounds.high)
			error = RequestError::outOfBound;
		else
			switch (type)
			{
				case RequestType::provideTime:
					setTime (information);
					break;

				case RequestType::lightRgb:
					light.setRgb (information, complement);
					break;

				case RequestType::lightOnOff:
					(information) ? light.switchOn() : light.switchOff();
					break;

				case RequestType::lightPower:
					light.setPowerPercent (information, complement);
					break;

				case RequestType::lightMode:
					light.setMode (information);
					break;

				case RequestType::lightModeSpeed:
					light.setSpeedPercent (information, complement);
					break;

				case RequestType::soundMode:
					sound.setMode (information);
					break;

				case RequestType::soundVolume:
					sound.setVolume (information);
					break;

				case RequestType::soundOnOff:
					(information) ? sound.switchOn() : sound.switchOff();
					break;

				case RequestType::dawnTime:
					alarms.setDawnTime (information);
					break;

				case RequestType::soundCommand:
					#if defined(LUMOS_ARDUINO_MEGA)
					sound.command (complement, information);
					#endif
					break;
			}

		displayDebug();
	}
	else if (error != RequestError::emptyString)
		warn << "Variable has not been changed (" << error.toString() << ")" << dendl;
} // Request::process

void Request::displayDebug ()
{
	switch (type)
	{
		case RequestType::provideTime:
			verb << "Time (Current value): " << now() << endl;
			trace << "Time (Current value) (readable): " << utils.getClock() << dendl;
			break;

		case RequestType::lightRgb:
			trace << "RGB of " << LightMode (complement).toString() << " (Current value): " << hex << light.getRgb (complement) << endl;
			verb << "Red       (Current value): " << light.getRed (complement) << endl;
			verb << "Green     (Current value): " << light.getGreen (complement) << endl;
			verb << "Blue      (Current value): " << light.getBlue (complement) << endl;
			trace << np << endl;
			break;

		case RequestType::lightOnOff:
			trace << "Light On/Off (Current value): " << boolalpha << light.isOn() << dendl;
			break;

		case RequestType::lightPower:
			trace << "Power of " << LightMode (complement).toString() << " (Current value): " << light.getPowerRaw (complement) << " (" << light.getPowerPercent() << "%)" << dendl;
			break;

		case RequestType::lightMode:
			verb << "Light mode (Text): " << LightMode (information).toString() << " (" << information << ")" << endl;
			trace << "Light mode (Current value): " << light.getMode().toString() << " (" << light.getMode() << ")" << dendl;
			break;

		case RequestType::lightModeSpeed:
			verb << "Min Speed: " << LightSetting::MIN_SPEED << endl;
			verb << "Max Speed: " << LightSetting::MAX_SPEED << endl;
			trace << "Speed of " << LightMode (complement).toString() << " (Current value): " << light.getSpeedRaw (complement) << " (" << light.getSpeedPercent() << "%)" << dendl;
			break;

		case RequestType::soundMode:
			verb << "Sound mode (Text): " << SoundMode (information).toString() << " (" << information << ")" << endl;
			trace << "Sound mode (Current value): " << sound.getMode().toString() << " (" << sound.getMode() << ")" << dendl;
			break;

		case RequestType::soundVolume:
			trace << "Volume (Current value): " << sound.getVolume() << dendl;
			break;

		case RequestType::soundOnOff:
			trace << "On/Off (Current value): " << boolalpha << sound.isOn() << dendl;
			break;

		case RequestType::dawnTime:
			trace << "Dawn time (Current value): " << alarms.getDawnTime() / 60 << ":" << alarms.getDawnTime() % 60 << " (" << alarms.getDawnTime() << ")" << dendl;
			break;

		case RequestType::soundCommand:
			verb << "Command data: (" << information << ")" << dendl;
			break;
	}
} // displayDebug

RequestError Request::getError (){ return error; }

RequestType Request::getType (){ return type; }

uint8_t Request::getComplement (){ return complement; }

uint32_t Request::getInformation (){ return information; }

String Request::getComplementString ()
{
	if (type.getComplementType() != ComplementCategory::none)
		return String (complement);

	return "";
}

String Request::getInformationString ()
{
	return utils.ltos (information, type == RequestType::lightRgb ? HEX : DEC);
}