// Web server requests
int webInfoType;
String request;
byte buf[20];
boolean error;

void readWeb ()
{
	client = server.available(); // Check if a client has connected

	if (!client) return;  // If nobody connected, we stop here

	while (!client.available()) // Wait until the client sends some data
		delay (1);

	request = client.readStringUntil ('\r'); // Read the first line of the request

	// [DEBUG] Printing the incomming request
	printlnNoPrefix();
	println ("Request: \"" + request + "\"");

	client.flush();
	error = false;

	// Match the request
	if (request.indexOf ("/RGB=") != -1)
	{
		getRgb();
		webInfoType = TYPE_RGB;
	}
	else if (request.indexOf ("/ONF=") != -1)
	{
		getOn();
		webInfoType = TYPE_ONF;
	}
	else if (request.indexOf ("/POW=") != -1)
	{
		getPow();
		webInfoType = TYPE_POW;
	}
	else if (request.indexOf ("/MOD=") != -1)
	{
		getMode();
		webInfoType = TYPE_MOD;
	}
	else
	{
		error       = true;
		webInfoType = TYPE_UNK;
	}

	if (error)
	{
		println ("CODING ERROR !");
	}
	else
	{
		print ("Sending to Arduino: ");

		// Now printing the correct vale in hexadecimal for RGB type and in decimal for any other type
		switch (webInfoType)
		{
			case TYPE_ONF:
				Serial.print ("ONF");
				Serial.print (on, DEC);
				Serial.print ('z'); // End character
				break;

			case TYPE_POW:
				Serial.print ("POW");
				Serial.print ((int) power, DEC);
				Serial.print ('z'); // End character
				break;

			case TYPE_RGB:
				Serial.print ("RGB");
				Serial.print (rgb, HEX);
				Serial.print ('z'); // End character
				break;

			case TYPE_MOD:
				Serial.print ("MOD");
				Serial.print (mode, DEC);
				Serial.print ('z'); // End character
				break;
		}

		printlnNoPrefix();
	}

	// Return the response to the client
	client.print ("HTTP/1.1 ");
	client.println (error ? "400 FORMAT ERROR" : "200 OK");
	client.println ("Content-Type: application/json");
	client.println ("Connection: keep-alive");
	client.println ("LosBigick: ESP8266");
	client.println (""); // Do not forget this one

	sendJson (error ? "ERROR" : "OK");
} // readWeb

// Decoding RGB request
void getRgb ()
{
	request.getBytes (buf, 16);

	// [DEBUG] Printing the incomming value ascii code
	print ("ASCII values: |");
	for (int i = 9; i < 15; i++)
	{
		printNoPrefix (buf[i], HEX);
		printNoPrefix ("|");
	}
	printlnNoPrefix();
	println ("----------------------");
	print ("Decoded values: |");

	// Converting values from their ascii
	for (int i = 9; i < 15; i++)
	{
		if ((buf[i] >= '0') && (buf[i] <= '9'))
		{
			buf[i] = buf[i] - '0';
		}
		else if ((buf[i] >= 'A') && (buf[i] <= 'F'))
		{
			buf[i] = buf[i] - 'A' + 10;
		}
		else // If one of the byte is not an hexadecimal value
		{
			error = true;
			break;
		}

		// [DEBUG] Now printing decoded values
		printNoPrefix (buf[i], HEX);
		printNoPrefix ("|");
	}

	rgb = 0;
	// Converting it to an integer
	for (int i = 9; i < 15; i++)
		rgb += buf[i] * pow (16, 5 - (i - 9));

	// [DEBUG] Printing the converted value
	printlnNoPrefix();
	print ("RGB = ");
	printlnNoPrefix (rgb, HEX);
} // getRgb

// Decoding on/off request
void getOn ()
{
	request.getBytes (buf, 11);

	// Only reading byte at position 8 and converting it to an integer
	on = buf[9] - '0';

	print ("ASCII value: ");
	printlnNoPrefix (buf[9], DEC);
	print ("Decoded value: ");
	printlnNoPrefix (buf[9] - '0', DEC);
	print ("ONF = ");
	printlnNoPrefix (on ? "True" : "False");

	// if the value is not correct, set it to -1
	if (on != 0 && on != 1)
		error = true;
}

// Decoding power request
void getPow ()
{
	request.getBytes (buf, 13);

	// [DEBUG] Printing the incomming value ascii code
	print ("ASCII values: |");
	for (int i = 9; i < 12; i++)
	{
		printNoPrefix (buf[i], HEX);
		printNoPrefix ("|");
	}
	printlnNoPrefix();
	println ("----------------------");
	print ("Decoded values: |");

	// Converting values from their ascii
	for (int i = 9; i < 12; i++)
	{
		if ((buf[i] >= '0') && (buf[i] <= '9'))
		{
			buf[i] = buf[i] - '0';
		}
		else // If one of the byte is not an decimal value
		{
			error = true;
			break;
		}

		// [DEBUG] Now printing decoded values
		printNoPrefix (buf[i], HEX);
		printNoPrefix ("|");
	}

	power = 0;
	// Converting it to an integer
	for (int i = 9; i < 12; i++)
		power += buf[i] * pow (10, 2 - (i - 9));

	// [DEBUG] Printing the converted value
	printlnNoPrefix();
	print ("POW = ");
	printlnNoPrefix ((int) power, DEC);

	// If the value is not between 0 and 100, set error flag
	if (power < 0 || power > 100)
		error = true;
} // getPow

// Decoding mode request
void getMode ()
{
	request.getBytes (buf, 11);

	mode = buf[9] - '0';

	print ("Mode = ");
	printlnNoPrefix (mode, DEC);
	print ("Mode (Text) = ");
	printlnNoPrefix
	(
		mode == 0 ? "DEFAULT" :
		mode == 1 ? "FLASH" :
		mode == 2 ? "STROBE" :
		mode == 3 ? "FADE" :
		mode == 4 ? "SMOOTH" :
		mode == 5 ? "WAKEUP" :
		"UNKNOWN"
	);

	if (mode < MODE_MIN || mode > MODE_MAX)
		error = true;
}
