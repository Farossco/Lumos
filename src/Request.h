#ifndef REQUEST_H
#define REQUEST_H

#include <Arduino.h>

#define REQ_PREFIX_LENGTH 3 // The length of the request prefix

// Serial reception errors types
enum RequestErrorType
{
	noError           = 0, // No error
	outOfBound        = 1, // Out of bound
	unknownComplement = 2, // Unknown complement
	unknowmType       = 3, // Unknown request
	badString         = 4  // Malformed string
};

// Serial reception message types
enum RequestMessageType : int
{
	MIN          = -4, // - Provide type minimum value -
	SEND_MIN     = 1,  // - First value to send to Serial -
	SEND_MAX     = 9,  // - Last value to send to Serial -
	MAX          = 9,  // - Provide type maximum value -

	requestTime  = -4, // Request : Time
	requestInfos = -3, // Request : Info
	provideTime  = -2, // Provide : Time
	soundCommand = -1, // Sound commands for "free choice" mode

	unknown      = 0, // Unknown type

	RGB          = 1, // Provide : Light RGB
	LON          = 2, // Provide : Light On/off
	POW          = 3, // Provide : Light mode Power
	LMO          = 4, // Provide : Light mode
	SPEED        = 5, // Provide : Light mode Speed
	SMO          = 6, // Provide : Sound mode
	VOL          = 7, // Provide : Sound volume
	SON          = 8, // Provide : Sound on/off
	DTM          = 9  // Provide : Dawn time
};

inline RequestMessageType & operator ++ (RequestMessageType& j, int) // <--- note -- must be a reference
{
	j = static_cast<RequestMessageType>((static_cast<int>(j) + 1));

	return j;
}

class Request
{
public:
	Request(String inputString);
	Request(String prefixString, String complementString, String informationString);
	void process ();

	RequestErrorType error  = unknowmType;
	RequestMessageType type = unknown;
	uint8_t complement      = 0;
	int32_t information     = 0;

private:
	void decodeInput ();
	void decodeSeparate ();
};

#endif // ifndef REQUEST_H