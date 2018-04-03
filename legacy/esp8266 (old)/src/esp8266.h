#ifndef esp8266_h
#define esp8266_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <TimeLib.h>

// ************************************************************************** //
// ************************* Constants declarations ************************* //
// ************************************************************************** //

const boolean DEBUG_ENABLED = false; // Debug mode

const int BAUD_RATE = 9600; // Serial baud rate

const int PIN_D0  = 16; // D0   USER   WAKE
const int PIN_D1  = 5;  // D1
const int PIN_D2  = 4;  // D2
const int PIN_D3  = 0;  // D3   FLASH
const int PIN_D4  = 2;  // D4   TXD1
const int PIN_D5  = 14; // D5          HSCLK
const int PIN_D6  = 12; // D6          HMISO
const int PIN_RX2 = 13; // D7   RXD2   HMOSI
const int PIN_TX2 = 15; // D8   TXD2   HCS
const int PIN_RX0 = 3;  // D9   RXD0
const int PIN_TX0 = 1;  // D10  TXD0

// Wi-Fi informations
const char * SSID0 = "Patatou";    // Wi-Fi SSID
const char * PASS0 = "FD00000000"; // Wi-Fi password

// ******** IDs ******** //
// Serial reception types
const int TYPE_RTM = -2; // Request : Time
const int TYPE_RIF = -1; // Request : Info
const int TYPE_UNK = 0;  // -Unknown type-
const int TYPE_TIM = 1;  // Provide : Time
const int TYPE_RGB = 2;  // Provide : RGB
const int TYPE_ONF = 3;  // Provide : On
const int TYPE_POW = 4;  // Provide : Power
const int TYPE_MOD = 5;  // Provide : Mode
const int TYPE_SPE = 6;  // Provide : Speed

// Serial reception errors
const int ERR_NOE = 0; // No error
const int ERR_OOB = 1; // Out of bound
const int ERR_UKM = 2; // Unknown mode
const int ERR_UKR = 3; // Unknown request

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

// Location
const String LATITUDE  = "48.866667";    // My latitude (Well... not really)
const String LONGITUDE = "2.333333";     // My longitude (Not really either :p)
const String TIME_ZONE = "Europe/Paris"; // My time zone

// Time
const int TIME_REQUEST_TIMEOUT = 15000;                // Request timeout
const int TIME_HTTP_PORT       = 80;                   // HTPP port
const char * TIME_HOST         = "api.timezonedb.com"; // HTPP host
const String TIME_FORMAT       = "json";               // Format for receive the time
const String TIME_KEY          = "0D2WZ3KAP6GV";       // TimeZoneDB API key
const String TIME_BY           = "zone";               // localization method
const String TIME_FIELDS       = "timestamp";          // Needed fields in the answer

// Bounds
const int MIN_POWER   = 0;   // Minimum power value
const int MAX_POWER   = 100; // Maximum power value
const int SEEKBAR_MIN = 0;   // Minimum app seek bars value
const int SEEKBAR_MAX = 100; // Maximum app seek bars value

// ************************************************************************** //
// ************************* Variables declarations ************************* //
// ************************************************************************** //

// Global
boolean on;          // If the leds are ON or OFF (True: ON / False: OFF)
unsigned long rgb;   // Currently displayed RGB value (From 0x000000 to 0xFFFFFF)
unsigned char red;   // Current red value including lightning power (From 0 to 255)
unsigned char green; // Current green value including lightning power (From 0 to 255)
unsigned char blue;  // Current blue value including lightning power (From 0 to 255)
int power[N_MODE];   // Current lightning power (from MINPOWER to MAXPOWER)
int speed[N_MODE];   // Current mode speed
unsigned char mode;  // Current lighting mode

// Wifi webserver
WiFiServer server (80); // Web serveur declaration with port 80

// ************************************************************************** //
// ************************** Functions prototypes ************************** //
// ************************************************************************** //

void setup ();
void loop ();
void initGpios ();
void readSerial ();
void initSerial ();
void printPrefix ();
void printDebugDigits (int digits);
void debugClockDisplay ();
void debugClockDisplayNoPrefix ();
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
void decodeRequest (String request, long & result, int & infoMode, int & infoType, int & errorType);
void sendJsonToSerial (String status, String message);
void sendJsonToClient (String status, String message, WiFiClient client);
void readWeb ();
String decodeWeb (WiFiClient client);
void initWifiServer ();
void sendTime ();
void getTime ();
String modeName (int mode);
String infoTypeName (int infoType, boolean shortened);
String errorTypeName (int infoType, boolean shortened);
void digitalClockDisplay ();
void printDigits (int digits);
void rgb2color ();

#endif // ifndef esp8266_h
