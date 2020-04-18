#include <TimeLib.h>
#include <string.h>
#include "Request.h"
#include "Utils.h"
#include "Logger.h"
#include "Light.h"
#include "Alarms.h"
#include "Sound.h"

RequestData Request::decode (String str)
{
	RequestData requestData;

	if (str.length() <= 0)
		requestData.error = badString;
	else if (str.startsWith ("INFO") )
		requestData.type = requestInfos;
	else if (str.startsWith ("TIME"))
		requestData.type = requestTime;
	else
	{
		if ((requestData.type = utils.getMessageTypeFromName (str.substring (0, 3))) != unknown)
			requestData.error = noError;

		// [DEBUG] Printing full word, world length and information type
		trace << "Word: " << str << endl;
		verb << "Length: " << str.length() << endl;
		verb << "Type: " << utils.getMessageTypeDisplayName (requestData.type) << " (" << requestData.type << ")" << endl;

		str = str.substring (3); // Remove 3 first caracteres of the array (The prefix)

		// If the data type needs a complement
		if (utils.messageTypeComplementType (requestData.type) != COMPLEMENT_TYPE_NONE)
		{
			requestData.complement = str.charAt (0) - '0'; // Store the complement
			str                    = str.substring (1);    // Remove first char of the array (the complement)

			if (utils.messageTypeComplementType (requestData.type) == COMPLEMENT_TYPE_LMO)
			{
				verb << "Complement: " << utils.getLightModeName (requestData.complement) << " (" << requestData.complement << ")" << endl;
				if (requestData.complement < LIGHT_MODE_MIN || requestData.complement > LIGHT_MODE_MAX)
					requestData.error = unknownComplement;
			}
			else if (utils.messageTypeComplementType (requestData.type) == COMPLEMENT_TYPE_SCP)
			{
				verb << "Command type: " << utils.getSoundCommandName (requestData.complement) << " (" << requestData.complement << ")" << endl;
				if (requestData.complement < SOUND_COMMAND_MIN || requestData.complement > SOUND_COMMAND_MAX)
					requestData.error = unknownComplement;
			}
		}


		verb << utils.getMessageTypeDisplayName (requestData.type) << ": " << str << endl;

		requestData.information = strtol (str.c_str(), NULL, requestData.type == RGB ? 16 : 10);

		verb << utils.getMessageTypeDisplayName (requestData.type) << " (decoded): " << ((requestData.type == RGB) ? hex : dec) << showbase << uppercase << requestData.information << endl;

		process (requestData);
	}

	return requestData;
} // Request::decode

RequestData Request::decode (String prefix, String complement, String information)
{
	RequestData requestData;

	if (prefix.length() <= 0)
		requestData.error = badString;
	else
	{
		if ((requestData.type = utils.getMessageTypeFromName (prefix)) != unknown)
			requestData.error = noError;

		if (prefix == "INFO")
			requestData.type = requestInfos;
		else
		{
			requestData.complement  = complement.toInt();
			requestData.information = strtol (information.c_str(), NULL, requestData.type == RGB ? 16 : 10);

			process (requestData);
		}
	}

	return requestData;
}

void Request::process (RequestData requestData)
{
	if (requestData.error == noError)
		switch (requestData.type)
		{
			case provideTime:
				if (requestData.information <= 0)
					requestData.error = outOfBound;
				else
					setTime (requestData.information);

				// Debug
				verb << "Time (Current value): " << now() << endl;
				trace << "Time (Current value) (readable): " << utils.getClock() << endl;
				break;

			case RGB:
				if ((uint32_t) requestData.information < LIGHT_MIN_RGB || (uint32_t) requestData.information > LIGHT_MAX_RGB)
					requestData.error = outOfBound;
				else
					light.setRgb (requestData.information, requestData.complement);

				// Debug
				trace << "RGB of " << utils.getLightModeName (requestData.complement) << " (Current value): " << hex << light.getRgb (requestData.complement) << endl;
				verb << "Red       (Current value): " << light.getRed (requestData.complement) << endl;
				verb << "Green     (Current value): " << light.getGreen (requestData.complement) << endl;
				verb << "Blue      (Current value): " << light.getBlue (requestData.complement) << endl;
				trace << np << endl;

				break;

			case LON:
				if (requestData.information != 0 && requestData.information != 1)
					requestData.error = outOfBound;
				else
				{
					if (requestData.information == 1)
						light.switchOn();
					else
						light.switchOff();
				}

				trace << "On/Off (Current value): " << boolalpha << light.isOn() << dendl;

				break;

			case POW:
				if (requestData.information < SEEKBAR_MIN || requestData.information > SEEKBAR_MAX)
					requestData.error = outOfBound;
				else
					light.setPowerPercent (requestData.information, requestData.complement);

				trace << "Power of " << utils.getLightModeName (requestData.complement) << " (Current value): " << light.getPowerRaw (requestData.complement) << " (" << light.getPowerPercent() << "%)" << dendl;

				break;

			case LMO:
				if (requestData.information < LIGHT_MODE_MIN || requestData.information > LIGHT_MODE_MAX)
					requestData.error = outOfBound;
				else
					light.setMode (requestData.information);

				verb << "Light mode (Text): " << utils.getLightModeName (requestData.information) << " (" << requestData.information << ")" << endl;
				trace << "Light mode (Current value): " << utils.getLightModeName (light.getMode()) << " (" << light.getMode() << ")" << dendl;

				break;

			case SPEED:
				if (LIGHT_MIN_SPEED[requestData.complement] == 0 && LIGHT_MAX_SPEED[requestData.complement] == 0)
				{
					if (requestData.information < 0)
						requestData.error = outOfBound;
				}
				else
				{
					if (requestData.information < SEEKBAR_MIN || requestData.information > SEEKBAR_MAX)
						requestData.error = outOfBound;
				}

				if (requestData.error == noError)
					light.setSpeedPercent (requestData.information, requestData.complement);

				// Debug
				verb << "Min Speed: " << LIGHT_MIN_SPEED[requestData.complement] << endl;
				verb << "Max Speed: " << LIGHT_MAX_SPEED[requestData.complement] << endl;
				trace << "Speed of " << utils.getLightModeName (requestData.complement) << " (Current value): " << light.getSpeedRaw (requestData.complement) << " (" << light.getSpeedPercent() << "%)" << dendl;

				break;

			case SMO:
				if (requestData.information < SOUND_MOD_MIN || requestData.information > SOUND_MOD_MAX)
					requestData.error = outOfBound;
				else
					sound.setMode (requestData.information);

				verb << "Sound mode (Text): " << utils.getSoundModeName (requestData.information) << " (" << requestData.information << ")" << endl;
				trace << "Sound mode (Current value): " << utils.getSoundModeName (sound.getMode()) << " (" << sound.getMode() << ")" << dendl;

				break;

			case VOL:
				if (requestData.information < SOUND_VOLUME_MIN || requestData.information > SOUND_VOLUME_MAX)
					requestData.error = outOfBound;
				else
					sound.setVolume (requestData.information);

				trace << "Volume (Current value): " << sound.getVolume() << dendl;

				break;

			case SON:
				if (requestData.information != 0 && requestData.information != 1)
					requestData.error = outOfBound;
				else
				{
					if (requestData.information)
						sound.switchOn();
					else
						sound.switchOff();
				}

				trace << "On/Off (Current value): " << boolalpha << sound.isOn() << dendl;
				break;

			case DTM:
				if (requestData.information < 0 || requestData.information > 1439)
					requestData.error = outOfBound;
				else
				{
					alarms.setDawnTime (requestData.information);
				}

				trace << "Dawn time (Current value): " << alarms.getDawnTime() / 60 << ":" << alarms.getDawnTime() % 60 << " (" << alarms.getDawnTime() << ")" << dendl;
				break;

			case soundCommand:
				verb << "Command data: (" << requestData.information << ")" << dendl;

				#if defined(LUMOS_ARDUINO_MEGA)
				sound.command (requestData.complement, requestData.information);
				#endif

				break;

			default:
				break;
		}

	if (requestData.error != noError)
		warn << "Variable has not been changed (" << utils.getErrorName (requestData.error) << ")" << dendl;
} // Request::process

Request request = Request();