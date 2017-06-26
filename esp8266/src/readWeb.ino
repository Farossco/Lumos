// Web server requests
int webInfoType;
String request;
char requestChar[10];
byte buf[20];
boolean error;
boolean jsonSent;
unsigned long tempValue;

void readWeb ()
{
	client = server.available(); // Check if a client has connected

	if (!client) return;  // If nobody connected, we stop here

	while (!client.available()) // Wait until the client sends some data
		delay (1);

	request = client.readStringUntil ('\r'); // Read the first line of the request

	printlnNoPrefix();
	print ("Received request from ");
	printlnNoPrefix (client.remoteIP());

	// [DEBUG] Printing the incomming request

	println ("Request: " + request);

	request.remove (0, 5);
	request.remove (request.indexOf (" "), request.length() - request.indexOf (" "));

	println ("Word: " + request);

	client.flush();
	error = false;

	// Match the request
	if (request.indexOf ("INFO") == 0)
	{
		sendJsonToClient ("OK", ""); // We just send the json and stop here
		return;
	}
	else if (request.indexOf ("RGB=") == 0)
		webInfoType = TYPE_RGB;
	else if (request.indexOf ("ONF=") == 0)
		webInfoType = TYPE_ONF;
	else if (request.indexOf ("POW=") == 0)
		webInfoType = TYPE_POW;
	else if (request.indexOf ("MOD=") == 0)
		webInfoType = TYPE_MOD;
	else
	{
		error       = true;
		webInfoType = TYPE_UNK;
		sendJsonToClient ("ERROR", "Unknown request"); // We just send the json and stop here
		jsonSent = true;
	}

	request.remove (0, 4);

	print
	(
		webInfoType == TYPE_RGB ? "RGB: " :
		webInfoType == TYPE_ONF ? "On: " :
		webInfoType == TYPE_POW ? "Power: " :
		webInfoType == TYPE_MOD ? "Mode: " :
		"Unknown: "
	);
	printlnNoPrefix (request);
	request.toCharArray (requestChar, request.length() + 1);

	print
	(
		webInfoType == TYPE_RGB ? "RGB (char): " :
		webInfoType == TYPE_ONF ? "On (char): " :
		webInfoType == TYPE_POW ? "Power (char): " :
		webInfoType == TYPE_MOD ? "Mode (char): " :
		"UNK (char): "
	);
	printlnNoPrefix (requestChar);

	tempValue = strtol (requestChar, NULL, webInfoType == TYPE_RGB ? 16 : 10);

	print
	(
		webInfoType == TYPE_RGB ? "RGB (decoded): " :
		webInfoType == TYPE_ONF ? "On (decoded): " :
		webInfoType == TYPE_POW ? "Power (decoded): " :
		webInfoType == TYPE_MOD ? "Mode (decoded): " :
		"UNK (decoded): "
	);

	printlnNoPrefix (tempValue, webInfoType == TYPE_RGB ? HEX : DEC);

	switch (webInfoType)
	{
		case TYPE_RGB:
			if (tempValue < 0 || tempValue > 0xFFFFFF)
				error = true;
			else
				rgb = tempValue;
			break;

		case TYPE_ONF:
			if (tempValue != 0 && tempValue != 1)
				error = true;
			else
				on = tempValue;
			break;

		case TYPE_POW:
			if (tempValue < 0 && tempValue > 100)
				error = true;
			else
				power = tempValue;
			break;

		case TYPE_MOD:
			print ("Mode (Text) = ");
			printlnNoPrefix (modeName (tempValue));

			if (tempValue < 0 && tempValue > MODE_MAX)
				error = true;
			else
				mode = tempValue;
			break;
	}

	if (error)
	{
		println ("CODING ERROR !");
	}
	else
	{
		// Now printing the correct value in hexadecimal for RGB type and in decimal for any other type
		print ("Sending to Arduino: ");
		Serial.print
		(
			webInfoType == TYPE_RGB ? "RGB" :
			webInfoType == TYPE_ONF ? "ONF" :
			webInfoType == TYPE_POW ? "POW" :
			webInfoType == TYPE_MOD ? "MOD" :
			"UNK"
		);
		Serial.print
		(
			webInfoType == TYPE_RGB ? rgb :
			webInfoType == TYPE_ONF ? on :
			webInfoType == TYPE_POW ? (int) power :
			webInfoType == TYPE_MOD ? mode :
			error
			,
			webInfoType == TYPE_RGB ? HEX : DEC
		);
		Serial.print ('z'); // End character

		printlnNoPrefix();
	}

	if (!jsonSent)
		sendJsonToClient (error ? "ERROR" : "OK", error ? "Error while decoding the request" : "");

	client.stop();
} // readWeb
