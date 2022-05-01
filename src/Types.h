#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>
#include <TimeLib.h>
#include "Utils.h"
#include <AL_ostream.h>

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

/** SettingBase
 *
 * T : The type of value stored
 * MINv : The minimum value of the setting
 * MAXv : The maximum value of the setting
 * DEFp : The default value in percentage of the max value
 *     (Ex : DEFp = 50 -> Default value is 50% of the max value)
 * UID : A unique ID to avoid types like LightOnOff and SoundOnOff to be the same type
 */
template <class T, const T MINv, const T MAXv, const uint8_t DEFp, int UID = 0> class SettingBase {
public:
	enum Enum : T {
		MIN = MINv,
		MAX = MAXv,
		DEF = T(ilmap(DEFp, 0, 100, MIN, MAX))
	};

	SettingBase() : _value(0) {}

	SettingBase (T value) : _value(constrain(value, MIN, MAX)) {}

	T operator = (Enum value) { return _value = (T)value; }

	T operator = (const Percentage & perc) { return _value = utils.map(perc.value(), Percentage::MIN, Percentage::MAX, MIN, MAX); }

	T operator = (const SettingBase & sett) { return _value = constrain(sett._value, MIN, MAX); }

	bool operator == (const SettingBase & sett) { return _value == sett._value; }

	bool operator != (const SettingBase & sett) { return _value != sett._value; }

	T operator * (const SettingBase & sett) { return _value * sett._value; }

	template <class U> U operator * (U value) { return _value * value; }

	template <class U> U operator / (U value) { return _value / value; }

	void operator += (const Percentage & perc) { _value = constrain(this->toPercent().value() + perc.value(), MIN, MAX); }

	void operator -= (const Percentage & perc) { _value = constrain(this->toPercent().value() - perc.value(), MIN, MAX); }

	T value() const { return _value; } /* Get the raw value */

	friend ostream & operator << (ostream & os, const SettingBase & sett) { return os << sett._value << " (" << sett.toPercent() << ")"; }

	Percentage toPercent() const { return Percentage(utils.map(_value, MIN, MAX, Percentage::MIN, Percentage::MAX)); }

protected:
	T _value;
};

/** SettingBaseArray
 *
 * T : The type of setting to store in the array
 */
template <class T, class ModeT> class SettingArrayBase {
public:
	T & operator [] (ModeT mode) { return _value[constrain(mode, ModeT::MIN, ModeT::MAX)]; }

	T operator [] (ModeT mode) const { return _value[constrain(mode, ModeT::MIN, ModeT::MAX)]; }

	SettingArrayBase operator = (const SettingArrayBase & copy)
	{
		for (ModeT mode = ModeT::MIN; mode <= ModeT::MAX; mode++)
			(*this)[mode] = copy[mode];

		return *this;
	}

	SettingArrayBase operator = (T value)
	{
		for (ModeT mode = ModeT::MIN; mode <= ModeT::MAX; mode++)
			_value[mode] = value;

		return *this;
	}

protected:
	T _value[ModeT::N];
};


/* Light Mode
 */
class LightMode {
public:
	enum Enum : uint8_t {
		continuous, /* Continuous lightning mode */
		flash,      /* Flash mode */
		strobe,     /* Strobe mode */
		fade,       /* Fade mode */
		smooth,     /* Smooth mode */
		dawn,       /* Dawn mode */
		sunset,     /* Sunset mode */
		music,      /* Music mode */

		N,
		MIN = 0,
		MAX = N - 1
	};

	LightMode();
	LightMode (uint8_t value);
	operator uint8_t ();
	const String toString() const;
	LightMode & operator ++ (int);
	friend ostream & operator << (ostream & os, const LightMode & mode);

private:
	Enum _value;
};

typedef SettingBase<bool, false, true, 0, 0> LightOnOff;

typedef SettingBase<uint8_t, 5, 15, 100> LightPower;
typedef SettingArrayBase<LightPower, LightMode> LightPowerArray;

typedef SettingBase<uint8_t, 0x00, 0xFF, 100> LightColor;
typedef SettingArrayBase<LightColor, LightMode> LightColorArray;

class LightRgb : public SettingBase<uint32_t, 0x00000000, 0x00FFFFFF, 100> {
public:
	LightRgb ();
	LightRgb (uint32_t);
	LightRgb setRed(LightColor);
	LightRgb setGreen(LightColor);
	LightRgb setBlue(LightColor);
	LightRgb setHue(uint8_t);
	LightColor getRed() const;
	LightColor getGreen() const;
	LightColor getBlue() const;
	LightRgb operator * (double) const;
	LightRgb operator / (double) const;
	friend ostream & operator << (ostream & os, const LightRgb & rgb);
};
typedef SettingArrayBase<LightRgb, LightMode> LightRgbArray;

typedef SettingBase<uint8_t, 0, 95, 67> LightSpeed;
typedef SettingArrayBase<LightSpeed, LightMode> LightSpeedArray;

class Timing : public SettingBase<uint32_t, 0, -1UL, 0> {
public:
	Timing ();
	Timing (uint32_t time);
	Timing (uint8_t minute, uint8_t second);
	uint8_t minute() const;
	uint8_t second() const;
	friend ostream & operator << (ostream & os, const Timing & timing);
};

class Time : public SettingBase<uint16_t, 0, 1439, 0> {
public:
	Time ();
	Time (uint16_t time);
	Time (uint8_t hour, uint8_t minute);
	uint8_t hour() const;
	uint8_t minute() const;
	friend ostream & operator << (ostream & os, const Time & time);
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


/* Sound Command
 */
class SoundCommand {
public:
	enum Enum {
		playRandom,
		playOne,
		playNext,
		playPrevious,
		pause,
		resume,
		playDawn,

		N,
		MIN = 0,
		MAX = N - 1
	};

	SoundCommand();
	SoundCommand(uint8_t value);
	operator uint8_t ();
	const String toString() const;
	friend ostream & operator << (ostream & os, const SoundCommand & mode);

private:
	Enum _value;
};

typedef SettingBase<bool, false, true, 0, 1> SoundOnOff;
typedef SettingBase<uint8_t, 0, 30, 15> SoundVolume;

#endif // ifndef TYPES_H
