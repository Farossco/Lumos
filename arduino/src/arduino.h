#ifndef head_h
#define head_h

#include <IRremote.h>
#include <TimeAlarms.h>
#include <DFRobotDFPlayerMini.h>

#define DEBUG_ENABLED    true // Debug Mode

#define ESP_BAUD_RATE    74880  // ESP8266 communication baud rate
#define DEBUG_BAUD_RATE  250000 // Debug baud rate
#define DFP_BAUD_RATE    9600   // DFPlayer communication baud rate

#define WAIT_FOR_TIME    true  // If we have to wait for time sync (if true, program will not start until time is synced)
#define INFRARED_ENABLED true  // If we allow infrared communication
#define SOUND_ENABLED    false // Enable sound
#define CLAP_ENABLED     false // Enable double claping

// ******* Pins ******* //
#define PIN_SOUND        2  // Sound detector IN pin
#define PIN_LED_INFRARED 5  // Infrared IN pin
#define PIN_LED_RED      4  // Red LED OUT pin
#define PIN_LED_GREEN    7  // Green LED OUT pin
#define PIN_LED_BLUE     11 // Blue LED OUT pin

// ******* Wake up ******* //
#define WAKEUP_HOURS   6
#define WAKEUP_MINUTES 15
#define WAKEUP_SECONDS 00

// ******* Prayer ******* //
#define PRAYER_FADE_SPEED 97 // Fade speed for prayer time

// ******** IDs ******** //
/******** Serial reception types ********/
#define TYPE_UNK -1
#define TYPE_TIM 0
#define TYPE_RGB 1
#define TYPE_ONF 2
#define TYPE_POW 3
#define TYPE_MOD 4
#define TYPE_PRT 5

/******** Modes ********/
#define MODE_DEFAULT 0
#define MODE_FLASH   1
#define MODE_STROBE  2
#define MODE_FADE    3
#define MODE_SMOOTH  4
#define MODE_WAKEUP  5
#define MODE_MAX     5

// ******* Infrared ******* //
#define N_COLOR 16 // Number of different colors

// ******* DFPlayer ******* //
#define DEFAULT_VOLUME 30

// ******** Sound ******** //
#define MIN_CLAP_DURATION          30  // Amount of time to ignore after clap started (To avoid sound bounce)
#define MAX_CLAP_DURATION          100 // Maximum clap duration in ms

#define MIN_TIME_BEETWIN_TWO_CLAPS 400 // Minimum time before starting second clap
#define MAX_TIME_BEETWIN_TWO_CLAPS 600 // Maximum time to start second clap

#define TIME_AFTER_START_OVER      1000 // Time to wait after double clap to start over

#define N_PRAYER                   6 // Number of different prayer (including sunrise)

#endif // ifndef head_h
