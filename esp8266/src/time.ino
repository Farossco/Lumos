void sendTime ()
{
	getPrayerTime();

	printlnNoPrefix();
	print ("Time is: ");
	digitalClockDisplay();

	if (timeStatus() != timeNotSet)
	{
		printlnNoPrefix();
		print ("Sending time to arduino: ");
		delay (1);

		Serial.print ("TIM");
		Serial.print (now());
		Serial.print ('z');

		for (int i = 0; i < 6; i++)
		{
			printlnNoPrefix();
			print ("Sending prayer time to arduino (");
			printNoPrefix (prayersName[i]);
			printNoPrefix (") : ");
			delay (1);

			delay (100);
			Serial.print ("PRT");
			Serial.print (prayersName[i][0]); // First letter of the prayer name
			Serial.print (prayerTime[i][2]);
			Serial.print ('z');
		}
	}
	else
	{
		printlnNoPrefix();
		print ("Time not set, not sending time to arduino...");
	}

	printlnNoPrefix();
} // sendTime

void getPrayerTime ()
{
	printlnNoPrefix();
	print ("Connecting to ");
	printlnNoPrefix (HOST);

	// Use WiFiClient class to create TCP connections
	if (!client.connect (HOST, HTTP_PORT))
	{
		println ("Connection failed");
		return;
	}

	// We now create an url for the request
	String url = "/timings/0?latitude=";
	url += LATITUDE;
	url += "&longitude=";
	url += LONGITUDE;
	url += "&timezonestring=";
	url += TIME_ZONE;
	url += "&method=";
	url += METHOD;

	// [DEBUG] Printing the url
	print ("Requesting URL: ");
	printlnNoPrefix (url);

	// Sending web request
	client.print (String ("GET ") + url + " HTTP/1.1\r\n" + "Host: " + HOST + "\r\n" + "Connection: close\r\n\r\n");
	timeout = millis();

	// Waiting for an answer
	while (!client.available())
	{
		// If nothing come out after the timeout, we abort
		if (millis() - timeout > REQUEST_TIMEOUT)
		{
			println ("Client Timeout !");
			client.stop();
			return;
		}
	}

	println ("Server answer:");
	println();
	println ("<======================================== Start ========================================>");

	// Read all the lines of the reply from server and print them to Serial
	while (client.available())
	{
		line = client.readStringUntil ('\r');

		printedLine = line;
		printedLine.replace ('\r', ' '); // Remove all occurences of \r
		printedLine.replace ('\n', ' '); // Remove all occurences of \n

		println (printedLine); // [DEBUG] We print the line we're currently reading
	}
	println ("<========================================= End =========================================>");
	println();

	if (line.indexOf ("{\"code") == 1)
	{
		// The first charactere is a nl, so we don't want it
		line = line.substring (1);
		println ("Success !");
	}
	else
	{
		print ("Failed ! (");
		printNoPrefix (line.indexOf ("{\"code"), DEC);
		printlnNoPrefix (")");
	}

	println ("Closing connection");

	// At this point, the last line of the answer is in the line variable,
	// that's actually the one we want

	print ("Json: ");
	printlnNoPrefix (line);

	JsonObject& jsonRootPrayer = jsonBuffer.parseObject (line);
	code      = jsonRootPrayer["code"];
	status    = jsonRootPrayer["status"];
	timestamp = jsonRootPrayer["data"]["date"]["timestamp"];

	printlnNoPrefix();
	print ("Code: ");
	printlnNoPrefix (code, DEC);

	print ("Status: ");
	printlnNoPrefix (status);
	printlnNoPrefix();

	if (code != 200 && status != "OK")
	{
		println ("Wrong status, leaving.");
		return;
	}

	// Parsing all paryers time (Format HH:MM)
	for (int i = 0; i < 6; i++)
		prayerTimeString[i] = jsonRootPrayer["data"]["timings"][prayersName[i]];

	// Converting it into integer
	for (int i = 0; i < 6; i++)
	{
		prayerTime[i][0] = (prayerTimeString[i][0] - '0') * 10 + prayerTimeString[i][1] - '0'; // Hour
		prayerTime[i][1] = (prayerTimeString[i][3] - '0') * 10 + prayerTimeString[i][4] - '0'; // Minute
		prayerTime[i][2] = prayerTime[i][0] * 60 + prayerTime[i][1];                           // Hour * 60 + minute
	}

	// [DEBUG] Some printing for each prayer
	for (int i = 0; i < 6; i++)
	{
		print (prayersName[i]);
		printNoPrefix (": ");
		printlnNoPrefix (prayerTimeString[i]);
		print (prayersName[i]);
		printNoPrefix (" (Integer): ");
		printNoPrefix (prayerTime[i][0], DEC);
		printNoPrefix (":");
		printNoPrefix (prayerTime[i][1], DEC);
		printNoPrefix (" (");
		printNoPrefix (prayerTime[i][0] * 60 + prayerTime[i][1], DEC);
		printNoPrefix (" / ");
		printNoPrefix (prayerTime[i][2], DEC);
		printlnNoPrefix (")");
		println ("");
	}

	print ("Timestamp: ");
	printlnNoPrefix (timestamp);

	printlnNoPrefix();
	print ("Setting Time to: ");
	printNoPrefix (strtol (timestamp, NULL, 10) + 3600 + 3600, DEC); // + 1h for the correct time in Paris
	printNoPrefix (" (");
	printNoPrefix (timestamp);
	printlnNoPrefix (" + 3600 + 3600)");

	setTime (strtol (timestamp, NULL, 10) + 3600 + 3600); // + 1h for the correct time in Paris

	println ("Time set  !");
} // getprayerTime
