#include "esp8266.h"

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

		println ("Received info request");
		return;
	}
	if (request.indexOf ("TIME") == 0)
	{
		*pInfoType = TYPE_RTM;

		println ("Received time request");
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
		if (DEBUG_ENABLED)
		{
			*pInfoType  = TYPE_UNK;
			*pErrorType = ERR_UKR;
		}
		else
			return;
	}

	// [DEBUG] Printing full word, world length and information type
	print ("Word: ");
	printlnNoPrefix (request);
	print ("Length: ");
	printlnNoPrefix (requestLength, DEC);
	print ("Type: ");
	printNoPrefix (infoTypeName (*pInfoType, false) + " (");
	printNoPrefix (*pInfoType, DEC);
	printlnNoPrefix (")");

	request.remove (0, 3);

	if (*pInfoType == TYPE_POW || *pInfoType == TYPE_SPE)
	{
		*pInfoMode = request.charAt (0) - '0';
		print ("Info mode: ");
		printNoPrefix (modeName (*pInfoMode));
		printNoPrefix (" (");
		printNoPrefix (*pInfoMode, DEC);
		printlnNoPrefix (")");
		request.remove (0, 1);
		if (*pInfoMode < MODE_MIN || *pInfoMode > MODE_MAX)
			*pErrorType = ERR_UKM;
	}
	else if (*pInfoType == TYPE_PRT)
	{
		for (*pInfoMode = 0;; (*pInfoMode)++)
			if (request.charAt (0) == PRAYERS_NAME[*pInfoMode].charAt (0))
				break;

		print ("Prayer: ");
		if (*pInfoMode < 0 || *pInfoMode >= N_PRAYER)
		{
			*pErrorType = ERR_UKP;
			printlnNoPrefix (*pInfoMode, DEC);
		}
		else
		{
			printNoPrefix (PRAYERS_NAME[*pInfoMode] + " (");
			printNoPrefix (*pInfoMode, DEC);
			printlnNoPrefix (")");
		}
	}

	print (infoTypeName (*pInfoType, false) + ": ");
	printlnNoPrefix (request);

	request.toCharArray (requestChar, request.length() + 1);

	print (infoTypeName (*pInfoType, false) + " (char): ");
	printlnNoPrefix (requestChar);

	*pResult = strtol (requestChar, NULL, *pInfoType == TYPE_RGB ? 16 : 10);

	print (infoTypeName (*pInfoType, false) + " (decoded): ");
	printlnNoPrefix (*pResult, *pInfoType == TYPE_RGB ? HEX : DEC);

	if (*pErrorType == ERR_NOE)
		switch (*pInfoType)
		{
			case TYPE_TIM:
				if (*pResult < 0)
					*pErrorType = ERR_OOB;
				else
					setTime (strtol (requestChar, NULL, 10));

				// Debug
				print ("TIME (Current value): ");
				printlnNoPrefix (now(), DEC);
				print ("TIME (Current value) (readable): ");
				digitalClockDisplay();
				printlnNoPrefix();
				break;

			case TYPE_RGB:
				if (*pResult < 0 || *pResult > 0xFFFFFF)
					*pErrorType = ERR_OOB;
				else
					rgb = *pResult;

				// Debug
				rgb2color();
				print ("RGB (Current value): ");
				printlnNoPrefix (rgb, HEX);
				print ("RED (Current value) : ");
				printlnNoPrefix (red, DEC);
				print ("GREEN (Current value): ");
				printlnNoPrefix (green, DEC);
				print ("BLUE (Current value): ");
				printlnNoPrefix (blue, DEC);
				break;

			case TYPE_ONF:
				if (*pResult != 0 && *pResult != 1)
					*pErrorType = ERR_OOB;
				else
					on = *pResult;

				// Debug
				print ("On (Current value): ");
				printNoPrefix (on == 1 ? "True" : on == 0 ? "False" : "Error (");
				if (on != 0 && on != 1)
				{
					printNoPrefix (on, DEC);
					printNoPrefix (")");
				}
				printlnNoPrefix();

				break;

			case TYPE_POW:
				if (*pResult < 0 || *pResult > 100)
					*pErrorType = ERR_OOB;
				else
					power[*pInfoMode] = *pResult;

				// Debug
				print ("Power (Current value): ");
				printlnNoPrefix ((int) power[*pInfoMode], DEC);

				break;

			case TYPE_MOD:
				if (*pResult < 0 || *pResult > MODE_MAX)
					*pErrorType = ERR_OOB;
				else
					mode = *pResult;

				// Debug
				print ("Mode (Text): ");
				printlnNoPrefix (modeName (*pResult));
				print ("Mode (Current value): ");
				printNoPrefix (mode, DEC);
				printlnNoPrefix (" (" + modeName (mode) + ")");

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
				print ("Prayer time (Current value): ");
				printlnNoPrefix (prayerTime[*pInfoMode][2], DEC);
				print ("Prayer time (Current value) (Readable): ");
				printDigits (prayerTime[*pInfoMode][0]);
				printNoPrefix (":");
				printDigits (prayerTime[*pInfoMode][1]);
				printlnNoPrefix ("\n");

				break;

			case TYPE_SPE:
				if (*pResult < SEEKBAR_MIN || *pResult > SEEKBAR_MAX)
					*pErrorType = ERR_OOB;
				else
					speed[*pInfoMode] = *pResult;

				// Debug
				print ("Speed (Current value): ");
				printlnNoPrefix (speed[*pInfoMode], DEC);
				break;
		}

	if (*pErrorType != ERR_NOE)
		println ("Variable has not been changed (" + errorTypeName (*pErrorType, false) + ")\n");
} // readWeb