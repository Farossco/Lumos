#include "arduino.h"

void setup ()
{
	isInitialized = false;

	initSerial(); // Initialize serial communications

	println (LEVEL_INFO, "Starting program...");

	waitForTime(); // Waiting for the ESP to send time (if WAIT_FOR_TIME)

	initSdCard(); // Start SD logging

	initInfrared(); // Initialize infrared reception

	initDFPlayer(); // Initialize DFPlayer communication

	initModes(); // Initialize default values for modes variables

	initGlobal(); // Initialize defaut values for global variables

	initVariableChange(); // Initialize defaut values for change variables

	sendInfo(); // Sending global variables informations to the ESP8266

	println (LEVEL_INFO, false);
	println (LEVEL_INFO, "Program started!");

	isInitialized = true;
} // setup

void loop ()
{
	Alarm.delay (0); // Needed for timeAlarms to work

	testVariableChange(); // Perform action at each variation of one of the global variables

	readClaps(); // Lighting on double claps

	readInfrared(); // Read the in-comming IR signal if present

	readSerial(); // Receive datas from ESP8266 for Wi-Wi control

	action(); // Do something according to current mode

	light(); // Finaly display the RGB value
}

void initGlobal ()
{
	if (eepromRead()) // Returns True if EEPROM is not correctly initialized (This may be the first launch)
	{
		println (LEVEL_INFO, false);
		println (LEVEL_INFO, "This is first launch, variables will be initialized to their default values");

		for (int i = MODE_MIN; i < N_MODE; i++)
		{
			rgb[i]   = DEFAULT_RGB[i];   // Initialize colors to their default value
			power[i] = DEFAULT_POWER[i]; // Initializing powers their default value
			speed[i] = DEFAULT_SPEED[i]; // Initializing speeds their default value
		}
		eepromWrite();
	}
	else
	{
		for (int i = MODE_FLASH; i < N_MODE; i++)
			rgb[i] = DEFAULT_RGB[i];  // Initialize colors to their default values (Starting from flash mode)
	}
}
