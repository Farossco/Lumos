#ifndef esp8266_h
#define esp8266_h

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <WiFiUdp.h>

#define BAUD_RATE     74880 // Serial baud rate
#define DEBUG_ENABLED true  // Debug mode

// Wi-Fi informations
#define SSID0 "Patatou"
#define PASS0 "FD00000000"

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

#define MODE_MIN     0
#define MODE_MAX     5

// Location
#define LATITUDE  "48.866667"
#define LONGITUDE "2.333333"
#define TIME_ZONE "Europe/Paris"

// Prayer times
#define PRAYER_REQUEST_TIMEOUT 15000
#define PRAYER_HTTP_PORT       80
#define PRAYER_HOST            "api.aladhan.com"
#define PRAYER_METHOD          1

// Time
#define TIME_REQUEST_TIMEOUT 15000
#define TIME_HTTP_PORT       80
#define TIME_HOST            "api.timezonedb.com"
#define TIME_FORMAT          "json"
#define TIME_KEY             "0D2WZ3KAP6GV"
#define TIME_BY              "zone"
#define TIME_FIELDS          "timestamp"


#endif // ifndef esp8266_h
