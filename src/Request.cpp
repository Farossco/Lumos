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
	if (inputString.length() <= 0)
		error = badString;
	else
	{
		error = noError;

		if (inputString.startsWith ("INFO") )
			type = requestInfos;
		else if (inputString.startsWith ("TIME"))
			type = requestTime;
		else
		{
			type = utils.getMessageTypeFromName (inputString.substring (0, 3));

			// [DEBUG] Printing full word, world length and information type
			trace << "Word: " << inputString << endl;
			verb << "Length: " << inputString.length() << endl;
			verb << "Type: " << utils.getMessageTypeDisplayName (type) << " (" << type << ")" << endl;

			if (type == unknown)
				error = unknowmType;
			else
			{
				inputString = inputString.substring (3); // Remove 3 first caracteres of the array (The prefix)

				// If the data type needs a complement
				if (getComplementType() != complementNone)
				{
					Bounds bounds = utils.getComplementBounds (type);

					complement  = inputString.charAt (0) - '0'; // Store the complement
					inputString = inputString.substring (1);    // Remove first char of the array (the complement)

					if (getComplementType() == complementLightMode)
						verb << "Light mode: " << utils.getLightModeName (complement) << " (" << complement << ")" << endl;
					else if (getComplementType() == complementSoundCommand)
						verb << "Command type: " << utils.getSoundCommandName (complement) << " (" << complement << ")" << endl;

					if (complement < bounds.low || complement > bounds.high)
						error = unknownComplement;
				}

				information = strtol (inputString.c_str(), NULL, type == lightRgb ? 16 : 10);

				verb << utils.getMessageTypeDisplayName (type) << ": " << inputString << endl;
				verb << utils.getMessageTypeDisplayName (type) << " (decoded): " << ((type == lightRgb) ? hex : dec) << showbase << uppercase << information << endl;
			}

			if (error != noError)
				trace << np << endl;
		}
	}
}

Request::Request(String prefixString, String complementString, String informationString)
{
	if (prefixString.length() <= 0)
		error = badString;
	else
	{
		if ((type = utils.getMessageTypeFromName (prefixString)) != unknown)
			error = noError;

		if (prefixString == "INFO")
			type = requestInfos;
		else
		{
			complement  = complementString.toInt();
			information = strtol (informationString.c_str(), NULL, type == lightRgb ? 16 : 10);
		}
	}
}

void Request::decodeInput ()
{ } // Request::decode

void Request::decodeSeparate ()
{ }

void Request::process ()
{
	if (error == noError)
	{
		switch (type)
		{
			case provideTime:
				if (information <= 0)
					error = outOfBound;
				else
					setTime (information);

				verb << "Time (Current value): " << now() << endl;
				trace << "Time (Current value) (readable): " << utils.getClock() << endl;
				verb << endl;
				break;

			case lightRgb:
				if ((uint32_t) information < LIGHT_MIN_RGB || (uint32_t) information > LIGHT_MAX_RGB)
					error = outOfBound;
				else
					light.setRgb (information, complement);

				trace << "RGB of " << utils.getLightModeName (complement) << " (Current value): " << hex << light.getRgb (complement) << endl;
				verb << "Red       (Current value): " << light.getRed (complement) << endl;
				verb << "Green     (Current value): " << light.getGreen (complement) << endl;
				verb << "Blue      (Current value): " << light.getBlue (complement) << endl;
				trace << np << endl;

				break;

			case lightOnOff:
				if (information != 0 && information != 1)
					error = outOfBound;
				else
				{
					if (information == 1)
						light.switchOn();
					else
						light.switchOff();
				}

				trace << "Light On/Off (Current value): " << boolalpha << light.isOn() << dendl;

				break;

			case lightPower:
				if (information < SEEKBAR_MIN || information > SEEKBAR_MAX)
					error = outOfBound;
				else
					light.setPowerPercent (information, complement);

				trace << "Power of " << utils.getLightModeName (complement) << " (Current value): " << light.getPowerRaw (complement) << " (" << light.getPowerPercent() << "%)" << dendl;

				break;

			case lightMode:
				if (information < LIGHT_MODE_MIN || information > LIGHT_MODE_MAX)
					error = outOfBound;
				else
					light.setMode (information);

				verb << "Light mode (Text): " << utils.getLightModeName (information) << " (" << information << ")" << endl;
				trace << "Light mode (Current value): " << utils.getLightModeName (light.getMode()) << " (" << light.getMode() << ")" << dendl;

				break;

			case lightModeSpeed:
				if (LIGHT_MIN_SPEED[complement] == 0 && LIGHT_MAX_SPEED[complement] == 0)
				{
					if (information < 0)
						error = outOfBound;
				}
				else
				{
					if (information < SEEKBAR_MIN || information > SEEKBAR_MAX)
						error = outOfBound;
				}

				if (error == noError)
					light.setSpeedPercent (information, complement);

				// Debug
				verb << "Min Speed: " << LIGHT_MIN_SPEED[complement] << endl;
				verb << "Max Speed: " << LIGHT_MAX_SPEED[complement] << endl;
				trace << "Speed of " << utils.getLightModeName (complement) << " (Current value): " << light.getSpeedRaw (complement) << " (" << light.getSpeedPercent() << "%)" << dendl;

				break;

			case soundMode:
				if (information < SOUND_MOD_MIN || information > SOUND_MOD_MAX)
					error = outOfBound;
				else
					sound.setMode (information);

				verb << "Sound mode (Text): " << utils.getSoundModeName (information) << " (" << information << ")" << endl;
				trace << "Sound mode (Current value): " << utils.getSoundModeName (sound.getMode()) << " (" << sound.getMode() << ")" << dendl;

				break;

			case soundVolume:
				if (information < SOUND_VOLUME_MIN || information > SOUND_VOLUME_MAX)
					error = outOfBound;
				else
					sound.setVolume (information);

				trace << "Volume (Current value): " << sound.getVolume() << dendl;

				break;

			case soundOnOff:
				if (information != 0 && information != 1)
					error = outOfBound;
				else
				{
					if (information)
						sound.switchOn();
					else
						sound.switchOff();
				}

				trace << "On/Off (Current value): " << boolalpha << sound.isOn() << dendl;
				break;

			case dawnTime:
				if (information < 0 || information > 1439)
					error = outOfBound;
				else
				{
					alarms.setDawnTime (information);
				}

				trace << "Dawn time (Current value): " << alarms.getDawnTime() / 60 << ":" << alarms.getDawnTime() % 60 << " (" << alarms.getDawnTime() << ")" << dendl;
				break;

			case soundCommand:
				verb << "Command data: (" << information << ")" << dendl;

				#if defined(LUMOS_ARDUINO_MEGA)
				sound.command (complement, information);
				#endif

				break;

			default:
				break;
		}
	}
	else if (type > unknown)
	{
		warn << "Variable has not been changed (" << utils.getErrorName (error) << ")" << dendl;
	}
} // Request::process

RequestError Request::getError (){ return error; }

RequestType Request::getType (){ return type; }

uint8_t Request::getComplement (){ return complement; }

int32_t Request::getInformation (){ return information; }

const char * Request::getTypeString ()
{
	return utils.getMessageTypeName (type);
}

String Request::getComplementString ()
{
	if (getComplementType() != complementNone)
		return String (complement);

	return "";
}

String Request::getInformationString ()
{
	return utils.ltos (information, type == lightRgb ? HEX : DEC);
}

ComplementType Request::getComplementType ()
{
	switch (type)
	{
		case lightRgb:
		case lightPower:
		case lightModeSpeed:
			return complementLightMode;

		case soundCommand:
			return complementSoundCommand;

		default:
			return complementNone;
	}
}