#include <TimeLib.h>
#include <string.h>
#include "Request.h"
#include "Utils.h"
#include "Logger.h"
#include "Light.h"
#include "Alarms.h"
#include "Sound.h"

Request::Request(String inputString)
{
	if (inputString.length() <= 0)
		error = badString;
	else if (inputString.startsWith ("INFO") )
		type = requestInfos;
	else if (inputString.startsWith ("TIME"))
		type = requestTime;
	else
	{
		if ((type = utils.getMessageTypeFromName (inputString.substring (0, 3))) != unknown)
			error = noError;

		// [DEBUG] Printing full word, world length and information type
		trace << "Word: " << inputString << endl;
		verb << "Length: " << inputString.length() << endl;
		verb << "Type: " << utils.getMessageTypeDisplayName (type) << " (" << type << ")" << endl;

		inputString = inputString.substring (3); // Remove 3 first caracteres of the array (The prefix)

		// If the data type needs a complement
		if (utils.messageTypeComplementType (type) != COMPLEMENT_TYPE_NONE)
		{
			complement = inputString.charAt (0) - '0'; // Store the complement
			inputString      = inputString.substring (1);    // Remove first char of the array (the complement)

			if (utils.messageTypeComplementType (type) == COMPLEMENT_TYPE_LMO)
			{
				verb << "Complement: " << utils.getLightModeName (complement) << " (" << complement << ")" << endl;
				if (complement < LIGHT_MODE_MIN || complement > LIGHT_MODE_MAX)
					error = unknownComplement;
			}
			else if (utils.messageTypeComplementType (type) == COMPLEMENT_TYPE_SCP)
			{
				verb << "Command type: " << utils.getSoundCommandName (complement) << " (" << complement << ")" << endl;
				if (complement < SOUND_COMMAND_MIN || complement > SOUND_COMMAND_MAX)
					error = unknownComplement;
			}
		}

		verb << utils.getMessageTypeDisplayName (type) << ": " << inputString << endl;

		information = strtol (inputString.c_str(), NULL, type == RGB ? 16 : 10);

		verb << utils.getMessageTypeDisplayName (type) << " (decoded): " << ((type == RGB) ? hex : dec) << showbase << uppercase << information << endl;

		if (error != noError)
			trace << np << endl;
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
			information = strtol (informationString.c_str(), NULL, type == RGB ? 16 : 10);
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

				// Debug
				verb << "Time (Current value): " << now() << endl;
				trace << "Time (Current value) (readable): " << utils.getClock() << endl;
				break;

			case RGB:
				if ((uint32_t) information < LIGHT_MIN_RGB || (uint32_t) information > LIGHT_MAX_RGB)
					error = outOfBound;
				else
					light.setRgb (information, complement);

				// Debug
				trace << "RGB of " << utils.getLightModeName (complement) << " (Current value): " << hex << light.getRgb (complement) << endl;
				verb << "Red       (Current value): " << light.getRed (complement) << endl;
				verb << "Green     (Current value): " << light.getGreen (complement) << endl;
				verb << "Blue      (Current value): " << light.getBlue (complement) << endl;
				trace << np << endl;

				break;

			case LON:
				if (information != 0 && information != 1)
					error = outOfBound;
				else
				{
					if (information == 1)
						light.switchOn();
					else
						light.switchOff();
				}

				trace << "On/Off (Current value): " << boolalpha << light.isOn() << dendl;

				break;

			case POW:
				if (information < SEEKBAR_MIN || information > SEEKBAR_MAX)
					error = outOfBound;
				else
					light.setPowerPercent (information, complement);

				trace << "Power of " << utils.getLightModeName (complement) << " (Current value): " << light.getPowerRaw (complement) << " (" << light.getPowerPercent() << "%)" << dendl;

				break;

			case LMO:
				if (information < LIGHT_MODE_MIN || information > LIGHT_MODE_MAX)
					error = outOfBound;
				else
					light.setMode (information);

				verb << "Light mode (Text): " << utils.getLightModeName (information) << " (" << information << ")" << endl;
				trace << "Light mode (Current value): " << utils.getLightModeName (light.getMode()) << " (" << light.getMode() << ")" << dendl;

				break;

			case SPEED:
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

			case SMO:
				if (information < SOUND_MOD_MIN || information > SOUND_MOD_MAX)
					error = outOfBound;
				else
					sound.setMode (information);

				verb << "Sound mode (Text): " << utils.getSoundModeName (information) << " (" << information << ")" << endl;
				trace << "Sound mode (Current value): " << utils.getSoundModeName (sound.getMode()) << " (" << sound.getMode() << ")" << dendl;

				break;

			case VOL:
				if (information < SOUND_VOLUME_MIN || information > SOUND_VOLUME_MAX)
					error = outOfBound;
				else
					sound.setVolume (information);

				trace << "Volume (Current value): " << sound.getVolume() << dendl;

				break;

			case SON:
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

			case DTM:
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