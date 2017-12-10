#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include "Utils.h"

// ************************************************************************** //
// ************************* Constants declarations ************************* //
// ************************************************************************** //

// **************************************** //
// **************** Global **************** //
// **************************************** //

const long ESP_BAUD_RATE   = 9600;   // ESP8266 communication baud rate
const long DEBUG_BAUD_RATE = 250000; // Debug baud rate
const long DFP_BAUD_RATE   = 9600;   // DFPlayer communication baud rate

const boolean WAIT_FOR_TIME         = true;  // If we have to wait for time sync (if true, program will not start until time is synced)
const boolean INFRARED_ENABLED      = true;  // If we allow infrared communication
const boolean SOUND_ENABLED         = false; // Enable sound
const boolean CLAP_ENABLED          = false; // Enable double claping
const boolean PRAYER_ALARM_ENABLED  = false; // Enable prayer alarms
const boolean MORNING_ALARM_ENABLED = true;  // Enable morning alarm

// Pins
const int PIN_SOUND        = 2;  // Sound detector IN pin
const int PIN_LED_INFRARED = 5;  // Infrared IN pin
const int PIN_LED_RED      = 11; // Red LED OUT pin
const int PIN_LED_GREEN    = 7;  // Green LED OUT pin
const int PIN_LED_BLUE     = 4;  // Blue LED OUT pin
const int PIN_SD_CS        = 49; // SD Chip Select

class Global
{
public:
	boolean on;                  // If the leds are ON or OFF (True: ON / False: OFF)
	boolean isInitialized;       // Set to true when program is complitely initialized
	unsigned long rgb[N_MODE];   // Current RGB value for each mode (From 0x000000 to 0xFFFFFF)
	int power[N_MODE];           // Current lightning power for each mode (from MINPOWER to MAXPOWER)
	int speed[N_MODE];           // Current mode speed for each mode
	unsigned char red[N_MODE];   // Current red value for each mode including lightning power (From 0 to 255)
	unsigned char green[N_MODE]; // Current green value for each mode including lightning power (From 0 to 255)
	unsigned char blue[N_MODE];  // Current blue value for each mode including lightning power (From 0 to 255)
	unsigned char mode;          // Current lighting mode (MODE_***)

	void begin ();
	void light ();     // Display the RGB value
	void rgb2color (); // Convert RGB value to Red, Green and Blue values
};

extern Global global;

#endif // ifndef GLOBAL_H
