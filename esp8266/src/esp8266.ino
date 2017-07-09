#include "esp8266.h"

// ******* Constants ******* //
const int minSpeed[MODE_MAX + 1] = { 0, -100, -100, 70, 76, -1 }; // Minimum speed or power value for each mode
const int maxSpeed[MODE_MAX + 1] = { 100, 50, 50, 100, 100, -1 };


// ******* Arduino values ******* //
boolean on;                      // If the leds are ON or OFF (True: ON / False: OFF)
unsigned long rgb[MODE_MAX + 1]; // Currently displayed RGB value (From 0x000000 to 0xFFFFFF)
float power[MODE_MAX + 1];       // Current lightning power (from MINPOWER to MAXPOWER)
int speed[MODE_MAX + 1];         // Current mode speed
unsigned char mode;              // Current lighting mode

// Wifi webserver
WiFiServer server (80);
WiFiClient client;

// decode
long result;
int infoMode;
int infoType;
int errorType;

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

	sendTime();
} // setup

void loop ()
{
	readSerial();

	readWeb();
} // loop
