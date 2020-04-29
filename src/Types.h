#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

/* A Boundary structure
 */
typedef struct Bounds
{
	Bounds(uint32_t min, uint32_t max) : low (min), high (max){ }

	uint32_t low  = 0;
	uint32_t high = 0;
} Bounds;


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


/* Light Setting
 */
class LightSetting
{
public:
	const static uint8_t MIN_POWER   = 5;               // Minimum power value
	const static uint8_t MAX_POWER   = 25;              // Maximum power value
	const static uint8_t DEF_POWER   = MAX_POWER;       // Default power on program startup
	const static uint8_t MIN_COLOR   = 0;               // Maximum color value
	const static uint8_t MAX_COLOR   = 0xFF;            // Maximum color value
	const static uint32_t MIN_RGB    = 0x00000000;      // Minimum RGB value
	const static uint32_t MAX_RGB    = 0x00FFFFFF;      // Maximum RGB value
	const static uint32_t DEF_RGB    = MAX_RGB;         // Default RGB on program startup
	const static uint16_t MIN_SPEED  = 0;               // Minimum speed
	const static uint16_t MAX_SPEED  = 95;              // Maximum speed
	const static uint16_t DEF_SPEED  = MAX_SPEED / 1.5; // Default speed on program first launch
	const static uint8_t MIN_PERCENT = 0;               // Minimum seekBar value
	const static uint8_t MAX_PERCENT = 100;             // Maximum seekBar value

	uint8_t & operator [] (LightMode mode);
	LightSetting operator = (const LightSetting & copy);
	LightSetting operator = (uint8_t value);

private:
	uint8_t _value[LightMode::N];
};


/* Serial reception errors types
 */
class RequestError
{
public:
	enum Enum : uint8_t
	{
		none = 0,          // No error
		outOfBound,        // Out of bound
		unknownComplement, // Unknown complement
		unknownType,       // Unknown request
		emptyString,       // Empty string

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
		requestInfos, // Request : Info
		soundCommand, // Sound commands for "free choice" mode
		provideTime,  // Provide : Time

		lightOnOff,     // Provide : Light On/off
		lightRgb,       // Provide : Light RGB
		lightPower,     // Provide : Light mode Power
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


/* Sound Setting
 */
class SoundSetting
{
public:
	const static uint8_t MIN_SOUND = 0;  // Minimum sound volume
	const static uint8_t DEF_SOUND = 5;  // Default sound volume
	const static uint8_t MAX_SOUND = 30; // Maximum sound volume

	SoundSetting();
	SoundSetting (uint8_t value);
	operator uint8_t ();

private:
	uint8_t _value;
};

#endif // ifndef TYPES_H