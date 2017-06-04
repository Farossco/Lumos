#include "head.h"

// ******* Global ******* //
boolean on;            // If the leds are ON or OFF (True: ON / False: OFF)
boolean isInitialized; // Set to true when program is complitely initialized
unsigned long rgb;     // Currently displayed RGB value (From 0x000000 to 0xFFFFFF)
float power;           // Current lightning power (from MINPOWER to MAXPOWER)
unsigned char red;     // Currentlty red value including lightning power (From 0 to 255)
unsigned char green;   // Currentlty green value including lightning power (From 0 to 255)
unsigned char blue;    // Currentlty blue value including lightning power (From 0 to 255)

// ******* Modes ******* //
unsigned char mode; // Current lighting mode (MODE_***)
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

	waitForTime(); // Waiting for the ESP to send time (if WAIT_FOR_TIME)

	initTimeAlarms(); // Initialize time alarms

	println ("Program started!\n");

	isInitialized = true;
} // setup

void loop ()
{
	readClaps(); // Lighting on double claps

	readInfrared(); // Read the in-comming IR signal if present

	readSerial(); // Receive datas from ESP8266 for Wi-Wi control

	action(); // Do something according to current mode

	light(); // Finaly display the RGB value
}

void initGlobal ()
{
	// Initializing to default values
	on    = false;         // LEDs are off on startup
	rgb   = DEFAULT_COLOR; // Initialize color to white
	mode  = MODE_DEFAULT;  // Initialize mode to constant lightning
	power = DEFAULT_POWER; // Initializing power its default value
}
