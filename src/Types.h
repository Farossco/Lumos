#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>
#include <TimeLib.h>
#include "Utils.h"
#include <AL_ostream.h>
#include "rgb.h"

#define ilmap(x, in_min, in_max, out_min, out_max) (((float(x) - float(in_min)) * (float(out_max) - float(out_min)) / (float(in_max) - float(in_min)) + float(out_min)))

/* A Boundary structure
 */
typedef struct Bounds {
	Bounds(uint32_t min, uint32_t max) : low(min), high(max) {}

	uint32_t low  = 0;
	uint32_t high = 0;
} Bounds;


class Percentage {
public:
	const static uint8_t MIN = 0;   /* Minimum seekBar value */
	const static uint8_t MAX = 100; /* Maximum seekBar value */

	Percentage (uint8_t value);

	uint8_t value() const { return _value; } /* Gets the raw value */

	friend ostream & operator << (ostream & os, const Percentage & p) { return os << p._value << "%"; }

private:
	uint8_t _value;
};

/* Serial reception errors types
 */
class RequestError {
public:
	enum Enum : uint8_t {
		none,                /* No error */
		incorrectValue,      /* Incorrect Information */
		incorrectComplement, /* Incorrect Complement */
		incorrectType,       /* Incorrect Type */
		emptyString,         /* Empty string */

		N,
		MIN = 0,
		MAX = N - 1
	};

	RequestError ();
	RequestError(uint8_t value);
	operator uint8_t ();
	const String toString() const;
	friend ostream & operator << (ostream & os, const RequestError & mode);

private:
	Enum _value;
};


/* Complement Category
 */
enum class ComplementCategory : uint8_t {
	none,        /* No complement */
	lightMode,   /* Light mode */
	soundCommand /* Sound command parameter */
};


/* Serial reception request types
 */
class RequestType {
public:

	enum Enum : uint8_t {
		unknown,      /* Unknown type */
		requestTime,  /* Request : Time */
		requestData,  /* Request : Info */
		soundCommand, /* Sound commands for "free choice" mode */
		provideTime,  /* Provide : Time */

		lightOnOff,              /* Provide : Light On/off */
		lightModeRgb,            /* Provide : Light RGB */
		lightModePower,          /* Provide : Light mode Power */
		lightMode,               /* Provide : Light mode */
		lightModeSpeed,          /* Provide : Light mode Speed */
		soundOnOff,              /* Provide : Sound on/off */
		soundMode,               /* Provide : Sound mode */
		soundVolume,             /* Provide : Sound volume */
		alarmDawnVolume,         /* Provide : Dawn sounds volume */
		alarmDawnTime,           /* Provide : Dawn alarm time */
		alarmDawnDuration,       /* Provide : Dawn alarm duration */
		alarmSunsetDuration,     /* Provide : Sunset mode duration */
		alarmSunsetDecreaseTime, /* Provide : Sunset mode decrease time */

		N,
		SEND_START = lightOnOff, /* - First value to send to Serial - */
		MIN        = 0,          /* - Minimum value - */
		MAX        = N - 1       /* - Maximum value - */
	};

	RequestType();
	RequestType(uint8_t value);
	operator uint8_t ();
	RequestType & operator ++ (int);
	Bounds getInformationBounds();
	Bounds getComplementBounds();
	ComplementCategory getComplementType();
	bool needsComplement();
	const String toString(bool shortened = false) const;
	RequestType operator = (const String & typeString);
	friend bool operator == (const String & string, const RequestType & type);
	friend ostream & operator << (ostream & os, const RequestType & mode);

private:
	Enum _value;
};


/* Sound Mode
 */
class SoundMode {
public:
	enum Enum {
		freeChoice, /* Free choice mode */

		N,
		MIN = 0,
		MAX = N - 1
	};

	SoundMode();
	SoundMode(uint8_t value);
	operator uint8_t ();
	const String toString() const;
	friend ostream & operator << (ostream & os, const SoundMode & mode);

private:
	Enum _value;
};


#endif // ifndef TYPES_H
