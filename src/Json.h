#ifndef JSON_H
#define JSON_H

#include <Arduino.h>
#include "Types.h"

class Json {
public:
	Json();
	String getDataPretty();
	String getData();
	String getResourcesPretty();
	String getResources();
	String getErrorPretty(RequestError error);
	String getError(RequestError error);

private:
	void generateData(String & string, bool pretty);
	void generateResources(String & string, bool pretty);
	void generateError(String & string, bool pretty, RequestError error);
};

extern Json json;

#endif // ifndef JSON_H
