void decodeRequest (String request)
{
	char requestChar[10];
	int requestLength = request.length();

	result    = 0;
	infoType  = 0;
	errorType = ERR_NOE;
	infoMode  = 0;

	if (request.indexOf ("INFO") == 0)
	{
		infoType = TYPE_RIF;

		println ("Received info request");
		return;
	}
	if (request.indexOf ("TIME") == 0)
	{
		infoType = TYPE_RTM;

		println ("Received time request");
		return;
	}
	else if (request.indexOf ("TIM") == 0)
		infoType = TYPE_TIM;
	else if (request.indexOf ("RGB") == 0)
		infoType = TYPE_RGB;
	else if (request.indexOf ("ONF") == 0)
		infoType = TYPE_ONF;
	else if (request.indexOf ("POW") == 0)
		infoType = TYPE_POW;
	else if (request.indexOf ("MOD") == 0)
		infoType = TYPE_MOD;
	else if (request.indexOf ("PRT") == 0)
		infoType = TYPE_PRT;
	else if (request.indexOf ("SPE") == 0)
		infoType = TYPE_SPE;
	else
	{
		if (DEBUG_ENABLED)
		{
			infoType  = TYPE_UNK;
			errorType = ERR_UKR;
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
	printNoPrefix (infoTypeName (infoType, false) + " (");
	printNoPrefix (infoType, DEC);
	printlnNoPrefix (")");

	request.remove (0, 3);

	if (infoType == TYPE_RGB || infoType == TYPE_POW || infoType == TYPE_SPE)
	{
		infoMode = request.charAt (0) - '0';
		print ("Info mode: ");
		printNoPrefix (modeName (infoMode));
		printNoPrefix (" (");
		printNoPrefix (infoMode, DEC);
		printlnNoPrefix (")");
		request.remove (0, 1);
		if (infoMode < MODE_MIN || infoMode > MODE_MAX)
			errorType = ERR_UKM;
	}
	else if (infoType == TYPE_PRT)
	{
		for (infoMode = 0; infoMode < N_PRAYER && !(request.charAt (0) == prayersName[infoMode].charAt (0)); infoMode++)
		{ }

		print ("Prayer: ");
		printNoPrefix (prayersName[infoMode] + " (");
		printNoPrefix (infoMode, DEC);
		printlnNoPrefix (")");

		request.remove (0, 1);
	}

	print (infoTypeName (infoType, false) + ": ");
	printlnNoPrefix (request);

	request.toCharArray (requestChar, request.length() + 1);

	print (infoTypeName (infoType, false) + " (char): ");
	printlnNoPrefix (requestChar);

	result = strtol (requestChar, NULL, infoType == TYPE_RGB ? 16 : 10);

	print (infoTypeName (infoType, false) + " (decoded): ");
	printlnNoPrefix (result, infoType == TYPE_RGB ? HEX : DEC);

	if (errorType == ERR_NOE)
		switch (infoType)
		{
			case TYPE_TIM:
				if (result < 0)
					errorType = ERR_OOB;
				else
					setTime (strtol (requestChar, NULL, 10));

				// Debug
				print ("TIME (number): ");
				printlnNoPrefix (now(), DEC);
				print ("TIME (readable): ");
				digitalClockDisplay();
				printlnNoPrefix();
				break;

			case TYPE_RGB:
				if (result < 0 || result > 0xFFFFFF)
					errorType = ERR_OOB;
				else
					rgb = result;

				// Debug
				rgb2color();
				print ("RGB (Current value): ");
				printlnNoPrefix (rgb, HEX);
				print ("RED: ");
				printlnNoPrefix (red, DEC);
				print ("GREEN: ");
				printlnNoPrefix (green, DEC);
				print ("BLUE: ");
				printlnNoPrefix (blue, DEC);
				printlnNoPrefix();
				break;

			case TYPE_ONF:
				if (result != 0 && result != 1)
					errorType = ERR_OOB;
				else
					on = result;

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
				if (result < 0 || result > 100)
					errorType = ERR_OOB;
				else
					power[infoMode] = result;

				// Debug
				print ("Power (Current value): ");
				printlnNoPrefix ((int) power[infoMode], DEC);

				break;

			case TYPE_MOD:
				if (result < 0 || result > MODE_MAX)
					errorType = ERR_OOB;
				else
					mode = result;

				// Debug
				print ("Mode (Text): ");
				printlnNoPrefix (modeName (result));
				print ("Mode (Current value): ");
				printNoPrefix (mode, DEC);
				printlnNoPrefix (" (" + modeName (mode) + ")");

				break;

			case TYPE_PRT:
				if (result < 0 && result > MODE_MAX)
					errorType = ERR_OOB;
				else
				{
					prayerTime[infoMode][2] = result;
					prayerTime[infoMode][0] = prayerTime[infoMode][2] / 60;
					prayerTime[infoMode][1] = prayerTime[infoMode][2] % 60;
				}

				// Debug
				print ("Prayer time (Current value): ");
				printlnNoPrefix (prayerTime[infoMode][2], DEC);
				print ("Prayer time (Readable) (Current value): ");
				printDigits (prayerTime[infoMode][0]);
				printNoPrefix (":");
				printDigits (prayerTime[infoMode][1]);
				printlnNoPrefix ("\n");

				break;

			case TYPE_SPE:
				if ((int) result < minSpeed[infoMode] || result > maxSpeed[infoMode])
					errorType = ERR_OOB;
				else
					speed[infoMode] = result;

				// Debug
				print ("Min Speed: ");
				printlnNoPrefix (minSpeed[infoMode], DEC);
				print ("Max Speed: ");
				printlnNoPrefix (maxSpeed[infoMode], DEC);
				print ("Speed (Current value): ");
				printlnNoPrefix (speed[infoMode], DEC);
				break;
		}

	if (errorType != ERR_NOE)
		println ("Variable has not been changed (" + ErrorTypeName (errorType, false) + ")\n");
} // readWeb
