#include "arduino.h"

void decodeRequest (String request, long * pResult, int * pInfoMode, int * pInfoType, int * pErrorType)
{
	char requestChar[20];
	int requestLength = request.length();

	*pResult    = 0;
	*pInfoType  = 0;
	*pErrorType = ERR_NOE;
	*pInfoMode  = 0;

	if (request.indexOf ("INFO") == 0)
	{
		*pInfoType = TYPE_RIF;

		println (LEVEL_DEBUG, "Received info request");
		return;
	}
	if (request.indexOf ("TIME") == 0)
	{
		*pInfoType = TYPE_RTM;

		println (LEVEL_DEBUG, "Received time request");
		return;
	}
	else if (request.indexOf ("TIM") == 0)
		*pInfoType = TYPE_TIM;
	else if (request.indexOf ("RGB") == 0)
		*pInfoType = TYPE_RGB;
	else if (request.indexOf ("ONF") == 0)
		*pInfoType = TYPE_ONF;
	else if (request.indexOf ("POW") == 0)
		*pInfoType = TYPE_POW;
	else if (request.indexOf ("MOD") == 0)
		*pInfoType = TYPE_MOD;
	else if (request.indexOf ("PRT") == 0)
		*pInfoType = TYPE_PRT;
	else if (request.indexOf ("SPE") == 0)
		*pInfoType = TYPE_SPE;
	else
	{
		if (SERIAL_LOG_ENABLED || SD_LOG_ENABLED)
		{
			*pInfoType  = TYPE_UNK;
			*pErrorType = ERR_UKR;
		}
		else
			return;
	}

	// [DEBUG] Printing full word, world length and information type
	printlnNoPrefix(LEVEL_DEBUG);
	print (LEVEL_DEBUG, "Word: ");
	printlnNoPrefix (LEVEL_DEBUG, request);
	print (LEVEL_DEBUG, "Length: ");
	printlnNoPrefix (LEVEL_DEBUG, requestLength, DEC);
	print (LEVEL_DEBUG, "Type: ");
	printNoPrefix (LEVEL_DEBUG, infoTypeName (*pInfoType, false) + " (");
	printNoPrefix (LEVEL_DEBUG, *pInfoType, DEC);
	printlnNoPrefix (LEVEL_DEBUG, ")");

	request.remove (0, 3);

	if (*pInfoType == TYPE_POW || *pInfoType == TYPE_SPE)
	{
		*pInfoMode = request.charAt (0) - '0';
		print (LEVEL_DEBUG, "Info mode: ");
		printNoPrefix (LEVEL_DEBUG, modeName (*pInfoMode, CAPS_FIRST));
		printNoPrefix (LEVEL_DEBUG, " (");
		printNoPrefix (LEVEL_DEBUG, *pInfoMode, DEC);
		printlnNoPrefix (LEVEL_DEBUG, ")");
		request.remove (0, 1);
		if (*pInfoMode < MODE_MIN || *pInfoMode > MODE_MAX)
			*pErrorType = ERR_UKM;
	}
	else if (*pInfoType == TYPE_PRT)
	{
		for (*pInfoMode = 0; *pInfoMode < N_PRAYER; (*pInfoMode)++)
			if (request.charAt (0) == PRAYERS_NAME[*pInfoMode].charAt (0))
				break;

		print (LEVEL_DEBUG, "Prayer: ");
		if (*pInfoMode < 0 || *pInfoMode >= N_PRAYER)
		{
			*pErrorType = ERR_UKP;
			printlnNoPrefix (LEVEL_DEBUG, *pInfoMode, DEC);
		}
		else
		{
			printNoPrefix (LEVEL_DEBUG, PRAYERS_NAME[*pInfoMode] + " (");
			printNoPrefix (LEVEL_DEBUG, *pInfoMode, DEC);
			printlnNoPrefix (LEVEL_DEBUG, ")");
		}


		request.remove (0, 1);
	}

	print (LEVEL_DEBUG, infoTypeName (*pInfoType, false) + ": ");
	printlnNoPrefix (LEVEL_DEBUG, request);

	request.toCharArray (requestChar, request.length() + 1);

	print (LEVEL_DEBUG, infoTypeName (*pInfoType, false) + " (char): ");
	printlnNoPrefix (LEVEL_DEBUG, requestChar);

	*pResult = strtol (requestChar, NULL, *pInfoType == TYPE_RGB ? 16 : 10);

	print (LEVEL_DEBUG, infoTypeName (*pInfoType, false) + " (decoded): ");
	printlnNoPrefix (LEVEL_DEBUG, *pResult, *pInfoType == TYPE_RGB ? HEX : DEC);

	if (*pErrorType == ERR_NOE)
		switch (*pInfoType)
		{
			case TYPE_TIM:
				if (*pResult < 0)
					*pErrorType = ERR_OOB;
				else
					setTime (strtol (requestChar, NULL, 10));

				// Debug
				print (LEVEL_DEBUG, "TIME (Current value): ");
				printlnNoPrefix (LEVEL_DEBUG, now(), DEC);
				print (LEVEL_DEBUG, "TIME (Current value) (readable): ");
				digitalClockDisplay(LEVEL_DEBUG);
				break;

			case TYPE_RGB:
				if (*pResult < 0 || *pResult > 0xFFFFFF)
					*pErrorType = ERR_OOB;
				else
					rgb[MODE_DEFAULT] = *pResult;

				// Debug
				rgb2color();
				print (LEVEL_DEBUG, "RGB (Current value): ");
				printlnNoPrefix (LEVEL_DEBUG, rgb[MODE_DEFAULT], HEX);
				print (LEVEL_DEBUG, "RED (Current value) : ");
				printlnNoPrefix (LEVEL_DEBUG, red[MODE_DEFAULT], DEC);
				print (LEVEL_DEBUG, "GREEN (Current value): ");
				printlnNoPrefix (LEVEL_DEBUG, green[MODE_DEFAULT], DEC);
				print (LEVEL_DEBUG, "BLUE (Current value): ");
				printlnNoPrefix (LEVEL_DEBUG, blue[MODE_DEFAULT], DEC);
				break;

			case TYPE_ONF:
				if (*pResult != 0 && *pResult != 1)
					*pErrorType = ERR_OOB;
				else
					on = *pResult;

				// Debug
				print (LEVEL_DEBUG, "On (Current value): ");
				if (on != 0 && on != 1)
				{
					printNoPrefix (LEVEL_DEBUG, "Error (");
					printNoPrefix (LEVEL_DEBUG, on, DEC);
					printlnNoPrefix (LEVEL_DEBUG, ")");
				}
				else
				{
					printlnNoPrefix (LEVEL_DEBUG, on == 1 ? "True" : "False" );
				}

				break;

			case TYPE_POW:
				if (*pResult < 0 || *pResult > 100)
					*pErrorType = ERR_OOB;
				else
					power[*pInfoMode] = *pResult;

				// Debug
				print (LEVEL_DEBUG, "Power (Current value): ");
				printlnNoPrefix (LEVEL_DEBUG, (int) power[*pInfoMode], DEC);

				break;

			case TYPE_MOD:
				if (*pResult < 0 || *pResult > MODE_MAX)
					*pErrorType = ERR_OOB;
				else
					mode = *pResult;

				// Debug
				print (LEVEL_DEBUG, "Mode (Text): ");
				printlnNoPrefix (LEVEL_DEBUG, modeName (*pResult, CAPS_FIRST));
				print (LEVEL_DEBUG, "Mode (Current value): ");
				printNoPrefix (LEVEL_DEBUG, mode, DEC);
				printlnNoPrefix (LEVEL_DEBUG, " (" + modeName (mode, CAPS_FIRST) + ")");

				break;

			case TYPE_PRT:
				if (*pResult < 0 && *pResult > MODE_MAX)
					*pErrorType = ERR_OOB;
				else
				{
					prayerTime[*pInfoMode][2] = *pResult;
					prayerTime[*pInfoMode][0] = prayerTime[*pInfoMode][2] / 60;
					prayerTime[*pInfoMode][1] = prayerTime[*pInfoMode][2] % 60;
				}

				// Debug
				print (LEVEL_DEBUG, "Prayer time (Current value): ");
				printlnNoPrefix (LEVEL_DEBUG, prayerTime[*pInfoMode][2], DEC);
				print (LEVEL_DEBUG, "Prayer time (Current value) (Readable): ");
				printDigits (LEVEL_DEBUG, prayerTime[*pInfoMode][0]);
				printNoPrefix (LEVEL_DEBUG, ":");
				printDigits (LEVEL_DEBUG, prayerTime[*pInfoMode][1]);
				printlnNoPrefix(LEVEL_DEBUG);
				break;

			case TYPE_SPE:
				if (*pResult < SEEKBAR_MIN || *pResult > SEEKBAR_MAX)
					*pErrorType = ERR_OOB;
				else
					speed[*pInfoMode] = *pResult * (MAX_SPEED[*pInfoMode] - MIN_SPEED[*pInfoMode]) / (SEEKBAR_MAX - SEEKBAR_MIN) + (MIN_SPEED[*pInfoMode] - SEEKBAR_MIN);

				// Debug
				print (LEVEL_DEBUG, "Min Speed: ");
				printlnNoPrefix (LEVEL_DEBUG, MIN_SPEED[*pInfoMode], DEC);
				print (LEVEL_DEBUG, "Max Speed: ");
				printlnNoPrefix (LEVEL_DEBUG, MAX_SPEED[*pInfoMode], DEC);
				print (LEVEL_DEBUG, "Speed (Current value): ");
				printlnNoPrefix (LEVEL_DEBUG, speed[*pInfoMode], DEC);
				break;
		}

	if (*pErrorType != ERR_NOE)
	{
		printlnNoPrefix(LEVEL_DEBUG);
		println (LEVEL_DEBUG, "Variable has not been changed (" + errorTypeName (*pErrorType, false) + ")");
	}
		
} // readWeb
