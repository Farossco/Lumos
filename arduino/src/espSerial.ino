// ******* readSerial ******* //
int infoType;                              // Information type ("Time", "ON", "RGB", "Power" or "Mode")
int messageLength;                         // Message length
char charRgb[7], charPow[4], charTime[11]; // Char arrays for message decrypting with strtol function
char messageChar[20];                      // Received message
String message;                            // Received message converted to String

void initSerial ()
{
	Serial.begin (DEBUG_BAUD_RATE); // Initialize debug communication
	Serial1.begin (ESP_BAUD_RATE);  // Initialize ESP8266 communication
	Serial2.begin (DFP_BAUD_RATE);  // Initialize DFPlayer communication
}

void waitForTime ()
{
	if (!WAIT_FOR_TIME)
		return;

	// Gently ask for time
	time_t lastMillis = millis() + 5000; // Starting 5s in the future to directly ask for time
	boolean flag      = false;

	while (timeStatus() == timeNotSet || !flag) // Doesn't start if time isn't set and we didn't receive all prayer times
	{
		readSerial();

		flag = true;

		for (int i = 0; i < N_PRAYER; i++)
			if (prayerTime[i][2] == 0)
				flag = false;

		if (millis() - lastMillis >= 5000)
		{
			askForTime();
			lastMillis = millis();
		}
	}
}

// Asking for time to the ESP8266 (via internet)
void askForTime ()
{
	println ("Gently asking for time\n");
	Serial1.print ("TIMEPLEASEz"); // z is the end character
}

// Receive datas from ESP8266 for Wi-Wi control
void readSerial ()
{
	if (!Serial.available() && !Serial1.available())
		return;  // Waiting for incomming datas

	// Reading incomming message
	if (Serial.available())
		message = Serial.readStringUntil ('z');  // z is the end character

	if (Serial1.available())
		message = Serial1.readStringUntil ('z');  // z is the end character

	messageLength = message.length(); // Message length

	// Testing what kind of data we are receiving (Testing if the prefix is present at position 0)
	if (message.indexOf ("INFOPLEASE") != -1)
	{
		printlnNoPrefix();
		println ("Received info request from ESP8266");
		sendInfo(); // We send the variables values to the ESP8266
		return;     // No need to go further
	}
	else if (message.indexOf ("TIM") == 0)
		infoType = TYPE_TIM;
	else if (message.indexOf ("ONF") == 0)
		infoType = TYPE_ONF;
	else if (message.indexOf ("RGB") == 0)
		infoType = TYPE_RGB;
	else if (message.indexOf ("POW") == 0)
		infoType = TYPE_POW;
	else if (message.indexOf ("MOD") == 0)
		infoType = TYPE_MOD;
	else if (message.indexOf ("PRT") == 0)
		infoType = TYPE_PRT;
	else // If the prefix correspond to nothing or there is no prefix
	{
		// If DEBUG is active, we continue with unkown type
		if (DEBUG_ENABLED)
			infoType = TYPE_UNK;

		// If not, we stop
		else return;
	}

	// Testing if data length is valid
	if (messageLength <= 3 && !DEBUG_ENABLED)
		return;
	else if (infoType == TYPE_TIM && messageLength > 13 && !DEBUG_ENABLED)
		return;
	else if (infoType == TYPE_ONF && messageLength != 4 && !DEBUG_ENABLED)
		return;
	else if (infoType == TYPE_RGB && messageLength > 9 && !DEBUG_ENABLED)
		return;
	else if (infoType == TYPE_POW && messageLength > 6 && !DEBUG_ENABLED)
		return;
	else if (infoType == TYPE_MOD && messageLength != 4 && !DEBUG_ENABLED)
		return;
	else if (infoType == TYPE_PRT && messageLength > 7 && !DEBUG_ENABLED)
		return;

	// [DEBUG] Printing full word, world length and information type
	print ("Word: ");
	printlnNoPrefix (message);
	print ("Length: ");
	printlnNoPrefix (messageLength, DEC);
	print ("Type: ");
	printlnNoPrefix (
		infoType == TYPE_TIM ? "TIM" : infoType == TYPE_ONF ? "ON" :
		infoType == TYPE_RGB ? "RGB" : infoType == TYPE_POW ? "POW" :
		infoType == TYPE_MOD ? "MOD" : infoType == TYPE_PRT ? "PRT" : "UNKNOWN");

	message.remove (0, 3); // Remove 3 first characters

	// [DEBUG] printing information without prefix
	print
	(
		infoType == TYPE_TIM ? "TIM: " :
		infoType == TYPE_ONF ? "ONF: " :
		infoType == TYPE_RGB ? "RGB: " :
		infoType == TYPE_POW ? "POW: " :
		infoType == TYPE_MOD ? "MOD: " :
		infoType == TYPE_PRT ? "PRT: " :
		"UNK: "
	);

	printlnNoPrefix
	(
		infoType == TYPE_ONF ?
		(
			message == "1" ? "True" :
			message == "0" ? "False" : "Error"
		) :
		infoType == TYPE_MOD ?
		(
			message.charAt (0) == MODE_DEFAULT + '0' ? "DEFAULT (" :
			message.charAt (0) == MODE_FLASH + '0' ? "FLASH (" :
			message.charAt (0) == MODE_STROBE + '0' ? "STROBE (" :
			message.charAt (0) == MODE_FADE + '0' ? "FADE (" :
			message.charAt (0) == MODE_SMOOTH + '0' ? "SMOOTH (" :
			message.charAt (0) == MODE_WAKEUP + '0' ? "WAKE UP (" : "UNKNOWN ("
		) + message + ") " :
		message
	);

	switch (infoType)
	{
		case TYPE_UNK:
			printlnNoPrefix();
			break;

		case TYPE_TIM:
			message.toCharArray (charTime, 11);
			setTime (strtol (charTime, NULL, 10));

			print ("TIME (number): ");
			printlnNoPrefix (now(), DEC);
			print ("TIME (readable): ");
			digitalClockDisplayNoPrefix();
			printlnNoPrefix ("\n");
			break;

		case TYPE_ONF:
			on = message.charAt (0) - '0';
			printlnNoPrefix();
			break;

		case TYPE_RGB:
			message.toCharArray (charRgb, 7);
			rgb = strtol (charRgb, NULL, 16);

			rgb2color();
			print ("Full RGB (number): ");
			printlnNoPrefix (rgb, HEX);
			print ("RED: ");
			printlnNoPrefix (red, DEC);
			print ("GREEN: ");
			printlnNoPrefix (green, DEC);
			print ("BLUE: ");
			printlnNoPrefix (blue, DEC);
			printlnNoPrefix();
			break;

		case TYPE_POW:
			message.toCharArray (charPow, 4);

			switch (mode)
			{
				case MODE_FLASH:
					flashSpeed = strtol (charPow, NULL, 10) * (MAX_FLASH - (float) MIN_FLASH) / (MAX_POWER - (float) MIN_POWER) + (MIN_FLASH - MIN_POWER);
					break;

				case MODE_STROBE:
					strobeSpeed = strtol (charPow, NULL, 10) * (MAX_STROBE - MIN_STROBE) / (MAX_POWER - MIN_POWER) + (MIN_STROBE - MIN_POWER);
					break;

				case MODE_FADE:
					fadeSpeed = strtol (charPow, NULL, 10) * (MAX_FADE - MIN_FADE) / (MAX_POWER - MIN_POWER) + (MIN_FADE - MIN_POWER);
					break;

				case MODE_SMOOTH:
					smoothSpeed = strtol (charPow, NULL, 10) * (MAX_SMOOTH - MIN_SMOOTH) / (MAX_POWER - MIN_POWER) + (MIN_SMOOTH - MIN_POWER);
					break;

				default:
					power = strtol (charPow, NULL, 10);
					break;
			}

			print ("Full POW (number): ");
			printlnNoPrefix (
				mode == MODE_FLASH ? (int) flashSpeed :
				mode == MODE_STROBE ? (int) strobeSpeed :
				mode == MODE_FADE ? (int) fadeSpeed :
				mode == MODE_SMOOTH ? (int) smoothSpeed : (int) power, DEC);
			printlnNoPrefix();
			break;

		case TYPE_MOD:
			mode = message.charAt (0) - '0';
			printlnNoPrefix();
			break;

		case TYPE_PRT:
			int i;

			for (i = 0; i < N_PRAYER && !(message.charAt (0) == prayersName[i].charAt (0)); i++)
			{ }

			message.remove (0, 1);
			prayerTime[i][2] = message.toInt();
			prayerTime[i][0] = prayerTime[i][2] / 60;
			prayerTime[i][1] = prayerTime[i][2] % 60;

			print ("Prayer: ");
			printlnNoPrefix (prayersName[i]);
			print ("Prayer time: ");
			printlnNoPrefix (prayerTime[i][2], DEC);
			print ("Prayer time (Readable): ");
			printDigits (prayerTime[i][0]);
			printNoPrefix (":");
			printDigits (prayerTime[i][1]);
			printlnNoPrefix ("\n");

			break;
	}
} // readSerial

sendInfo()
{
	printlnNoPrefix();
	println ("Sending variables infos to the ESP8266");

	print ("Sending RGB: ");
	Serial1.print ("RGB");
	Serial1.print (rgb, HEX);
	serial1.print ("z");

	print ("Sending On: ");
	Serial1.print ("ONF");
	Serial1.print (on ? 1 : 0, DEC);
	serial1.print ("z");

	print ("Sending Power: ");
	Serial1.print ("POW");
	Serial1.print (power, DEC);
	serial1.print ("z");

	print ("Sending Mode: ");
	Serial1.print ("MOD");
	Serial1.print (mode, DEC);
	serial1.print ("z");
}
