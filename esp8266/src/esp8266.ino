#include "esp8266.h"

// ******* Constants ******* //
const int minSpeed[MODE_MAX + 1] = { 0, -100, -100, 70, 76, -1 }; // Minimum speed or power value for each mode
const int maxSpeed[MODE_MAX + 1] = { 100, 50, 50, 100, 100, -1 };

// ******* Arduino values ******* //
boolean on;                // If the leds are ON or OFF (True: ON / False: OFF)
unsigned long rgb;         // Currently displayed RGB value (From 0x000000 to 0xFFFFFF)
unsigned char red;         // Current red value including lightning power (From 0 to 255)
unsigned char green;       // Current green value including lightning power (From 0 to 255)
unsigned char blue;        // Current blue value including lightning power (From 0 to 255)
float power[MODE_MAX + 1]; // Current lightning power (from MINPOWER to MAXPOWER)
int speed[MODE_MAX + 1];   // Current mode speed
unsigned char mode;        // Current lighting mode

// ******* Prayer ******* //
const String prayersName[N_PRAYER] = { "Fajr", "Sunrise", "Dhuhr", "Asr", "Maghrib", "Isha" };
int prayerTime[N_PRAYER][3]; // [0] = Hours / [1] = Minutes / [2] = Hours & Minutes
String line;
String printedLine;
unsigned long timeout;
int code;
boolean prayersAreSet;
const char * json, * status, * timestamp, * prayerTimeString[6];

// ******* Wifi webserver ******* //
WiFiServer server (80);

// ******* Decode ******* //
long result;
int infoMode;
int infoType;
int errorType;

void setup ()
{
	initGpios();

	initSerial();

	initWifiServer();

	sendTime();
}

void loop ()
{
	readSerial();

	readWeb();
}

void initGpios ()
{
	pinMode (0, OUTPUT);
	pinMode (2, OUTPUT);
	digitalWrite (0, LOW);
	digitalWrite (2, LOW);
}
