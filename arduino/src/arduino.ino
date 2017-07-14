#include "arduino.h"

void setup ()
{
	isInitialized = false;

	initSerial(); // Initialize serial communications

	println ("Starting program...");

	initInfrared(); // Initialize infrared reception

	initDFPlayer(); // Initialize DFPlayer communication

	initModes(); // Initialize default values for modes variables

	waitForTime(); // Waiting for the ESP to send time (if WAIT_FOR_TIME)

	initGlobal(); // Initialize defaut values for global variables

	initVariableChange(); // Initialize defaut values for change variables

	sendInfo(); // Sending global variables informations to the ESP8266

	printlnNoPrefix();
	println ("Program started!");

	isInitialized = true;
} // setup

void loop ()
{
	testVariableChange(); // Perform action at each variation of one of the global variables

	peakTime(); // Perform action every peak time

	readClaps(); // Lighting on double claps

	testPrayerTime(); // Perform action at prayer time

	testWakeUpTime(); // Perform action at wake up time

	readInfrared(); // Read the in-comming IR signal if present

	readSerial(); // Receive datas from ESP8266 for Wi-Wi control

	action(); // Do something according to current mode

	light(); // Finaly display the RGB value
}

void initGlobal ()
{
	if (eepromRead()) // Returns True if EEPROM is not correctly initialized (This may be the first launch)
	{
		printlnNoPrefix();
		println ("Setting variables to their default value");

		for (int i = MODE_MIN; i < N_MODE; i++)
		{
			rgb[i]   = DEFAULT_RGB[i];   // Initialize colors to their default value
			power[i] = DEFAULT_POWER[i]; // Initializing powers their default value
			speed[i] = DEFAULT_SPEED[i]; // Initializing speeds their default value
		}
	}
	else
	{
		for (int i = MODE_FLASH; i < N_MODE; i++)
			rgb[i] = DEFAULT_RGB[i];  // Initialize colors to their default values (Starting from flash mode)
	}
	on = false; // LEDs are off on startup

	mode = MODE_DEFAULT; // Initialize mode to constant lightning
}
