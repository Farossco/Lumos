#include <TimeLib.h>
#include <string.h>
#include "Request.h"
#include "Utils.h"
#include "Logger.h"
#include "Light.h"
// #include "Alarms.h"
#include "Sound.h"

void Request::decode (char * request, uint8_t & type, uint8_t & complement, int32_t & information, int8_t & error)
{
	int16_t requestLength = strlen (request);

	if (requestLength <= 0)
		return;

	information = 0;
	complement  = 0;

	if (strstr (request, "INFO") == request)
	{
		type = TYPE_RIF;
		Log.trace ("Received info request" dendl);
		return;
	}
	if (strstr (request, "TIME") == request)
	{
		type = TYPE_RTM;

		Log.trace ("Received time request" dendl);

		return;
	}

	type  = TYPE_UNK;
	error = ERR_UKR;

	for (uint8_t i = TYPE_SEND_MIN; i <= TYPE_MAX; i++)
		if (strstr (request, utils.infoTypeName (i, true)) == request)
		{
			type  = i;
			error = ERR_NOE;
			break;
		}

	// [DEBUG] Printing full word, world length and information type
	Log.verbose ("Word: %s" endl, request);
	Log.verbose ("Length: %d" endl, requestLength);
	Log.verbose ("Type: %s (%d)" endl, utils.infoTypeName (type, false), type);

	utils.reduceCharArray (&request, 3); // Remove 3 first char of the array (The prefix)


	if (type == TYPE_POW || type == TYPE_SPE || type == TYPE_SCO)
	{
		complement = request[0] - '0';
		utils.reduceCharArray (&request, 1); // Remove first char of the array (mod specifier)

		if (type == TYPE_POW || type == TYPE_SPE)
		{
			Log.verbose ("Mod: %s (%d)" endl, utils.lightModName (complement, CAPS_FIRST), complement);
			if (complement < LIGHT_MOD_MIN || complement > LIGHT_MOD_MAX)
				error = ERR_UKC;
		}
		else if (type == TYPE_SCO)
		{
			Log.verbose ("Command type: %s (%d)" endl, utils.soundCommandName (complement, CAPS_FIRST), complement);
			if (complement < SOUND_COMMAND_MIN || complement > SOUND_COMMAND_MAX)
				error = ERR_UKC;
		}
	}


	Log.verbose ("%s: %s" endl, utils.infoTypeName (type, false), request);

	information = strtol (request, NULL, type == TYPE_RGB ? 16 : 10);

	if (type == TYPE_RGB)
		Log.verbose ("%s (decoded): %x" endl, utils.infoTypeName (type, false), information);
	else
		Log.verbose ("%s (decoded): %l" endl, utils.infoTypeName (type, false), information);

	if (error == ERR_NOE)
		switch (type)
		{
			case TYPE_TIM:
				if (information <= 0)
					error = ERR_OOB;
				else
					setTime (information);

				char buf[20];

				// Debug
				Log.verbose ("Time (Current value): %l" endl, now());
				Log.trace ("Time (Current value) (readable): %s" endl, utils.clock (buf));

				// alarms.initDawn();

				break;

			case TYPE_RGB:
				if (information < 0 || information > 0xFFFFFF)
					error = ERR_OOB;
				else
					light.setRgb (information, LIGHT_MOD_CONTINUOUS);

				// Debug
				Log.trace ("RGB   (Current value): %x" endl, light.getRgb (LIGHT_MOD_CONTINUOUS));
				Log.verbose ("Red   (Current value): %d" endl, light.getRed (LIGHT_MOD_CONTINUOUS));
				Log.verbose ("Green (Current value): %d" endl, light.getGreen (LIGHT_MOD_CONTINUOUS));
				Log.verbose ("Blue  (Current value): %d" endl, light.getBlue (LIGHT_MOD_CONTINUOUS));

				break;

			case TYPE_ONF:
				if (information != 0 && information != 1)
					error = ERR_OOB;
				else
				{
					if (information)
						light.switchOn();
					else
						light.switchOff();
				}

				Log.trace ("On/Off (Current value): %T" dendl, light.isOn());

				break;

			case TYPE_POW:
				if (information < SEEKBAR_MIN || information > SEEKBAR_MAX)
					error = ERR_OOB;
				else
					light.setPower (utils.map (information, SEEKBAR_MIN, SEEKBAR_MAX, LIGHT_MIN_POWER, LIGHT_MAX_POWER), complement);

				Log.trace ("Power of %s (Current value): %d" dendl, utils.lightModName (complement, CAPS_NONE), light.getPower (complement));

				break;

			case TYPE_LMO:
				if (information < LIGHT_MOD_MIN || information > LIGHT_MOD_MAX)
					error = ERR_OOB;
				else
					light.setMod (information);

				Log.verbose ("Light mod (Text): %s (%d)" endl, utils.lightModName (information, CAPS_FIRST), information);
				Log.trace ("Light mod (Current value): %s (%d)" dendl, utils.lightModName (light.getMod(), CAPS_FIRST), light.getMod());

				break;

			case TYPE_SPE:
				if (complement == LIGHT_MOD_DAWN)
				{
					if (information < 0)
						error = ERR_OOB;
					else
					{
						light.setSpeed (information, complement);
						// alarms.initDawn();
					}
				}
				else
				{
					if (information < SEEKBAR_MIN || information > SEEKBAR_MAX)
						error = ERR_OOB;
					else
						light.setSpeed (utils.map (information, SEEKBAR_MIN, SEEKBAR_MAX, LIGHT_MIN_SPEED[complement], LIGHT_MAX_SPEED[complement]), complement);
				}

				// Debug
				Log.verbose ("Min Speed: %d" endl, LIGHT_MIN_SPEED[complement]);
				Log.verbose ("Max Speed: %d" endl, LIGHT_MAX_SPEED[complement]);
				Log.trace ("Speed of %s (Current value): %d" dendl, utils.lightModName (complement, CAPS_NONE), light.getSpeed (complement));

				break;

			case TYPE_SMO:
				if (information < SOUND_MOD_MIN || information > SOUND_MOD_MAX)
					error = ERR_OOB;
				else
					sound.setMod (information);

				Log.verbose ("Sound mod (Text): %s (%d)" endl, utils.soundModName (information, CAPS_FIRST), information);
				Log.trace ("Sound mod (Current value): %s (%d)" dendl, utils.soundModName (sound.getMod(), CAPS_FIRST), sound.getMod());

				break;

			case TYPE_VOL:
				if (information < SOUND_VOLUME_MIN || information > SOUND_VOLUME_MAX)
					error = ERR_OOB;
				else
					sound.setVolume (information);

				Log.trace ("Volume (Current value): %d" dendl, sound.getVolume());

				break;

			case TYPE_SON:
				if (information != 0 && information != 1)
					error = ERR_OOB;
				else
				{
					if (information)
						sound.switchOn();
					else
						sound.switchOff();
				}

				Log.trace ("On/Off (Current value): %T" dendl, sound.isOn());
				break;
		}

	if (error != ERR_NOE)
		Log.warning ("Variable has not been changed (%s)" dendl, utils.errorTypeName (error, false));
} // Request::decode

Request request = Request();
