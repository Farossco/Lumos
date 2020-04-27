#ifndef REQUEST_H
#define REQUEST_H

#include <Arduino.h>

#define REQ_PREFIX_LENGTH 3 // The length of the request prefix

// Serial reception errors types
enum RequestError : uint8_t
{
	noError           = 0, // No error
	outOfBound        = 1, // Out of bound
	unknownComplement = 2, // Unknown complement
	unknowmType       = 3, // Unknown request
	badString         = 4  // Malformed string
};

// Serial reception message types
enum RequestType : uint8_t
{
	MIN            = 0,  // - Minimum value -
	SEND_MIN       = 5,  // - First value to send to Serial -
	SEND_MAX       = 13, // - Last value to send to Serial -
	MAX            = 13, // - Maximum value -

	unknown        = 0, // Unknown type

	requestTime    = 1, // Request : Time
	requestInfos   = 2, // Request : Info
	soundCommand   = 3, // Sound commands for "free choice" mode

	provideTime    = 4, // Provide : Time

	lightOnOff     = 5,  // Provide : Light On/off
	lightRgb       = 6,  // Provide : Light RGB
	lightPower     = 7,  // Provide : Light mode Power
	lightMode      = 8,  // Provide : Light mode
	lightModeSpeed = 9,  // Provide : Light mode Speed
	soundOnOff     = 10, // Provide : Sound on/off
	soundMode      = 11, // Provide : Sound mode
	soundVolume    = 12, // Provide : Sound volume
	dawnTime       = 13  // Provide : Dawn time
};

inline RequestType & operator ++ (RequestType& j, int)
{
	j = static_cast<RequestType>((static_cast<int>(j) + 1));

	return j;
}

enum ComplementType
{
	complementNone         = 0, // No complement
	complementLightMode    = 1, // Light mode
	complementSoundCommand = 2  // Sound command parameter
};

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

	ComplementType getComplementType ();

private:
	RequestError error  = unknowmType;
	RequestType type    = unknown;
	uint8_t complement  = 0;
	int32_t information = 0;

	void decodeInput ();
	void decodeSeparate ();
};

#endif // ifndef REQUEST_H