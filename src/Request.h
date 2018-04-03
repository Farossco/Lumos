#ifndef REQUEST_H
#define REQUEST_H

#include <Arduino.h>

// Serial reception types
const uint8_t TYPE_RTM      = -2; // Request : Time
const uint8_t TYPE_RIF      = -1; // Request : Info
const uint8_t TYPE_MIN      = 0;  // - Provide type minimum value -
const uint8_t TYPE_UNK      = 0;  // Unknown type
const uint8_t TYPE_TIM      = 1;  // Provide : Time
const uint8_t TYPE_SEND_MIN = 2;  // - Send informations from this...
const uint8_t TYPE_RGB      = 2;  // Provide : Light RGB
const uint8_t TYPE_ONF      = 3;  // Provide : Light On/off
const uint8_t TYPE_POW      = 4;  // Provide : Light mod Power
const uint8_t TYPE_LMO      = 5;  // Provide : Light mod
const uint8_t TYPE_SPE      = 6;  // Provide : Light mod Speed
const uint8_t TYPE_SMO      = 7;  // Provide : Sound mod
const uint8_t TYPE_VOL      = 8;  // Provide : Sound volume
const uint8_t TYPE_SON      = 9;  // Provide : Sound on/off
const uint8_t TYPE_SEND_MAX = 9;  // ...to this -
const uint8_t TYPE_SCO      = 10; // Provide : Sound free choice mod commands
const uint8_t TYPE_MAX      = 10; // - Provide type maximum value -

// Serial reception errors
const int ERR_NOE = 0; // No error
const int ERR_OOB = 1; // Out of bound
const int ERR_UKC = 2; // Unknown complement
const int ERR_UKR = 3; // Unknown request

class Request
{
public:
	void decode (char * request, uint8_t & type, uint8_t & complement, int32_t & information, int8_t & error);
};

extern Request request;

#endif // ifndef REQUEST_H
