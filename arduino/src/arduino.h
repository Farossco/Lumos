#ifndef head_h
#define head_h

#include <IRremote.h>
#include <Time.h>
#include <DFRobotDFPlayerMini.h>
#include <EEPROM.h>

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

// ******* Pins ******* //
const int PIN_SOUND        = 2;  // Sound detector IN pin
const int PIN_LED_INFRARED = 5;  // Infrared IN pin
const int PIN_LED_RED      = 4;  // Red LED OUT pin
const int PIN_LED_GREEN    = 7;  // Green LED OUT pin
const int PIN_LED_BLUE     = 11; // Blue LED OUT pin

// ******* Wake up ******* //
const int WAKEUP_HOURS   = 6;
const int WAKEUP_MINUTES = 15;
const int WAKEUP_SECONDS = 00;

// ******* Prayer ******* //
const int PRAYER_FADE_SPEED = 97; // Fade speed for prayer time

// ******* EEPROM ******* //
const int EEPROM_START = 0x0000; // EEPROM address start

// ******** IDs ******** //
/******** Serial reception types ********/
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

/******** Serial reception errors ********/
const int ERR_NOE = 0; // No error
const int ERR_OOB = 1; // Out of bound
const int ERR_UKM = 2; // Unknown mode
const int ERR_UKR = 3; // Unknown request

/******** Modes ********/
const int MODE_MIN     = 0;
const int MODE_DEFAULT = 0;
const int MODE_FLASH   = 1;
const int MODE_STROBE  = 2;
const int MODE_FADE    = 3;
const int MODE_SMOOTH  = 4;
const int MODE_WAKEUP  = 5;
const int MODE_MAX     = 5;

// ******* Infrared ******* //
const int N_COLOR = 16; // Number of different colors

// ******* DFPlayer ******* //
const int DEFAULT_VOLUME = 30;

// ******** Sound ******** //
const int MIN_CLAP_DURATION = 30;  // Amount of time to ignore after clap started (To avoid sound bounce)
const int MAX_CLAP_DURATION = 100; // Maximum clap duration in ms

const int MIN_TIME_BEETWIN_TWO_CLAPS = 400; // Minimum time before starting second clap
const int MAX_TIME_BEETWIN_TWO_CLAPS = 600; // Maximum time to start second clap

const int TIME_AFTER_START_OVER = 1000; // Time to wait after double clap to start over

const int N_PRAYER = 6; // Number of different prayer (including sunrise)

const unsigned long defaultRgb[MODE_MAX + 1] =
{ 0xFFFFFF, 0xFF0000, 0xFFFFFF, 0xFFFFFF, 0xFF0000, 0x0000FF };         // Default color on program startup
const float defaultPower[MODE_MAX + 1] = { 50, 100, 100, 100, 100, 0 }; // Default power on program startup
const int defaultSpeed[MODE_MAX + 1] = { -1, 0, 0, 75, 80, 200 };       // Default speed on program startup
const int minSpeed[MODE_MAX + 1] = { 0, -100, -100, 70, 76, 0 };        // Minimum speed or power value for each mode
const int maxSpeed[MODE_MAX + 1] = { 100, 50, 50, 100, 100, 2000 };     // Maximum speed or power value for each mode
const int IRChandeStep[MODE_MAX + 1] = { 5, 1, 1, 1, 1, -1 };           // increasion or decreasion step value for infrared for each mode for each mode

// ************************************************************************** //
// ************************* Variables declarations ************************* //
// ************************************************************************** //

// Global
boolean on;                        // If the leds are ON or OFF (True: ON / False: OFF)
boolean isInitialized;             // Set to true when program is complitely initialized
unsigned long rgb[MODE_MAX + 1];   // Current RGB value for each mode (From 0x000000 to 0xFFFFFF)
float power[MODE_MAX + 1];         // Current lightning power for each mode (from MINPOWER to MAXPOWER)
unsigned char red[MODE_MAX + 1];   // Current red value for each mode including lightning power (From 0 to 255)
unsigned char green[MODE_MAX + 1]; // Current green value for each mode including lightning power (From 0 to 255)
unsigned char blue[MODE_MAX + 1];  // Current blue value for each mode including lightning power (From 0 to 255)
int speed[MODE_MAX + 1];           // Current mode speed for each mode
unsigned char mode;                // Current lighting mode (MODE_***)

// Decode
long result;
int infoMode;
int infoType;
int errorType;

// Prayer
const String prayersName[N_PRAYER] = { "Fajr", "Sunrise", "Dhuhr", "Asr", "Maghrib", "Isha" };
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
int state;    // Current state used by some modes
int count;    // Delay counting
int lastMode; // Mode in previous loop - Allows mode initializations

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
float changePower[MODE_MAX + 1];
int changeSpeed[MODE_MAX + 1];
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
void decodeRequest (String request);
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
void onPrayerTime ();
void prayerStart ();
void prayerStop ();
void testWakeUpTime ();
void peakTime ();
String modeName (int mode);
String infoTypeName (int infoType, boolean shortened);
String ErrorTypeName (int infoType, boolean shortened);
void digitalClockDisplay ();
void printDigits (int digits);
void softwareReset ();
void testVariableChange ();
void initVariableChange ();
void sendInfo ();

#endif // ifndef head_h
