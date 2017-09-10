#ifndef ARDUINO_H
#define ARDUINO_H

#include <IRremote.h>
#include <Time.h>
#include <DFRobotDFPlayerMini.h>
#include <EEPROM.h>
#include <SD.h>

// ************************************************************************** //
// ************************* Constants declarations ************************* //
// ************************************************************************** //

const boolean DEBUG_ENABLED = true; // Debug Mode

const long ESP_BAUD_RATE   = 74880;  // ESP8266 communication baud rate
const long DEBUG_BAUD_RATE = 250000; // Debug baud rate
const long DFP_BAUD_RATE   = 9600;   // DFPlayer communication baud rate

const boolean WAIT_FOR_TIME    = true;  // If we have to wait for time sync (if true, program will not start until time is synced)
const boolean INFRARED_ENABLED = true;  // If we allow infrared communication
const boolean SOUND_ENABLED    = false; // Enable sound
const boolean CLAP_ENABLED     = false; // Enable double claping

// Pins
const int PIN_SOUND        = 2;  // Sound detector IN pin
const int PIN_LED_INFRARED = 5;  // Infrared IN pin
const int PIN_LED_RED      = 4;  // Red LED OUT pin
const int PIN_LED_GREEN    = 7;  // Green LED OUT pin
const int PIN_LED_BLUE     = 11; // Blue LED OUT pin

// Wake up
const int WAKEUP_HOURS   = 06;
const int WAKEUP_MINUTES = 00;
const int WAKEUP_SECONDS = 00;

// Prayer
const int PRAYER_FADE_SPEED         = 97; // Fade speed for prayer time
const int N_PRAYER                  = 6;  // Number of different prayer (including sunrise)
const String PRAYERS_NAME[N_PRAYER] = { "Fajr", "Sunrise", "Dhuhr", "Asr", "Maghrib", "Isha" };

// EEPROM
const int EEPROM_START = 0x0000; // EEPROM start address

// ******** IDs ******** //
// Serial reception types
const int TYPE_RTM = -2; // Request : Time
const int TYPE_RIF = -1; // Request : Info
const int TYPE_UNK = 0;  // Unknown type
const int TYPE_TIM = 1;  // Provide : Time
const int TYPE_RGB = 2;  // Provide : RGB
const int TYPE_ONF = 3;  // Provide : On
const int TYPE_POW = 4;  // Provide : Power
const int TYPE_MOD = 5;  // Provide : Mode
const int TYPE_PRT = 6;  // Provide : Prayer time
const int TYPE_SPE = 7;  // Provide : Speed

// Serial reception errors
const int ERR_NOE = 0; // No error
const int ERR_OOB = 1; // Out of bound
const int ERR_UKM = 2; // Unknown mode
const int ERR_UKR = 3; // Unknown request
const int ERR_UKP = 4; // Unknown prayer

// Modes
const int MODE_MIN     = 0; // -Minimum mode value-
const int MODE_DEFAULT = 0; // Default mode
const int MODE_FLASH   = 1; // Flash mode
const int MODE_STROBE  = 2; // Strobe mode
const int MODE_FADE    = 3; // Fade mode
const int MODE_SMOOTH  = 4; // Smooth mode
const int MODE_WAKEUP  = 5; // Wake up mode
const int MODE_MAX     = 5; // -Maximum mode value-
const int N_MODE       = 6; // --Number of different modes--

// Caps
const int CAPS_NONE  = 0; // All letters in lower case
const int CAPS_FIRST = 1; // First letter in upper case
const int CAPS_ALL   = 2; // All letters in upper case

// Default values
const unsigned long DEFAULT_RGB[N_MODE] =
{ 0xFFFFFF, 0xFF0000, 0xFFFFFF, 0xFFFFFF, 0xFF0000, 0x0000FF };    // Default color on program startup
const float DEFAULT_POWER[N_MODE] = { 50, 100, 100, 100, 100, 0 }; // Default power on program startup
const int DEFAULT_SPEED[N_MODE] = { -1, 0, 0, 750, 800, 2000 };    // Default speed on program startup

// Bounds
const int MIN_SPEED[N_MODE] = { -1, 1, 1, 50, 10, 1 };         // Minimum speed or power value for each mode
const int MAX_SPEED[N_MODE] = { -1, 25, 25, 600, 1000, 1000 }; // Maximum speed or power value for each mode
const int MIN_POWER   = 0;                                     // Minimum power value
const int MAX_POWER   = 100;                                   // Maximum power value
const int SEEKBAR_MIN = 0;                                     // Minimum app seek bars value
const int SEEKBAR_MAX = 100;                                   // Maximum app seek bars value

// Infrared
const int N_COLOR        = 16; // Number of different colors
const int IR_CHANGE_STEP = 5;  // increasion or decreasion step value for infrared
const int MIN_IR_POWER   = 5;  // Minimum value in wich the power can go with infrared

// DFPlayer
const int DEFAULT_VOLUME = 30;

// Sound
const int MIN_CLAP_DURATION          = 30;   // Amount of time to ignore after clap started (To avoid sound bounce)
const int MAX_CLAP_DURATION          = 100;  // Maximum clap duration in ms
const int MIN_TIME_BEETWIN_TWO_CLAPS = 400;  // Minimum time before starting second clap
const int MAX_TIME_BEETWIN_TWO_CLAPS = 600;  // Maximum time to start second clap
const int TIME_AFTER_START_OVER      = 1000; // Time to wait after double clap to start over

// ************************************************************************** //
// ************************* Variables declarations ************************* //
// ************************************************************************** //

// Global
boolean on;                  // If the leds are ON or OFF (True: ON / False: OFF)
boolean isInitialized;       // Set to true when program is complitely initialized
unsigned long rgb[N_MODE];   // Current RGB value for each mode (From 0x000000 to 0xFFFFFF)
int power[N_MODE];           // Current lightning power for each mode (from MINPOWER to MAXPOWER)
int speed[N_MODE];           // Current mode speed for each mode
unsigned char red[N_MODE];   // Current red value for each mode including lightning power (From 0 to 255)
unsigned char green[N_MODE]; // Current green value for each mode including lightning power (From 0 to 255)
unsigned char blue[N_MODE];  // Current blue value for each mode including lightning power (From 0 to 255)
unsigned char mode;          // Current lighting mode (MODE_***)

// Prayer
int prayerTime[N_PRAYER][3]; // [0] = Hours / [1] = Minutes / [2] = Hours & Minutes

// Read Claps
int clapState;              // Same as "state" but for claps
unsigned long endStateTime; // Time position at the end of a state (Allow time counting)

// Sound object declaration
DFRobotDFPlayerMini myDFPlayer;

// Infrared reception objects declaration
IRrecv irrecv (PIN_LED_INFRARED);
decode_results results;

// Mode
int state;              // Current state used by some modes
unsigned long count;    // Delay counting
unsigned char lastMode; // Mode in previous loop - Allows mode initializations

// ******* Time Alarms ******* //
// Prayer
int flagEnter; // If we have to start fading
int flagLeave; // If we have to stop fading
int faded;     // If we already gave the order to start fading
int unfaded;   // If we already gave the order to stop fading
// Wake up
int wokeUp;    // If we already gave the order to wake up
int timeAsked; // If we already asked for time
// Global
int i, j; // Just counting variables

// Variable Change
boolean changeOn;
unsigned long changeRgb;
int changePower[N_MODE];
int changeSpeed[N_MODE];
unsigned char changeMode;


// ************************************************************************** //
// ************************** Functions prototypes ************************** //
// ************************************************************************** //

void setup ();
void loop ();
void initGlobal ();
void initDFPlayer ();
void eepromDump (unsigned int start, unsigned int limit);
void eepromWrite ();
boolean eepromRead ();
void readClaps ();
void printPrefix ();
void debugPrintDigits (int digits);
void debugClockDisplay ();
size_t print (const __FlashStringHelper * message);
size_t print (const String & message);
size_t print (const char * message);
size_t print (unsigned char message, int base);
size_t print (int message, int base);
size_t print (unsigned int message, int base);
size_t print (long message, int base);
size_t print (unsigned long message, int base);
size_t print (double message, int base);
size_t print (const Printable & message);
size_t printNoPrefix (const __FlashStringHelper * message);
size_t printNoPrefix (const String & message);
size_t printNoPrefix (const char * message);
size_t printNoPrefix (unsigned char message, int base);
size_t printNoPrefix (int message, int base);
size_t printNoPrefix (unsigned int message, int base);
size_t printNoPrefix (long message, int base);
size_t printNoPrefix (unsigned long message, int base);
size_t printNoPrefix (double message, int base);
size_t printNoPrefix (const Printable & message);
size_t println ();
size_t println (const __FlashStringHelper * message);
size_t println (const String & message);
size_t println (const char * message);
size_t println (unsigned char message, int base);
size_t println (int message, int base);
size_t println (unsigned int message, int base);
size_t println (long message, int base);
size_t println (unsigned long message, int base);
size_t println (double message, int base);
size_t println (const Printable & message);
size_t printlnNoPrefix ();
size_t printlnNoPrefix (const __FlashStringHelper * message);
size_t printlnNoPrefix (const String & message);
size_t printlnNoPrefix (const char * message);
size_t printlnNoPrefix (unsigned char message, int base);
size_t printlnNoPrefix (int message, int base);
size_t printlnNoPrefix (unsigned int message, int base);
size_t printlnNoPrefix (long message, int base);
size_t printlnNoPrefix (unsigned long message, int base);
size_t printlnNoPrefix (double message, int base);
size_t printlnNoPrefix (const Printable & message);
void decodeRequest (String request, long * pResult, int * pInfoMode, int * pInfoType, int * pErrorType);
void initSerial ();
void waitForTime ();
void askForTime ();
void readSerial ();
void initInfrared ();
void readInfrared ();
void light ();
void rgb2color ();
void initModes ();
void action ();
void initModeFlash ();
void modeFlash ();
void initModeStrobe ();
void modeStrobe ();
void initModeFade ();
void modeFade ();
void initModeSmooth ();
void modeSmooth ();
void initModeWakeup ();
void modeWakeup ();
void testPrayerTime ();
void prayerStart ();
void prayerStop ();
void testWakeUpTime ();
void peakTime ();
String modeName (int mode, int caps);
String infoTypeName (int infoType, boolean shortened);
String errorTypeName (int infoType, boolean shortened);
void digitalClockDisplay ();
void printDigits (int digits);
void softwareReset ();
void testVariableChange ();
void initVariableChange ();
void sendInfo ();

#endif // ifndef ARDUINO_H
