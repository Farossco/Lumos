#include <Time.h>
#include <string.h>
#include "Request.h"
#include "Utils.h"
#include "Logger.h"
#include "Global.h"
#include "Alarms.h"

void Request::decode (char * request, long & result, int & infoMode, int & infotype, int & errorType)
{
	int requestLength = strlen (request);

	result    = 0;
	infotype  = 0;
	errorType = ERR_NOE;
	infoMode  = 0;

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
	else if (strstr (request, "PRT") == request)
		infotype = TYPE_PRT;
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
		infoMode = request[0] - '0';
		Log.verbose ("Info mode: %s (%d)" endl, utils.modeName (infoMode, CAPS_FIRST), infoMode);

		utils.reduceCharArray (&request, 1); // Remove first char of the array (mod specifier)

		if (infoMode < MODE_MIN || infoMode > MODE_MAX)
			errorType = ERR_UKM;
	}
	else if (infotype == TYPE_PRT)
	{
		for (infoMode = 0; infoMode < N_PRAYER; (infoMode)++)
			if (request[0] == PRAYERS_NAME[infoMode][0])
				break;

		Log.verbose ("Prayer: ");
		if (infoMode < 0 || infoMode >= N_PRAYER)
		{
			errorType = ERR_UKP;
			Log.verbosenp ("%d" endl, infoMode);
		}
		else
		{
			Log.verbosenp ("%s (%d)" endl, PRAYERS_NAME[infoMode], infoMode);
		}

		utils.reduceCharArray (&request, 1); // Remove first char of the array (prayer specifier)
	}

	Log.verbose ("%s: %s" endl, utils.infoTypeName (infotype, false), request);

	result = strtol (request, NULL, infotype == TYPE_RGB ? 16 : 10);

	if (infotype == TYPE_RGB)
		Log.verbose ("%s (decoded): %x" endl, utils.infoTypeName (infotype, false), result);
	else
		Log.verbose ("%s (decoded): %d" endl, utils.infoTypeName (infotype, false), result);

	if (errorType == ERR_NOE)
		switch (infotype)
		{
			case TYPE_TIM:
				if (result < 0)
					errorType = ERR_OOB;
				else
					setTime (result);

				char buf[20];

				// Debug
				Log.verbose ("Time (Current value): %d" endl, now());
				Log.verbose ("Time (Current value) (readable): %s" endl, utils.clock (buf));

				alarms.initDawn();
				break;

			case TYPE_RGB:
				if (result < 0 || result > 0xFFFFFF)
					errorType = ERR_OOB;
				else
					global.rgb[MODE_DEFAULT] = result;

				// Debug
				global.rgb2color();
				Log.verbose ("RGB   (Current value): %x" endl, global.rgb[MODE_DEFAULT]);
				Log.verbose ("Red   (Current value): %d" endl, global.red[MODE_DEFAULT]);
				Log.verbose ("Green (Current value): %d" endl, global.green[MODE_DEFAULT]);
				Log.verbose ("Blue  (Current value): %d" endl, global.blue[MODE_DEFAULT]);

				break;

			case TYPE_ONF:
				if (result != 0 && result != 1)
					errorType = ERR_OOB;
				else
					global.on = result;

				Log.verbose ("On/Off (Current value): ");

				if (global.on != 0 && global.on != 1)
				{
					Log.verbosenp ("Error (%d)" dendl, global.on);
				}
				else
				{
					Log.verbosenp ("%T" dendl, global.on);
				}

				break;

			case TYPE_POW:
				if (result < SEEKBAR_MIN || result > SEEKBAR_MAX)
					errorType = ERR_OOB;
				else
					global.power[infoMode] = utils.convertBoundaries (result, SEEKBAR_MIN, SEEKBAR_MAX, MIN_POWER, MAX_POWER);

				Log.verbose ("Power (Current value): %d" dendl, global.power[infoMode]);

				break;

			case TYPE_MOD:
				if (result < MODE_MIN || result > MODE_MAX)
					errorType = ERR_OOB;
				else
					global.mode = result;

				Log.verbose ("Mode (Text): %s" endl, utils.modeName (result, CAPS_FIRST));
				Log.verbose ("Mode (Current value): %s (%d)" dendl, global.power[infoMode], utils.modeName (global.mode, CAPS_FIRST), global.mode);

				break;

			case TYPE_PRT:
				if (result < MODE_MIN && result > MODE_MAX)
					errorType = ERR_OOB;
				else
				{
					alarms.prayerTime[infoMode][2] = result;
					alarms.prayerTime[infoMode][0] = alarms.prayerTime[infoMode][2] / 60;
					alarms.prayerTime[infoMode][1] = alarms.prayerTime[infoMode][2] % 60;

					alarms.prayersSet[infoMode] = true;
				}

				// Debug
				Log.verbose ("Prayer time (Current value): %d" endl, alarms.prayerTime[infoMode][2]);
				Log.verbose ("Prayer time (Current value) (Readable): %d:%d" dendl, alarms.prayerTime[infoMode][0], alarms.prayerTime[infoMode][1]);

				alarms.initPrayer();

				break;


			case TYPE_SPE:
				if (infoMode == MODE_DAWN)
				{
					if (result < 0)
						errorType = ERR_OOB;
					else
					{
						global.speed[infoMode] = result;
						alarms.initDawn();
					}
				}
				else
				{
					if (result < SEEKBAR_MIN || result > SEEKBAR_MAX)
						errorType = ERR_OOB;
					else
						global.speed[infoMode] = utils.convertBoundaries (result, SEEKBAR_MIN, SEEKBAR_MAX, MIN_SPEED[infoMode], MAX_SPEED[infoMode]);
				}

				// Debug
				Log.verbose ("Min Speed: %d" endl, MIN_SPEED[infoMode]);
				Log.verbose ("Max Speed: %d" endl, MAX_SPEED[infoMode]);
				Log.verbose ("Speed (Current value): %d" dendl, global.speed[infoMode]);

				break;
		}

	if (errorType != ERR_NOE)
	{
		Log.verbosenp (endl);
		Log.warning ("Variable has not been changed (%s)" dendl, utils.errorTypeName (errorType, false));
	}
} // Request::decode

Request request = Request();
