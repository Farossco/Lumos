#ifndef ESP_WIFI_H
#define ESP_WIFI_H

#include <Arduino.h>

#if defined(LUMOS_ESP8266)

# include <ESP8266WiFi.h>

# define TIME_FORMAT "json"         // Format for receive the time
# define TIME_KEY    "0D2WZ3KAP6GV" // TimeZoneDB API key
# define TIME_BY     "zone"         // localization method
# define TIME_FIELDS "timestamp"    // Needed fields in the answer

// Time
const int TIME_REQUEST_TIMEOUT = 15000; // Request timeout
const int TIME_HTTP_PORT       = 80;    // HTPP port
# define TIME_HOST "api.timezonedb.com" // HTPP host

class Wifi
{
public:
	Wifi();
	void init ();
	void getTime ();
	void receiveAndDecode ();
	void sendHeader (Client & client);

	time_t clientTimeout, restartTimeout;

private:
	WiFiServer server;
};

extern Wifi wifi;

#endif // if defined(LUMOS_ESP8266)

#endif // ifndef ESP_WIFI_H