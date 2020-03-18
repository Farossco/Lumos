#include <TimeLib.h>
#include <string.h>
#include "Request.h"
#include "Utils.h"
#include "Logger.h"
#include "Light.h"
#include "Alarms.h"
#include "Sound.h"

#if defined(LUMOS_ARDUINO_MEGA)
# include "ArduinoSerial.h"
# include "VariableChange.h"
#endif

#if defined(LUMOS_ESP8266)
# include "ESPSerial.h"
# include "Json.h"
# include "Wifi.h"
#endif

Req Request::decode (String str)
{
	int16_t strLength = str.length();
	Req requestData;

	if (str.length() <= 0)
	{
		requestData.error = badString;
		return requestData;
	}
	if (str.startsWith ("INFO") )
		requestData.type = requestInfos;
	else if (str.startsWith ("TIME"))
		requestData.type = requestTime;
	else
	{
		// Test correspondance for every type
		for (ReqMes i = MIN; i <= MAX; i++)
			if (str.startsWith (utils.messageTypeName (i, true))) // If there is a match, we store it
			{
				requestData.type  = i;
				requestData.error = none;
				break;
			}

		// [DEBUG] Printing full word, world length and information type
		Log.verbose ("Word: %s" endl, str.c_str());
		Log.verbose ("Length: %d" endl, strLength);
		Log.verbose ("Type: %s (%d)" endl, utils.messageTypeName (requestData.type, false), requestData.type);

		str = str.substring (3); // Remove 3 first caracteres of the array (The prefix)

		// If the data type needs a complement
		if (utils.messageTypeComplementType (requestData.type) != COMPLEMENT_TYPE_NONE)
		{
			requestData.complement = str.charAt (0) - '0'; // Store the complement
			str                    = str.substring (1);    // Remove first char of the array (the complement)

			if (utils.messageTypeComplementType (requestData.type) == COMPLEMENT_TYPE_LMO)
			{
				Log.verbose ("Complement: %s (%d)" endl, utils.lightModName (requestData.complement, CAPS_FIRST), requestData.complement);
				if (requestData.complement < LIGHT_MOD_MIN || requestData.complement > LIGHT_MOD_MAX)
					requestData.error = unknownComplement;
			}
			else if (utils.messageTypeComplementType (requestData.type) == COMPLEMENT_TYPE_SCP)
			{
				Log.verbose ("Command type: %s (%d)" endl, utils.soundCommandName (requestData.complement, CAPS_FIRST), requestData.complement);
				if (requestData.complement < SOUND_COMMAND_MIN || requestData.complement > SOUND_COMMAND_MAX)
					requestData.error = unknownComplement;
			}
		}

		Log.verbose ("%s: %s" endl, utils.messageTypeName (requestData.type, false), str.c_str());

		requestData.information = strtol (str.c_str(), NULL, requestData.type == RGB ? 16 : 10);

		if (requestData.type == RGB)
			Log.verbose ("%s (decoded): %x" endl, utils.messageTypeName (requestData.type, false), requestData.information);
		else
			Log.verbose ("%s (decoded): %l" endl, utils.messageTypeName (requestData.type, false), requestData.information);

		process (requestData);
	}

	return requestData;
} // Request::decode

void Request::process (Req requestData)
{
	if (requestData.error == none)
		switch (requestData.type)
		{
			case TIM:
				if (requestData.information <= 0)
					requestData.error = outOfBound;
				else
					setTime (requestData.information);

				char buf[20];

				// Debug
				Log.verbose ("Time (Current value): %l" endl, now());
				Log.trace ("Time (Current value) (readable): %s" endl, utils.clock (buf));
				break;

			case RGB:
				if ((uint32_t) requestData.information < LIGHT_MIN_RGB || (uint32_t) requestData.information > LIGHT_MAX_RGB)
					requestData.error = outOfBound;
				else
					light.setRgb (requestData.information, requestData.complement);

				// Debug
				Log.trace ("RGB   (Current value): %x" endl, light.getRgb (requestData.complement));
				Log.verbose ("Red   (Current value): %d" endl, light.getRed (requestData.complement));
				Log.verbose ("Green (Current value): %d" endl, light.getGreen (requestData.complement));
				Log.verbose ("Blue  (Current value): %d" dendl, light.getBlue (requestData.complement));

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

				Log.trace ("On/Off (Current value): %T" dendl, light.isOn());

				break;

			case POW:
				if (requestData.information < SEEKBAR_MIN || requestData.information > SEEKBAR_MAX)
					requestData.error = outOfBound;
				else
					light.setPower (utils.map (requestData.information, SEEKBAR_MIN, SEEKBAR_MAX, LIGHT_MIN_POWER, LIGHT_MAX_POWER), requestData.complement);

				Log.trace ("Power of %s (Current value): %d" dendl, utils.lightModName (requestData.complement, CAPS_NONE), light.getPower (requestData.complement));

				break;

			case LMO:
				if (requestData.information < LIGHT_MOD_MIN || requestData.information > LIGHT_MOD_MAX)
					requestData.error = outOfBound;
				else
				{
					light.setMod (requestData.information);
					light.switchOn();
				}

				Log.verbose ("Light mod (Text): %s (%d)" endl, utils.lightModName (requestData.information, CAPS_FIRST), requestData.information);
				Log.trace ("Light mod (Current value): %s (%d)" dendl, utils.lightModName (light.getMod(), CAPS_FIRST), light.getMod());

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

				if (requestData.error == none)
					light.setSpeed (utils.map (requestData.information, SEEKBAR_MIN, SEEKBAR_MAX, LIGHT_MIN_SPEED[requestData.complement], LIGHT_MAX_SPEED[requestData.complement]), requestData.complement);

				// Debug
				Log.verbose ("Min Speed: %d" endl, LIGHT_MIN_SPEED[requestData.complement]);
				Log.verbose ("Max Speed: %d" endl, LIGHT_MAX_SPEED[requestData.complement]);
				Log.trace ("Speed of %s (Current value): %d" dendl, utils.lightModName (requestData.complement, CAPS_NONE), light.getSpeed (requestData.complement));

				break;

			case SMO:
				if (requestData.information < SOUND_MOD_MIN || requestData.information > SOUND_MOD_MAX)
					requestData.error = outOfBound;
				else
					sound.setMod (requestData.information);

				Log.verbose ("Sound mod (Text): %s (%d)" endl, utils.soundModName (requestData.information, CAPS_FIRST), requestData.information);
				Log.trace ("Sound mod (Current value): %s (%d)" dendl, utils.soundModName (sound.getMod(), CAPS_FIRST), sound.getMod());

				break;

			case VOL:
				if (requestData.information < SOUND_VOLUME_MIN || requestData.information > SOUND_VOLUME_MAX)
					requestData.error = outOfBound;
				else
					sound.setVolume (requestData.information);

				Log.trace ("Volume (Current value): %d" dendl, sound.getVolume());

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

				Log.trace ("On/Off (Current value): %T" dendl, sound.isOn());
				break;

			case DTM:
				if (requestData.information < 0 || requestData.information > 1439)
					requestData.error = outOfBound;
				else
				{
					alarms.setDawnTime (requestData.information);
				}

				Log.trace ("Dawn time (Current value): %d:%d (%d)" dendl, alarms.getDawnTime() / 60, alarms.getDawnTime() % 60, alarms.getDawnTime());
				break;

			case SCO:
				Log.verbose ("Command data: (%d)" dendl, requestData.information);

				#if defined(LUMOS_ARDUINO_MEGA)
				sound.command (requestData.complement, requestData.information);
				#endif

				break;

			default:
				break;
		}

	if (requestData.error != none)
	{
		Log.verbosenp (endl);
		Log.warning ("Variable has not been changed (%s)" dendl, utils.errorTypeName (requestData.error, false));
	}
} // Request::process

Request request = Request();
