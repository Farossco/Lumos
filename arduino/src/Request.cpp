#include <Time.h>
#include <string.h>
#include "Request.h"
#include "Utils.h"
#include "Logger.h"
#include "Light.h"
#include "Alarms.h"

void Request::decode (char * request, long & result, int & infoMod, int & infotype, int & errorType)
{
	int requestLength = strlen (request);

	if (requestLength <= 0)
		return;

	result    = 0;
	infotype  = 0;
	errorType = ERR_NOE;
	infoMod   = 0;

	if (strstr (request, "INFO") == request)
	{
		infotype = TYPE_RIF;
		Log.trace ("Received info request" dendl);
		return;
	}
	if (strstr (request, "TIME") == request)
	{
		infotype = TYPE_RTM;

		Log.trace ("Received time request" dendl);

		return;
	}
	else if (strstr (request, "TIM") == request)
		infotype = TYPE_TIM;
	else if (strstr (request, "RGB") == request)
		infotype = TYPE_RGB;
	else if (strstr (request, "ONF") == request)
		infotype = TYPE_ONF;
	else if (strstr (request, "POW") == request)
		infotype = TYPE_POW;
	else if (strstr (request, "MOD") == request)
		infotype = TYPE_MOD;
	else if (strstr (request, "SPE") == request)
		infotype = TYPE_SPE;
	else
	{
		if (SERIAL_LOG_ENABLED || SD_LOG_ENABLED)
		{
			infotype  = TYPE_UNK;
			errorType = ERR_UKR;
		}
		else
			return;
	}

	// [DEBUG] Printing full word, world length and information type
	Log.verbose ("Word: %s" endl, request);
	Log.verbose ("Length: %d" endl, requestLength);
	Log.verbose ("Type: %s (%d)" endl, utils.infoTypeName (infotype, false), infotype);

	utils.reduceCharArray (&request, 3); // Remove 3 first char of the array (The prefix)

	if (infotype == TYPE_POW || infotype == TYPE_SPE)
	{
		infoMod = request[0] - '0';
		Log.verbose ("Info mod: %s (%d)" endl, utils.modName (infoMod, CAPS_FIRST), infoMod);

		utils.reduceCharArray (&request, 1); // Remove first char of the array (mod specifier)

		if (infoMod < MOD_MIN || infoMod > MOD_MAX)
			errorType = ERR_UKM;
	}

	Log.verbose ("%s: %s" endl, utils.infoTypeName (infotype, false), request);

	result = strtol (request, NULL, infotype == TYPE_RGB ? 16 : 10);

	if (infotype == TYPE_RGB)
		Log.verbose ("%s (decoded): %x" endl, utils.infoTypeName (infotype, false), result);
	else
		Log.verbose ("%s (decoded): %l" endl, utils.infoTypeName (infotype, false), result);

	if (errorType == ERR_NOE)
		switch (infotype)
		{
			case TYPE_TIM:
				if (result <= 0)
					errorType = ERR_OOB;
				else
					setTime (result);

				char buf[20];

				// Debug
				Log.verbose ("Time (Current value): %l" endl, now());
				Log.trace ("Time (Current value) (readable): %s" endl, utils.clock (buf));

				alarms.initDawn();

				break;

			case TYPE_RGB:
				if (result < 0 || result > 0xFFFFFF)
					errorType = ERR_OOB;
				else
					light.setRgb (result, MOD_CONTINUOUS);

				// Debug
				Log.trace ("RGB   (Current value): %x" endl, light.getRgb (MOD_CONTINUOUS));
				Log.verbose ("Red   (Current value): %d" endl, light.getRed (MOD_CONTINUOUS));
				Log.verbose ("Green (Current value): %d" endl, light.getGreen (MOD_CONTINUOUS));
				Log.verbose ("Blue  (Current value): %d" endl, light.getBlue (MOD_CONTINUOUS));

				break;

			case TYPE_ONF:
				if (result != 0 && result != 1)
					errorType = ERR_OOB;
				else
				{
					if (result)
						light.switchOn();
					else
						light.switchOff();
				}

				Log.trace ("On/Off (Current value): %T" dendl, light.isOn());

				break;

			case TYPE_POW:
				if (result < SEEKBAR_MIN || result > SEEKBAR_MAX)
					errorType = ERR_OOB;
				else
					light.setPower (utils.map (result, SEEKBAR_MIN, SEEKBAR_MAX, MIN_POWER, MAX_POWER), infoMod);

				Log.trace ("Power of %s (Current value): %d" dendl, utils.modName (infoMod, CAPS_NONE), light.getPower (infoMod));

				break;

			case TYPE_MOD:
				if (result < MOD_MIN || result > MOD_MAX)
					errorType = ERR_OOB;
				else
					light.setMod (result);

				Log.verbose ("Mod (Text): %s" endl, utils.modName (result, CAPS_FIRST));
				Log.trace ("Mod (Current value): %s (%d)" dendl, utils.modName (light.getMod(), CAPS_FIRST), light.getMod());

				break;

			case TYPE_SPE:
				if (infoMod == MOD_DAWN)
				{
					if (result < 0)
						errorType = ERR_OOB;
					else
					{
						light.setSpeed (result, infoMod);
						alarms.initDawn();
					}
				}
				else
				{
					if (result < SEEKBAR_MIN || result > SEEKBAR_MAX)
						errorType = ERR_OOB;
					else
						light.setSpeed (utils.map (result, SEEKBAR_MIN, SEEKBAR_MAX, MIN_SPEED[infoMod], MAX_SPEED[infoMod]), infoMod);
				}

				// Debug
				Log.verbose ("Min Speed: %d" endl, MIN_SPEED[infoMod]);
				Log.verbose ("Max Speed: %d" endl, MAX_SPEED[infoMod]);
				Log.trace ("Speed of %s (Current value): %d" dendl, utils.modName (infoMod, CAPS_NONE), light.getSpeed (infoMod));

				break;
		}

	if (errorType != ERR_NOE)
	{
		Log.verbosenp (endl);
		Log.warning ("Variable has not been changed (%s)" dendl, utils.errorTypeName (errorType, false));
	}
} // Request::decode

Request request = Request();
