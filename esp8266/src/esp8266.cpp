#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <WiFiUdp.h>

#define BAUD_RATE     250000 // Serial baud rate
#define DEBUG_ENABLED true   // Debug mode

// Wi-Fi informations
#define SSID0 "Patatou"
#define PASS0 "FD00000000"

// ******** IDs ******** //
/******** Serial reception types ********/
#define TYPE_UNK -1
#define TYPE_TIM 0
#define TYPE_RGB 1
#define TYPE_ONF 2
#define TYPE_POW 3
#define TYPE_MOD 4
#define TYPE_PRT 5

/******** Modes ********/
#define MODE_DEFAULT 0
#define MODE_FLASH   1
#define MODE_STROBE  2
#define MODE_FADE    3
#define MODE_SMOOTH  4
#define MODE_WAKEUP  5

#define MODE_MIN     0
#define MODE_MAX     5

// Prayer times
#define REQUEST_TIMEOUT 5000
#define HTTP_PORT       80
#define HOST            "api.aladhan.com"
#define LATITUDE        "48.866667"
#define LONGITUDE       "2.333333"
#define TIME_ZONE       "Europe/Paris"
#define METHOD          1

// Functions prototypes
void setup ();
void loop ();
void readWeb ();
void readSerial ();
void sendTime ();
void getRgb ();
void getOn ();
void getPow ();
void getMode ();
void digitalClockDisplay ();
void printDigits (int digits);
void getPrayerTime ();
void prayerRequest ();

// ******* Arduino values ******* //
boolean on;         // If the leds are ON or OFF (True: ON / False: OFF)
unsigned long rgb;  // Currently displayed RGB value (From 0x000000 to 0xFFFFFF)
float power;        // Current lightning power (from MINPOWER to MAXPOWER)
unsigned char mode; // Current lighting mode (MODE_***)

// Wifi webserver
WiFiServer server (80);
WiFiClient client;

// Web server requests
int infoType;
String request;
byte buf[20];
boolean error;
DynamicJsonBuffer jsonBuffer;

// Serial requests
int n;
char messageChar[20];
String message;

// Prayer time request
String line;
unsigned long timeout;
int code, prayerTime[6][3];
const char * json, * status, * timestamp, * prayerTimeString[6];
const char * prayersName[] = { "Fajr", "Sunrise", "Dhuhr", "Asr", "Maghrib", "Isha" };

void setup ()
{
	pinMode (0, OUTPUT);
	pinMode (2, OUTPUT);
	digitalWrite (0, LOW);
	digitalWrite (2, LOW);

	Serial.begin (BAUD_RATE);
	delay (10);

	// Connect to WiFi network
	if (DEBUG_ENABLED)
	{
		Serial.println();
		Serial.println();
		Serial.print ("Connecting to ");
		Serial.println (SSID0);
	}

	WiFi.begin (SSID0, PASS0);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay (500);

		if (DEBUG_ENABLED)
			Serial.print (".");
	}

	if (DEBUG_ENABLED)
	{
		Serial.println ("");
		Serial.println ("WiFi connected");
	}

	// Start the server
	server.begin();

	if (DEBUG_ENABLED)
		Serial.println ("Server started");
} // setup

void loop ()
{
	readSerial();

	readWeb();
} // loop

void sendJson (String status)
{
	JsonObject& jsonRootGlobal = jsonBuffer.createObject();

	jsonRootGlobal["STATUS"] = status;
	jsonRootGlobal["ON"]     = on;
	jsonRootGlobal["RGB"]    = rgb;
	jsonRootGlobal["POWER"]  = (int) power;
	jsonRootGlobal["MODE"]   = mode;
	jsonRootGlobal.printTo (client);
}

void readSerial ()
{
	// If nothing is comming, we stop here
	if (!Serial.available())
		return;

	message = "";

	for (n = 0; n < 20 && Serial.available(); n++)
	{
		messageChar[n] = Serial.read();
		delay (1);
	}

	// Converting char array into String
	for (int i = 0; i < n; i++)
		message += String (messageChar[i]);

	// If the Arduino is asking for time
	if (message.indexOf ("TIMEPLEASE") != -1)
	{
		if (DEBUG_ENABLED)
			Serial.println ("\nReceived time request from arduino");

		// We send the time to the Arduino
		sendTime();
	}
}

void readWeb ()
{
	// Check if a client has connected
	client = server.available();

	// If nobody connected, we stop here
	if (!client) return;

	// Wait until the client sends some data
	while (!client.available())
		delay (1);

	// Read the first line of the request
	request = client.readStringUntil ('\r');

	// [DEBUG] Printing the incomming request
	if (DEBUG_ENABLED)
	{
		Serial.println();
		Serial.println (request);
	}

	client.flush();
	error = false;

	// Match the request
	if (request.indexOf ("/RGB=") != -1)
	{
		getRgb();
		infoType = TYPE_RGB;
	}
	else if (request.indexOf ("/ONF=") != -1)
	{
		getOn();
		infoType = TYPE_ONF;
	}
	else if (request.indexOf ("/POW=") != -1)
	{
		getPow();
		infoType = TYPE_POW;
	}
	else if (request.indexOf ("/MOD=") != -1)
	{
		getMode();
		infoType = TYPE_MOD;
	}
	else
	{
		error    = true;
		infoType = TYPE_UNK;
	}

	if (error)
	{
		if (DEBUG_ENABLED)
			Serial.println ("CODING ERROR !");
	}
	else
	{
		if (DEBUG_ENABLED)
			Serial.print ("\nSending to Arduino: ");

		// Now printing the correct vale in hexadecimal for RGB type and in decimal for any other type
		switch (infoType)
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

		if (DEBUG_ENABLED)
			Serial.println();
	}

	// Return the response to the client
	client.print ("HTTP/1.1 ");
	client.println (error ? "400 FORMAT ERROR" : "200 OK");
	client.println ("Content-Type: text/html");
	client.println (""); // Do not forget this one

	sendJson (error ? "ERROR" : "OK");
} // readWeb

void sendTime ()
{
	getPrayerTime();

	if (DEBUG_ENABLED)
	{
		Serial.print ("\nTime is: ");
		digitalClockDisplay();
	}

	if (timeStatus() != timeNotSet)
	{
		if (DEBUG_ENABLED)
		{
			Serial.print ("\nSending time to arduino: ");
			delay (1);
		}

		Serial.print ("TIM");
		Serial.print (now());
		Serial.print ('z');

		for (int i = 0; i < 6; i++)
		{
			if (DEBUG_ENABLED)
			{
				Serial.print ("\nSending prayer time to arduino (");
				Serial.print (prayersName[i]);
				Serial.print (") : ");
				delay (1);
			}
			delay (100);
			Serial.print ("PRT");
			Serial.print (prayersName[i][0]); // First letter of the prayer name
			Serial.print (prayerTime[i][2]);
			Serial.print ('z');
		}
	}
	else if (DEBUG_ENABLED)
	{
		Serial.print ("\nTime not set, not sending time to arduino...");
	}

	if (DEBUG_ENABLED)
		Serial.println();
} // sendTime

// Decoding RGB request
void getRgb ()
{
	request.getBytes (buf, 16);

	// [DEBUG] Printing the incomming value ascii code
	if (DEBUG_ENABLED)
	{
		for (int i = 9; i < 15; i++)
		{
			Serial.print (buf[i], HEX);
			Serial.print (" / ");
		}
		Serial.println ("\n ----------- ");
	}

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
		if (DEBUG_ENABLED)
		{
			Serial.print (buf[i], HEX);
			Serial.print (" / ");
		}
	}

	rgb = 0;
	// Converting it to an integer
	for (int i = 9; i < 15; i++)
		rgb += buf[i] * pow (16, 5 - (i - 9));

	// [DEBUG] Printing the converted value
	if (DEBUG_ENABLED)
	{
		Serial.println();
		Serial.print ("\nRGB = ");
		Serial.println (rgb, HEX);
	}
} // getRgb

// Decoding on/off request
void getOn ()
{
	request.getBytes (buf, 10);

	// Only reading byte at position 8 and converting it to an integer
	on = buf[8] - '0';

	// [DEBUG] Printing that value
	if (DEBUG_ENABLED)
	{
		Serial.println();
		Serial.print ("ONF = ");
		Serial.println (on);
	}

	// if the value is not correct, set it to -1
	if (on != 0 && on != 1)
		error = true;
}

// Decoding power request
void getPow ()
{
	request.getBytes (buf, 13);

	// [DEBUG] Printing the incomming value ascii code
	if (DEBUG_ENABLED)
	{
		for (int i = 9; i < 12; i++)
		{
			Serial.print (buf[i], HEX);
			Serial.print (" / ");
		}
		Serial.println ("\n ----------- ");
	}

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

		if (DEBUG_ENABLED)
		{
			Serial.print (buf[i], DEC);
			Serial.print (" / ");
		}
	}

	power = 0;
	// Converting it to an integer
	for (int i = 9; i < 12; i++)
		power += buf[i] * pow (10, 2 - (i - 9));

	// [DEBUG] Printing the converted value
	if (DEBUG_ENABLED)
	{
		Serial.println();
		Serial.print ("\nPOW = ");
		Serial.println (power);
	}

	// If an error occured or the value is not between 0 and 100, set it to -1
	if (power < 0 || power > 100)
		error = true;
} // getPow

// Decoding mode request
void getMode ()
{
	request.getBytes (buf, 11);

	mode = buf[9] - '0';

	if (DEBUG_ENABLED)
	{
		Serial.println();
		Serial.print ("Mode = ");
		Serial.println (mode);
		Serial.print ("Mode (Text) = ");
		Serial.println
		(
			mode == 0 ? "DEFAULT" :
			mode == 1 ? "FLASH" :
			mode == 2 ? "STROBE" :
			mode == 3 ? "FADE" :
			mode == 4 ? "SMOOTH" :
			mode == 5 ? "WAKEUP" :
			"UNKNOWN"
		);
	}

	if (mode < MODE_MIN || mode > MODE_MAX)
		error = true;
}

// Digital clock display of the time
void digitalClockDisplay ()
{
	printDigits (day());
	Serial.print ("/");
	printDigits (month());
	Serial.print ("/");
	Serial.print (year());

	Serial.print (" ");

	printDigits (hour());
	Serial.print (":");
	printDigits (minute());
	Serial.print (":");
	printDigits (second());

	Serial.println();
}

// Utility for digital clock display: prints preceding colon and leading 0
void printDigits (int digits)
{
	if (digits < 10)
		Serial.print ('0');
	Serial.print (digits);
}

void getPrayerTime ()
{
	if (DEBUG_ENABLED)
	{
		Serial.print ("\nConnecting to ");
		Serial.println (HOST);
	}

	// Use WiFiClient class to create TCP connections

	if (!client.connect (HOST, HTTP_PORT))
	{
		Serial.println ("\nConnection failed");
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
	if (DEBUG_ENABLED)
	{
		Serial.print ("\nRequesting URL: ");
		Serial.println (url);
		Serial.println();
	}

	// Sending web request
	client.print (String ("GET ") + url + " HTTP/1.1\r\n" + "Host: " + HOST + "\r\n" + "Connection: close\r\n\r\n");
	timeout = millis();

	// Waiting for an answer
	while (!client.available())
	{
		// If nothing come out after the timeout, we abort
		if (millis() - timeout > REQUEST_TIMEOUT)
		{
			if (DEBUG_ENABLED)
				Serial.println ("Client Timeout !");
			client.stop();
			return;
		}
	}

	if (DEBUG_ENABLED)
	{
		Serial.println ("Server answer:\n\n==================== Start ====================");
	}

	// Read all the lines of the reply from server and print them to Serial
	while (client.available())
	{
		line = client.readStringUntil ('\r');
		// [DEBUG] We print the line we're currently reading
		if (DEBUG_ENABLED)
			Serial.print (line);
	}

	if (DEBUG_ENABLED)
	{
		Serial.println ("\n===================== End =====================\n");
	}

	if (line.indexOf ("{\"code") == 1)
	{
		// The first charactere is a nl, so we don't want it
		line = line.substring (1);
		if (DEBUG_ENABLED)
			Serial.println ("Success !");
	}
	else if (DEBUG_ENABLED)
	{
		Serial.print ("Failed ! (");
		Serial.print (line.indexOf ("{\"code"));
		Serial.println (")");
	}

	if (DEBUG_ENABLED)
	{
		Serial.println();
		Serial.println ("Closing connection");
	}

	// At this point, the last line of the answer is in the line variable,
	// that's actually the one we want

	if (DEBUG_ENABLED)
	{
		Serial.print ("\nJson: ");
		Serial.println (line);
	}

	JsonObject& jsonRootPrayer = jsonBuffer.parseObject (line);
	code      = jsonRootPrayer["code"];
	status    = jsonRootPrayer["status"];
	timestamp = jsonRootPrayer["data"]["date"]["timestamp"];

	if (DEBUG_ENABLED)
	{
		Serial.print ("\nCode: ");
		Serial.println (code);

		Serial.print ("Status: ");
		Serial.println (status);
		Serial.println();
	}

	if (code != 200 && status != "OK")
	{
		if (DEBUG_ENABLED)
			Serial.println ("Wrong status, leaving.");
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

	if (DEBUG_ENABLED)
	{
		for (int i = 0; i < 6; i++)
		{
			Serial.print (prayersName[i]);
			Serial.print (": ");
			Serial.println (prayerTimeString[i]);
			Serial.print (prayersName[i]);
			Serial.print (" (Integer): ");
			Serial.print (prayerTime[i][0]);
			Serial.print (":");
			Serial.print (prayerTime[i][1]);
			Serial.print (" (");
			Serial.print (prayerTime[i][0] * 60 + prayerTime[i][1]);
			Serial.print (" / ");
			Serial.print (prayerTime[i][2]);
			Serial.print (")");
			Serial.println ("\n");
		}

		Serial.print ("Timestamp: ");
		Serial.println (timestamp);
	}

	if (DEBUG_ENABLED)
	{
		Serial.print ("\nSetting Time to: ");
		Serial.print (strtol (timestamp, NULL, 10) + 3600); // + 1h for the correct time in Paris
		Serial.print (" (");
		Serial.print (timestamp);
		Serial.println (" + 3600)");
	}

	setTime (strtol (timestamp, NULL, 10) + 3600); // + 1h for the correct time in Paris
} // getprayerTime
