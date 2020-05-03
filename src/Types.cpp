#include "Types.h"
#include "Resources.h"
#include "Utils.h"
#include "ArduinoLogger.h"

#define getArrayString(in, array) ((in < (sizeof(array) / sizeof(*array))) ? array[in] : nameUnknown)

/****************************** LightMode ******************************/
LightMode::LightMode() : _value (MIN){ }

LightMode::LightMode (uint8_t value){ _value = static_cast<Enum>(value); }

const String LightMode::toString () const { return getArrayString (_value, lightModeName); }

LightMode::operator uint8_t (){ return static_cast<uint8_t>(_value); }


/****************************** Percentage ******************************/
Percentage::Percentage (uint8_t value){ _value = constrain (value, MIN, MAX); }


/****************************** RequestError ******************************/
RequestError::RequestError (){ }

RequestError::RequestError(uint8_t value){ _value = static_cast<Enum>(value); }

RequestError::operator uint8_t (){ return static_cast<uint8_t>(_value); }

const String RequestError::toString () const { return getArrayString (_value, errorName); }


/****************************** RequestType ******************************/
RequestType::RequestType(){ }

RequestType::RequestType(uint8_t value){ _value = static_cast<Enum>(value); }

RequestType::operator uint8_t (){ return static_cast<uint8_t>(_value); }

RequestType & RequestType::operator ++ (int){ _value = static_cast<Enum>(static_cast<uint8_t>(_value) + 1); return *this; }

Bounds RequestType::getComplementBounds ()
{
	switch (_value)
	{
		case lightModeRgb: case lightModePower: case lightModeSpeed:
			return { LightMode::MIN, LightMode::MAX };

		case soundCommand:
			return{ SoundCommand::MIN, SoundCommand::MAX };

		default:
			return{ 0, 0 };
	}
}

Bounds RequestType::getInformationBounds ()
{
	switch (_value)
	{
		case unknown: case requestTime: case requestData: case N:
			return { 0, 0 };

		case provideTime:
			return { 0, -1UL };

		case lightModeRgb:
			return { LightRgb::MIN, LightRgb::MAX };

		case lightOnOff:
		case soundOnOff:
			return{ 0, 1 };

		case lightModePower:
		case lightModeSpeed:
			return { Percentage::MIN, Percentage::MAX };

		case lightMode:
			return { LightMode::MIN, LightMode::MAX };

		case soundMode:
			return { SoundMode::MIN, SoundMode::MAX };

		case soundVolume:
			return { SoundVolume::MIN, SoundVolume::MAX };

		case soundCommand:
			return { SoundCommand::MIN, SoundCommand::MAX };

		case dawnTime:
			return { 0, 1439 };
	}

	return { 0, 0 };
} // RequestType::getInformationBounds

ComplementCategory RequestType::getComplementType ()
{
	switch (_value)
	{
		case lightModeRgb: case lightModePower: case lightModeSpeed:
			return ComplementCategory::lightMode;

		case soundCommand:
			return ComplementCategory::soundCommand;

		default:
			return ComplementCategory::none;
	}
}

const String RequestType::toString (bool shortened) const { return getArrayString (_value, (shortened ? messageTypeName : messageTypeDisplayName)); }

RequestType RequestType::operator = (const String & typeString)
{
	// Test correspondance for each type
	for (RequestType type = MIN; type <= MAX; type++)
		if (typeString == type) // If there is a match, we return it
			return *this = type;

	return *this = RequestType::unknown;
}

bool operator == (const String & string, const RequestType & type){ return string.equals (type.toString (true)); }


/****************************** Sound Mode ******************************/
SoundMode::SoundMode(){ }

SoundMode::SoundMode (uint8_t value){ _value = static_cast<Enum>(value); }

SoundMode::operator uint8_t (){ return (uint8_t) _value; }

const String SoundMode::toString () const { return getArrayString (_value, soundModeName); }


/****************************** Sound Command ******************************/
SoundCommand::SoundCommand() : _value (MIN){ }

SoundCommand::SoundCommand (uint8_t value){ _value = static_cast<Enum>(value); }

SoundCommand::operator uint8_t (){ return (uint8_t) _value; }

const String SoundCommand::toString () const { return getArrayString (_value, soundCommandName); }