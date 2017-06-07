// Receive datas from ESP8266 for Wi-Wi control
void readSerial ()
{
	// If nothing is comming, we stop here
	if (!Serial.available())
		return;

	int serialInfoType;                        // Information type ("Time", "ON", "RGB", "Power" or "Mode")
	int messageLength;                         // Message length
	char charRgb[7], charPow[4], charTime[11]; // Char arrays for message decrypting with strtol function
	String message;                            // Received message
	boolean error;                             // If we ran through an error

	error   = false;
	message = Serial.readStringUntil ('z'); // z is the end character

	messageLength = message.length(); // Message length

	// Testing what kind of data we are receiving (Testing if the prefix is present at position 0)
	if (message.indexOf ("TIMEPLEASE") != -1)
	{
		printlnNoPrefix();
		println ("Received time request from arduino");
		sendTime(); // We send the time to the Arduino
		return;     // No need to go further
	}
	else if (message.indexOf ("ONF") == 0)
		serialInfoType = TYPE_ONF;
	else if (message.indexOf ("RGB") == 0)
		serialInfoType = TYPE_RGB;
	else if (message.indexOf ("POW") == 0)
		serialInfoType = TYPE_POW;
	else if (message.indexOf ("MOD") == 0)
		serialInfoType = TYPE_MOD;
	else // If the prefix correspond to nothing or there is no prefix
	{
		// If DEBUG is active, we continue with unkown type
		if (DEBUG_ENABLED)
			serialInfoType = TYPE_UNK;

		// If not, we stop
		else return;
	}

	// Testing if data length is valid
	if (messageLength <= 3 && !DEBUG_ENABLED)
		return;
	else if (serialInfoType == TYPE_ONF && messageLength != 4)
		if (!DEBUG_ENABLED)
			return;
		else
			error = true;
	else if (serialInfoType == TYPE_RGB && messageLength > 9)
		if (!DEBUG_ENABLED)
			return;
		else
			error = true;
	else if (serialInfoType == TYPE_POW && messageLength > 6)
		if (!DEBUG_ENABLED)
			return;
		else
			error = true;
	else if (serialInfoType == TYPE_MOD && messageLength != 4)
		if (!DEBUG_ENABLED)
			return;
		else
			error = true;

	// [DEBUG] Printing full word, world length and information type
	printlnNoPrefix();
	print ("Word: ");
	printlnNoPrefix (message);
	print ("Length: ");
	printlnNoPrefix (messageLength, DEC);
	print ("Type: ");
	printlnNoPrefix (
		serialInfoType == TYPE_ONF ? "ON" : serialInfoType == TYPE_RGB ? "RGB" :
		serialInfoType == TYPE_POW ? "POW" : serialInfoType == TYPE_MOD ? "MOD" : "UNKNOWN");

	message.remove (0, 3); // Remove 3 first characters

	// [DEBUG] printing information without prefix
	print
	(
		serialInfoType == TYPE_ONF ? "ONF: " :
		serialInfoType == TYPE_RGB ? "RGB: " :
		serialInfoType == TYPE_POW ? "POW: " :
		serialInfoType == TYPE_MOD ? "MOD: " :
		"UNK: "
	);

	printlnNoPrefix
	(
		serialInfoType == TYPE_ONF ?
		(
			message == "1" ? "True" :
			message == "0" ? "False" : "Error"
		) :
		serialInfoType == TYPE_MOD ?
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

	switch (serialInfoType)
	{
		case TYPE_ONF:
			if ((message.charAt (0) - '0' == 0 || message.charAt (0) - '0' == 1) && !error)
				on = message.charAt (0) - '0';
			else
				println ("Not set !");

			print ("ON (Current value): ");
			printNoPrefix (on == 1 ? "True" : on == 0 ? "False" : "Error (");
			if (on > 1)
			{
				printNoPrefix (on, DEC);
				printNoPrefix (")");
			}
			printlnNoPrefix();
			break;

		case TYPE_RGB:
			message.toCharArray (charRgb, 7);
			if (strtol (charRgb, NULL, 16) >= 0 && strtol (charRgb, NULL, 16) <= 0xFFFFFF && !error)
				rgb = strtol (charRgb, NULL, 16);
			else
				println ("Not set !");

			print ("Full RGB (Current value): ");
			printlnNoPrefix (rgb, HEX);
			break;

		case TYPE_POW:
			message.toCharArray (charPow, 4);
			if (strtol (charPow, NULL, 10) >= 0 && strtol (charPow, NULL, 10) <= 100 && !error)
				power = strtol (charPow, NULL, 10);
			else
				println ("Not set !");

			print ("Full POW (Current value): ");
			printlnNoPrefix ((int) power, DEC);
			break;

		case TYPE_MOD:
			if (message.charAt (0) - '0' >= 0 && message.charAt (0) - '0' <= MODE_MAX && !error)
				mode = message.charAt (0) - '0';
			else
				println ("Not set !");
			break;
	}
} // readSerial
