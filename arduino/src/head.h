#ifndef head_h
#define head_h

#include <IRremote.h>
#include <TimeAlarms.h>
#include <DFRobotDFPlayerMini.h>

#define DEBUG_ENABLED    true // Debug Mode

#define ESP_BAUD_RATE    250000 // ESP8266 communication baud rate
#define DEBUG_BAUD_RATE  250000 // Debug baud rate
#define DFP_BAUD_RATE    9600   // DFPlayer communication baud rate

#define DEFAULT_COLOR    0xFFFFFF // Default color on program startup

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

// ******* Power ******* //
#define MIN_POWER     0   // Minimum power value
#define MAX_POWER     100 // Maximum power value
#define DEFAULT_POWER 50  // Default power value

// ******* Modes ******* //
/******** Flash *******/
#define MIN_FLASH              -100 // Minimum flash speed
#define MAX_FLASH              50   // Maximum flash speed
#define IR_FLASH_CHANGE_SPEED  1    // Flash increasion or dicreasion speed
#define DEFAULT_FLASH_SPEED    0    // Default flash speed value
/******** Strobe *******/
#define MIN_STROBE             -100 // Minimum strobe speed
#define MAX_STROBE             50   // Maximum strobe speed
#define IR_STROBE_CHANGE_SPEED 1    // Strobe increasion or dicreasion speed
#define DEFAULT_STROBE_SPEED   0    // Default strobe speed value
/******** Fade *******/
#define MIN_FADE               70  // Minimum fade speed
#define MAX_FADE               100 // Maximum fade speed
#define IR_FADE_CHANGE_SPEED   1   // Fade increasion or dicreasion speed
#define DEFAULT_FADE_SPEED     75  // Default fade speed value
/******** Smooth *******/
#define MIN_SMOOTH             76  // Minimum smooth speed
#define MAX_SMOOTH             100 // Maximum smooth speed
#define IR_SMOOTH_CHANGE_SPEED 1   // Smooth increasion or dicreasion speed
#define DEFAULT_SMOOTH_SPEED   80  // Default smooth speed value
/******** Wake up *******/
#define WAKE_UP_SLOWNESS       1000 // Wake up slowness

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

// ******* Infrared ******* //
#define POWER_CHANGE_SPEED 5  // Power increasion/decreasion speed
#define N_COLOR            16 // Number of different colors

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
