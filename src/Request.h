#ifndef REQUEST_H
#define REQUEST_H

#include <Arduino.h>
#include "Types.h"

#define REQ_PREFIX_LENGTH 3 // The length of the request prefix

class Request
{
public:
	Request(String inputString);
	Request(String prefixString, String complementString, String informationString);
	void process ();

	RequestError getError ();
	RequestType getType ();
	uint8_t getComplement ();
	int32_t getInformation ();

	const char * getTypeString ();
	String getComplementString ();
	String getInformationString ();

private:
	RequestError error   = RequestError::unknownType;
	RequestType type     = RequestType::unknown;
	uint8_t complement   = 0;
	uint32_t information = 0;

	void decodeInput ();
	void decodeSeparate ();
	void displayDebug ();
};

#endif // ifndef REQUEST_H