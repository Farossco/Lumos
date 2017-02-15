#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <WiFiUdp.h>

#define BAUD_RATE     250000 // Serial baud rate
#define DEBUG_ENABLED true   // Debug mode

// Wi-Fi informations
#define SSID0 "Patatou"
#define PASS0 "FD00000000"

// Some IDs used for serial reception decrypt
#define TYPE_ERROR -1
#define TYPE_RGB   1
#define TYPE_ON    2
#define TYPE_POW   3
#define TYPE_MOD   4

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
time_t getNtpTime ();
void sendNTPpacket (IPAddress& address);
void getPrayerTime ();
void prayerRequest ();

// Wifi webserver
WiFiServer server (80);
WiFiClient client;

// Web server requests
int infoType;
String request;
byte buf[20];
long value;
boolean error;

// Serial requests
int n;
char messageChar[20];
String message;

// Prayer time request
DynamicJsonBuffer jsonBuffer;
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

		if (DEBUG_ENABLED) Serial.print (".");
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

	// Match the request
	if (request.indexOf ("/RGB=") != -1)
	{
		getRgb();
		infoType = TYPE_RGB;
	}
	else if (request.indexOf ("/ON=") != -1)
	{
		getOn();
		infoType = TYPE_ON;
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
		value    = -1;
		infoType = TYPE_ERROR;
	}

	if (value == -1)
	{
		if (DEBUG_ENABLED)
			Serial.println ("CODING ERROR !");
	}
	else
	{
		if (DEBUG_ENABLED)
			Serial.print ("\nSending to Arduino: ");

		// Print suffixe according to the type
		Serial.print
		(
			infoType == TYPE_RGB ? "RGB" :
			infoType == TYPE_ON ? "ON" :
			infoType == TYPE_POW ? "POW" :
			infoType == TYPE_MOD ? "MOD" : ""
		);

		// Now printing the correct vale in hexadecimal for RGB type and in decimal for any other type
		Serial.print (value, infoType == TYPE_RGB ? HEX : DEC);

		if (DEBUG_ENABLED)
			Serial.println();
	}

	// Return the response to the client
	client.print ("HTTP/1.1 ");
	client.println (value == -1 ? "400 FORMAT ERROR" : "200 OK");
	client.println ("Content-Type: text/html");
	client.println (""); // Do not forget this one

	if (value == -1)
		client.println ("Format error !");
	else
		client.println (value, infoType == TYPE_RGB ? HEX : DEC);
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

		for (int i = 0; i < 6; i++)
		{
			if (DEBUG_ENABLED)
			{
				Serial.print ("\nSending prayer time to arduino (");
				Serial.print (prayersName[i]);
				Serial.print (") : ");
				delay (1);
			}
			Serial.print ("PRT");
			Serial.print (prayersName[i][0]); // First letter of the prayer name
			Serial.print (prayerTime[i][2]);
			delay (1);
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
	value = 0;
	error = false;

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

	// Converting it to an integer
	for (int i = 9; i < 15; i++)
		value += buf[i] * pow (16, 5 - (i - 9));

	// [DEBUG] Printing the converted value
	if (DEBUG_ENABLED)
	{
		Serial.println();
		Serial.print ("\nRGB = ");
		Serial.println (value, HEX);
	}

	// If an error occured during the decoding, set the value to -1
	if (error)
		value = -1;
} // getRgb

// Decoding on/off request
void getOn ()
{
	request.getBytes (buf, 10);

	// Only reading byte at position 8 and converting it to an integer
	value = buf[8] - '0';

	// [DEBUG] Printing that value
	if (DEBUG_ENABLED)
	{
		Serial.println();
		Serial.print ("ON = ");
		Serial.println (value);
	}

	// if the value is not correct, set it to -1
	if ((value != 0) && (value != 1))
		value = -1;
}

// Decoding power request
void getPow ()
{
	value = 0;
	error = false;

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

	// Converting it to an integer
	for (int i = 9; i < 12; i++)
		value += buf[i] * pow (10, 2 - (i - 9));

	// [DEBUG] Printing the converted value
	if (DEBUG_ENABLED)
	{
		Serial.println();
		Serial.print ("\nPOW = ");
		Serial.println (value);
	}

	// If an error occured or the value is not between 0 and 100, set it to -1
	if ((value < 0) || (value > 100) || error)
		value = -1;
} // getPow

// Decoding mode request
void getMode ()
{
	request.getBytes (buf, 11);

	value = buf[9] - '0';

	if (DEBUG_ENABLED)
	{
		Serial.println();
		Serial.print ("Mode = ");
		Serial.println (value);
		Serial.print ("Mode (Text) = ");
		Serial.println (value == 0 ?
		  "DEFAULT" :
		  value == 1 ?
		  "FLASH" :
		  value == 2 ?
		  "STROBE" :
		  value == 3 ?
		  "FADE" :
		  value == 4 ? "SMOOTH" :
		  value == 5 ? "WAKEUP" :
		  "UNKNOWN");
	}

	if ((value < 0) || (value > 5)) value = -1;
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
	Serial.print ("\nConnecting to ");
	Serial.println (HOST);

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

	Serial.print ("\nJson: ");
	Serial.println (line);

	JsonObject& jsonRoot = jsonBuffer.parseObject (line);
	code      = jsonRoot["code"];
	status    = jsonRoot["status"];
	timestamp = jsonRoot["data"]["date"]["timestamp"];

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
		prayerTimeString[i] = jsonRoot["data"]["timings"][prayersName[i]];

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
