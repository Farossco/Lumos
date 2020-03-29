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
	POW          = 3, // Provide : Light mod Power
	LMO          = 4, // Provide : Light mod
	SPEED        = 5, // Provide : Light mod Speed
	SMO          = 6, // Provide : Sound mod
	VOL          = 7, // Provide : Sound volume
	SON          = 8, // Provide : Sound on/off
	DTM          = 9  // Provide : Dawn time
};

inline RequestMessageType & operator ++ (RequestMessageType& j, int) // <--- note -- must be a reference
{
	j = static_cast<RequestMessageType>((static_cast<int>(j) + 1));

	return j;
}

typedef struct RequestData
{
	RequestMessageType type        = unknown;
	uint8_t     complement  = 0;
	int32_t     information = 0;
	RequestErrorType   error       = unknowmType;
} RequestData;

class Request
{
public:
	RequestData decode (String str);
	RequestData decode (String prefix, String complement, String information);
	void process (RequestData request);
};

extern Request request;

#endif // ifndef REQUEST_H