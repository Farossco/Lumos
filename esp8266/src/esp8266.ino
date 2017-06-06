#include "esp8266.h"

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
String line, printedLine;
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
	printlnNoPrefix();
	printlnNoPrefix();
	print ("Connecting to ");
	printlnNoPrefix (SSID0);

	WiFi.begin (SSID0, PASS0);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay (500);
		printNoPrefix (".");
	}

	printlnNoPrefix();
	println ("WiFi connected");

	// Start the server
	server.begin();

	println ("Server started");

	getPrayerTime();
} // setup

void loop ()
{
	readSerial();

	readWeb();
} // loop
