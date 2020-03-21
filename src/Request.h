#ifndef REQUEST_H
#define REQUEST_H

#include <Arduino.h>

#define REQ_PREFIX_LENGTH 3 // The length of the request prefix

// Serial reception errors types
enum ReqErr
{
	none              = 0, // No error
	outOfBound        = 1, // Out of bound
	unknownComplement = 2, // Unknown complement
	unknowmType       = 3, // Unknown request
	badString         = 4  // Malformed string
};

// Serial reception message types
enum ReqMes : int
{
	MIN          = 0,  // - Provide type minimum value -
	SEND_MIN     = 2,  // - First value to send to Serial -
	SEND_MAX     = 10, // - Last value to send to Serial -
	MAX          = 11, // - Provide type maximum value -

	requestTime  = -2, // Request : Time
	requestInfos = -1, // Request : Info

	unknown      = 0, // Unknown type

	TIM          = 1,  // Provide : Time
	RGB          = 2,  // Provide : Light RGB
	LON          = 3,  // Provide : Light On/off
	POW          = 4,  // Provide : Light mod Power
	LMO          = 5,  // Provide : Light mod
	SPEED        = 6,  // Provide : Light mod Speed
	SMO          = 7,  // Provide : Sound mod
	VOL          = 8,  // Provide : Sound volume
	SON          = 9,  // Provide : Sound on/off
	DTM          = 10, // Provide : Dawn time
	SCO          = 11  // Provide : Sound free choice mod commands
};

inline ReqMes& operator ++ (ReqMes& j, int) // <--- note -- must be a reference
{
	j = static_cast<ReqMes>((static_cast<int>(j) + 1));

	return j;
}

typedef struct REQ
{
	ReqMes  type        = unknown;
	uint8_t complement  = 0;
	int32_t information = 0;
	ReqErr  error       = unknowmType;
} Req;

class Request
{
public:
	Req decode (String str);
	void process (Req request);
};

extern Request request;

#endif // ifndef REQUEST_H