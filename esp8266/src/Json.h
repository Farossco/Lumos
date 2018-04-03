#ifndef JSON_H
#define JSON_H

#include <ESP8266WiFi.h>

class Json
{
public:
	Json();
	void send (char * status, char * message, WiFiClient *client);
};

extern Json json;

#endif // ifndef JSON_H
