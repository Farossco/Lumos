#include "head.h"

unsigned long defaultRgb[MODE_MAX + 1] = { 0xFFFFFF, 0xFF0000, 0xFFFFFF, 0xFFFFFF, 0xFF0000, 0x0000FF }; // Default color on program startup
float defaultPower[MODE_MAX + 1] = { 50, 100, 100, 0, 100, 0 };                                          // Default color on program startup

// ******* Global ******* //
boolean on;                        // If the leds are ON or OFF (True: ON / False: OFF)
boolean isInitialized;             // Set to true when program is complitely initialized
unsigned long rgb[MODE_MAX + 1];   // Currently displayed RGB value (From 0x000000 to 0xFFFFFF)
float power[MODE_MAX + 1];         // Current lightning power (from MINPOWER to MAXPOWER)
unsigned char red[MODE_MAX + 1];   // Currentlty red value including lightning power (From 0 to 255)
unsigned char green[MODE_MAX + 1]; // Currentlty green value including lightning power (From 0 to 255)
unsigned char blue[MODE_MAX + 1];  // Currentlty blue value including lightning power (From 0 to 255)
unsigned char mode;                // Current lighting mode (MODE_***)

// ******* Modes ******* //
/******* modeFlash ********/
int flashSpeed; // Current flash speed (From MINFLASH to MAXFLASH)
/******* modeStrobe *******/
int strobeSpeed; // Current strobe speed (From MINSTROBE to MAXSTROBE)
/******* modeFade *******/
int fadeSpeed; // Current fade speed (From MINFADE to MAXFADE)
/******* modeSmooth *******/
int smoothSpeed; // Current smooth speed (From MINSMOOTH to MAXSMOOTH)

// ******* Prayer ******* //
String prayersName[N_PRAYER] = { "Fajr", "Sunrise", "Dhuhr", "Asr", "Maghrib", "Isha" };
int prayerTime[N_PRAYER][3]; // [0] = Hours / [1] = Minutes / [2] = Hours & Minutes

void setup ()
{
	isInitialized = false;

	initSerial(); // Initialize serial communications

	println ("Starting program...\n");

	initInfrared(); // Initialize infrared reception

	initDFPlayer(); // Initialize DFPlayer communication

	initModes(); // Initialize default values for modes variables

	initGlobal(); // Initialize defaut values for global variables

	initVariableChange(); // Initialize defaut values for change variables

	waitForTime(); // Waiting for the ESP to send time (if WAIT_FOR_TIME)

	sendInfo(); // Sending global variables informations to the ESP8266

	println ("Program started!\n");

	isInitialized = true;
} // setup

void loop ()
{
	testVariableChange(); // Perform action at each variation of one of the global variables

	peakTime(); // Perform action every peak time

	readClaps(); // Lighting on double claps

	onPrayerTime(); // Perform action at prayer time

	readInfrared(); // Read the in-comming IR signal if present

	readSerial(); // Receive datas from ESP8266 for Wi-Wi control

	action(); // Do something according to current mode

	light(); // Finaly display the RGB value
}

void initGlobal ()
{
	// Initializing to default values
	on = false; // LEDs are off on startup
	for (int i = 0; i < MODE_MAX + 1; i++)
	{
		rgb[i]   = defaultRgb[i];   // Initialize default mode color to its default value
		power[i] = defaultPower[i]; // Initializing default mode power its default value
	}
	mode = MODE_DEFAULT; // Initialize mode to constant lightning
}
