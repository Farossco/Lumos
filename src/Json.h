#ifndef JSON_H
#define JSON_H

#include <Arduino.h>

#if defined(ESP8266_PERI_H_INCLUDED)

# include <ESP8266WiFi.h>

class Json
{
public:
	Json();
	void send (char * status, char * message, WiFiClient * client);
};

extern Json json;

#endif // if defined(ESP8266_PERI_H_INCLUDED)

#endif // ifndef JSON_H
