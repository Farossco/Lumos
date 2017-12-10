#include "arduino.h"

void decodeRequest (String request, long & result, int & infoMode, int & infotype, int & errorType)
{
	char requestChar[20];
	int requestLength = request.length();

	result    = 0;
	infotype  = 0;
	errorType = ERR_NOE;
	infoMode  = 0;

	if (request.indexOf ("INFO") == 0)
	{
		infotype = TYPE_RIF;

		println (LEVEL_DEBUG, "Received info request");
		return;
	}
	if (request.indexOf ("TIME") == 0)
	{
		infotype = TYPE_RTM;

		println (LEVEL_DEBUG, "Received time request");
		return;
	}
	else if (request.indexOf ("TIM") == 0)
		infotype = TYPE_TIM;
	else if (request.indexOf ("RGB") == 0)
		infotype = TYPE_RGB;
	else if (request.indexOf ("ONF") == 0)
		infotype = TYPE_ONF;
	else if (request.indexOf ("POW") == 0)
		infotype = TYPE_POW;
	else if (request.indexOf ("MOD") == 0)
		infotype = TYPE_MOD;
	else if (request.indexOf ("PRT") == 0)
		infotype = TYPE_PRT;
	else if (request.indexOf ("SPE") == 0)
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
	println (LEVEL_DEBUG, false);
	print (LEVEL_DEBUG, "Word: ");
	println (LEVEL_DEBUG, request, false);
	print (LEVEL_DEBUG, "Length: ");
	println (LEVEL_DEBUG, requestLength, DEC, false);
	print (LEVEL_DEBUG, "Type: ");
	print (LEVEL_DEBUG, infoTypeName (infotype, false) + " (", false);
	print (LEVEL_DEBUG, infotype, DEC, false);
	println (LEVEL_DEBUG, ")", false);

	request.remove (0, 3);

	if (infotype == TYPE_POW || infotype == TYPE_SPE)
	{
		infoMode = request.charAt (0) - '0';
		print (LEVEL_DEBUG, "Info mode: ");
		print (LEVEL_DEBUG, modeName (infoMode, CAPS_FIRST), false);
		print (LEVEL_DEBUG, " (", false);
		print (LEVEL_DEBUG, infoMode, DEC, false);
		println (LEVEL_DEBUG, ")", false);
		request.remove (0, 1);
		if (infoMode < MODE_MIN || infoMode > MODE_MAX)
			errorType = ERR_UKM;
	}
	else if (infotype == TYPE_PRT)
	{
		for (infoMode = 0; infoMode < N_PRAYER; (infoMode)++)
			if (request.charAt (0) == PRAYERS_NAME[infoMode].charAt (0))
				break;

		print (LEVEL_DEBUG, "Prayer: ");
		if (infoMode < 0 || infoMode >= N_PRAYER)
		{
			errorType = ERR_UKP;
			println (LEVEL_DEBUG, infoMode, DEC, false);
		}
		else
		{
			print (LEVEL_DEBUG, PRAYERS_NAME[infoMode] + " (", false);
			print (LEVEL_DEBUG, infoMode, DEC, false);
			println (LEVEL_DEBUG, ")", false);
		}


		request.remove (0, 1);
	}

	print (LEVEL_DEBUG, infoTypeName (infotype, false) + ": ");
	println (LEVEL_DEBUG, request, false);

	request.toCharArray (requestChar, request.length() + 1);

	print (LEVEL_DEBUG, infoTypeName (infotype, false) + " (char): ");
	println (LEVEL_DEBUG, requestChar, false);

	result = strtol (requestChar, NULL, infotype == TYPE_RGB ? 16 : 10);

	print (LEVEL_DEBUG, infoTypeName (infotype, false) + " (decoded): ");
	println (LEVEL_DEBUG, result, infotype == TYPE_RGB ? HEX : DEC, false);

	if (errorType == ERR_NOE)
		switch (infotype)
		{
			case TYPE_TIM:
				if (result < 0)
					errorType = ERR_OOB;
				else
					setTime (strtol (requestChar, NULL, 10));

				// Debug
				print (LEVEL_DEBUG, "TIME (Current value): ");
				println (LEVEL_DEBUG, now(), DEC, false);
				print (LEVEL_DEBUG, "TIME (Current value) (readable): ");
				digitalClockDisplay (LEVEL_DEBUG);

				initMorningAlarm();
				break;

			case TYPE_RGB:
				if (result < 0 || result > 0xFFFFFF)
					errorType = ERR_OOB;
				else
					rgb[MODE_DEFAULT] = result;

				// Debug
				rgb2color();
				print (LEVEL_DEBUG, "RGB (Current value): ");
				println (LEVEL_DEBUG, rgb[MODE_DEFAULT], HEX, false);
				print (LEVEL_DEBUG, "RED (Current value) : ");
				println (LEVEL_DEBUG, red[MODE_DEFAULT], DEC, false);
				print (LEVEL_DEBUG, "GREEN (Current value): ");
				println (LEVEL_DEBUG, green[MODE_DEFAULT], DEC, false);
				print (LEVEL_DEBUG, "BLUE (Current value): ");
				println (LEVEL_DEBUG, blue[MODE_DEFAULT], DEC, false);
				break;

			case TYPE_ONF:
				if (result != 0 && result != 1)
					errorType = ERR_OOB;
				else
					on = result;

				// Debug
				print (LEVEL_DEBUG, "On (Current value): ");
				if (on != 0 && on != 1)
				{
					print (LEVEL_DEBUG, "Error (", false);
					print (LEVEL_DEBUG, on, DEC, false);
					println (LEVEL_DEBUG, ")", false);
				}
				else
				{
					println (LEVEL_DEBUG, on == 1 ? "True" : "False", false);
				}

				break;

			case TYPE_POW:
				if (result < SEEKBAR_MIN || result > SEEKBAR_MAX)
					errorType = ERR_OOB;
				else
					power[infoMode] = convertBoundaries (result, SEEKBAR_MIN, SEEKBAR_MAX, MIN_POWER, MAX_POWER, true);

				// Debug
				print (LEVEL_DEBUG, "Power (Current value): ");
				println (LEVEL_DEBUG, (int) power[infoMode], DEC, false);

				break;

			case TYPE_MOD:
				if (result < MODE_MIN || result > MODE_MAX)
					errorType = ERR_OOB;
				else
					mode = result;

				// Debug
				print (LEVEL_DEBUG, "Mode (Text): ");
				println (LEVEL_DEBUG, modeName (result, CAPS_FIRST), false);
				print (LEVEL_DEBUG, "Mode (Current value): ");
				print (LEVEL_DEBUG, mode, DEC, false);
				println (LEVEL_DEBUG, " (" + modeName (mode, CAPS_FIRST) + ")", false);

				break;

			case TYPE_PRT:
				if (result < MODE_MIN && result > MODE_MAX)
					errorType = ERR_OOB;
				else
				{
					prayerTime[infoMode][2] = result;
					prayerTime[infoMode][0] = prayerTime[infoMode][2] / 60;
					prayerTime[infoMode][1] = prayerTime[infoMode][2] % 60;
				}

				// Debug
				print (LEVEL_DEBUG, "Prayer time (Current value): ");
				println (LEVEL_DEBUG, prayerTime[infoMode][2], DEC, false);
				print (LEVEL_DEBUG, "Prayer time (Current value) (Readable): ");
				printDigits (LEVEL_DEBUG, prayerTime[infoMode][0]);
				print (LEVEL_DEBUG, ":", false);
				printDigits (LEVEL_DEBUG, prayerTime[infoMode][1]);
				println (LEVEL_DEBUG, false);

				initPrayerAlarms();
				break;

			case TYPE_SPE:
				if (infoMode == MODE_WAKEUP)
				{
					if (result < 0)
						errorType = ERR_OOB;
					else
					{
						speed[infoMode] = result;
						initMorningAlarm();
					}
				}
				else
				{
					if (result < SEEKBAR_MIN || result > SEEKBAR_MAX)
						errorType = ERR_OOB;
					else
						speed[infoMode] = convertBoundaries (result, SEEKBAR_MIN, SEEKBAR_MAX, MIN_SPEED[infoMode], MAX_SPEED[infoMode], true);
				}

				// Debug
				print (LEVEL_DEBUG, "Min Speed: ");
				println (LEVEL_DEBUG, MIN_SPEED[infoMode], DEC, false);
				print (LEVEL_DEBUG, "Max Speed: ");
				println (LEVEL_DEBUG, MAX_SPEED[infoMode], DEC, false);
				print (LEVEL_DEBUG, "Speed (Current value): ");
				println (LEVEL_DEBUG, speed[infoMode], DEC, false);
				break;
		}

	if (errorType != ERR_NOE)
	{
		println (LEVEL_DEBUG, false);
		println (LEVEL_DEBUG, "Variable has not been changed (" + errorTypeName (errorType, false) + ")");
	}
} // decodeRequest
