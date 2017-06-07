#include "esp8266.h"

// ******* Arduino values ******* //
boolean on;         // If the leds are ON or OFF (True: ON / False: OFF)
unsigned long rgb;  // Currently displayed RGB value (From 0x000000 to 0xFFFFFF)
float power;        // Current lightning power (from MINPOWER to MAXPOWER)
unsigned char mode; // Current lighting mode (MODE_***)

// Wifi webserver
WiFiServer server (80);
WiFiClient client;

// json
DynamicJsonBuffer jsonBuffer;

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

	getTime();

	getPrayerTime();
} // setup

void loop ()
{
	readSerial();

	readWeb();
} // loop
