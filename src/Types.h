#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>
#include <Time.h>
#include "Utils.h"
#include <ostream>

#define ilmap(x, in_min, in_max, out_min, out_max) (((float(x) - float(in_min)) * (float(out_max) - float(out_min)) / (float(in_max) - float(in_min)) + float(out_min)))

/* A Boundary structure
 */
typedef struct Bounds
{
	Bounds(uint32_t min, uint32_t max) : low (min), high (max){ }

	uint32_t low  = 0;
	uint32_t high = 0;
} Bounds;


class Percentage
{
public:
	const static uint8_t MIN = 0;   // Minimum seekBar value
	const static uint8_t MAX = 100; // Maximum seekBar value

	Percentage (uint8_t value);

	// uint8_t & raw (){ return _value; } // Gets the raw value

	uint8_t value () const { return _value; } // Gets the raw value

	friend ostream & operator << (ostream & os, const Percentage & p){ return os << p._value; }

	// operator uint8_t () const;

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
template <class T, const T MINv, const T MAXv, const uint8_t DEFp, int UID = 0>
class SettingBase
{
public:
	enum Enum : T
	{
		MIN = MINv,
		MAX = MAXv,
		DEF = T (ilmap (DEFp, 0, 100, MIN, MAX))
	};

	SettingBase(){ }

	SettingBase (T value){ _value = constrain (value, MIN, MAX); }

	T operator =  (Enum value){ return _value = (T) value; }

	T operator = (const Percentage & perc){ return _value = utils.map (perc.value(), Percentage::MIN, Percentage::MAX, MIN, MAX); }

	T operator = (const SettingBase & sett){ return _value = constrain (sett._value, MIN, MAX); }

	bool operator == (const SettingBase & sett){ return _value == sett._value; }

	bool operator != (const SettingBase & sett){ return _value != sett._value; }

	T operator * (const SettingBase & sett){ return _value * sett._value; }

	template <class U>
	U operator * (U value){ return _value * value; }

	template <class U>
	U operator / (U value){ return _value / value; }

	T operator >> (int value){ return _value >> value; }

	uint32_t operator << (int value){ return ((uint32_t) _value) << value; }

	T operator & (T value){ return _value & value; }

	void operator += (const Percentage & perc){ _value = constrain (this->toPercent().value() + perc.value(), MIN, MAX); }

	void operator -= (const Percentage & perc){ _value = constrain (this->toPercent().value() - perc.value(), MIN, MAX); }

	T value () const { return _value; } // Get the raw value

	friend ostream & operator << (ostream & os, const SettingBase & sett){ return os << sett._value; }

	Percentage toPercent (){ return Percentage (utils.map (_value, MIN, MAX, Percentage::MIN, Percentage::MAX)); }

protected:
	T _value;
};

/** SettingBaseArray
 *
 * T : The type of setting to store in the array
 */
template <class T, class ModeT>
class SettingArrayBase
{
public:
	T & operator [] (ModeT mode){ return _value[constrain (mode, ModeT::MIN, ModeT::MAX)]; }

	T operator [] (ModeT mode) const { return _value[constrain (mode, ModeT::MIN, ModeT::MAX)]; }

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
class LightMode
{
public:
	enum Enum : uint8_t
	{
		continuous,     // Continuous lightning mode
		flash,          // Flash mode
		strobe,         // Strobe mode
		fade,           // Fade mode
		smooth,         // Smooth mode
		dawn,           // Dawn mode
		sunset,         // Sunset mode
		startAnimation, // Start animation mode
		music,          // Music mode

		N,
		MIN = continuous, // -Minimum mode value-
		MAX = music,      // -Maximum mode value-
	};

	LightMode();
	LightMode (uint8_t value);
	operator uint8_t ();
	const String toString () const;
	inline LightMode & operator ++ (int){ _value = static_cast<Enum>(static_cast<uint8_t>(_value) + 1); return *this; }

private:
	Enum _value;
};

typedef SettingBase<bool, false, true, 0, 0> LightOnOff;

typedef SettingBase<uint8_t, 5, 25, 100> LightPower;
typedef SettingArrayBase<LightPower, LightMode> LightPowerArray;

typedef SettingBase<uint32_t, 0x00000000, 0x00FFFFFF, 100> LightRgb;
typedef SettingArrayBase<LightRgb, LightMode> LightRgbArray;

typedef SettingBase<uint8_t, 0x00, 0xFF, 100> LightColor;
typedef SettingArrayBase<LightColor, LightMode> LightColorArray;

typedef SettingBase<uint8_t, 0, 95, 67> LightSpeed;
typedef SettingArrayBase<LightSpeed, LightMode> LightSpeedArray;

typedef SettingBase<time_t, 0, -1U, 0> LightTiming;
typedef SettingArrayBase<LightTiming, LightMode> LightTimingArray;

/* Serial reception errors types
 */
class RequestError
{
public:
	enum Enum : uint8_t
	{
		none = 0,            // No error
		incorrectValue,      // Incorrect Information
		incorrectComplement, // Incorrect Complement
		incorrectType,       // Incorrect Type
		emptyString,         // Empty string

		N,
		MIN = none,
		MAX = emptyString
	};

	RequestError ();
	RequestError(uint8_t value);
	operator uint8_t ();
	const String toString () const;

private:
	Enum _value;
};


/* Complement Category
 */
enum class ComplementCategory : uint8_t
{
	none,        // No complement
	lightMode,   // Light mode
	soundCommand // Sound command parameter
};


/* Serial reception request types
 */
class RequestType
{
public:

	enum Enum : uint8_t
	{
		unknown,      // Unknown type
		requestTime,  // Request : Time
		requestData,  // Request : Info
		soundCommand, // Sound commands for "free choice" mode
		provideTime,  // Provide : Time

		lightOnOff,     // Provide : Light On/off
		lightModeRgb,   // Provide : Light RGB
		lightModePower, // Provide : Light mode Power
		lightMode,      // Provide : Light mode
		lightModeSpeed, // Provide : Light mode Speed
		soundOnOff,     // Provide : Sound on/off
		soundMode,      // Provide : Sound mode
		soundVolume,    // Provide : Sound volume
		dawnTime,       // Provide : Dawn time

		N,
		MIN      = unknown,    // - Minimum value -
		SEND_MIN = lightOnOff, // - First value to send to Serial -
		SEND_MAX = dawnTime,   // - Last value to send to Serial -
		MAX      = dawnTime    // - Maximum value -
	};

	RequestType();
	RequestType(uint8_t value);
	operator uint8_t ();
	RequestType & operator ++ (int);
	Bounds getInformationBounds ();
	Bounds getComplementBounds ();
	ComplementCategory getComplementType ();
	const String toString (bool shortened = false) const;
	RequestType operator = (const String & typeString);

private:
	Enum _value;
};

bool operator == (const String & string, const RequestType & type);


/* Sound Mode
 */
class SoundMode
{
public:
	enum Enum
	{
		freeChoice, // Free choice mode

		N,
		MIN = freeChoice, // -Minimum mode value-
		MAX = freeChoice  // -Maximum mode value-
	};

	SoundMode();
	SoundMode(uint8_t value);
	operator uint8_t ();
	const String toString () const;

private:
	Enum _value;
};


/* Sound Command
 */
class SoundCommand
{
public:
	enum Enum
	{
		playRandom,
		playOne,
		playNext,
		playPrevious,
		pause,
		resume,
		playDawn,

		N,
		MIN = playRandom,
		MAX = playDawn
	};

	SoundCommand();
	SoundCommand(uint8_t value);
	operator uint8_t ();
	const String toString () const;

private:
	Enum _value;
};

typedef SettingBase<bool, false, true, 0, 1> SoundOnOff;
typedef SettingBase<uint8_t, 0, 30, 15> SoundVolume;

#endif // ifndef TYPES_H