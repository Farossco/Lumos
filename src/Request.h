#ifndef REQUEST_H
#define REQUEST_H

#include <Arduino.h>
#include "Types.h"

#define REQ_PREFIX_LENGTH 3 /* The length of the request prefix */

class Request {
public:
	Request(const String & prefixString, const String & complementString, const String & informationString);
	void process();

	RequestError getError();
	RequestType getType();
	uint8_t getComplement();
	uint32_t getInformation();

	String getComplementString();
	String getInformationString();

private:
	RequestError error   = RequestError::none;
	RequestType type     = RequestType::unknown;
	uint8_t complement   = 0;
	uint32_t information = 0;

	void displayDebug();
};

#endif // ifndef REQUEST_H
