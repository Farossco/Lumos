#include "Types.h"
#include "Resources.h"
#include "Utils.h"
#include "ArduinoLogger.h"
#include "light.h"

#define getArrayString(in, array) ((in < (sizeof(array) / sizeof(*array))) ? array[in] : nameUnknown)

/****************************** RequestError ******************************/
RequestError::RequestError () : _value(static_cast<Enum>(0)) {}

RequestError::RequestError(uint8_t value) : _value(constrain(static_cast<Enum>(value), MIN, MAX)) {}

RequestError::operator uint8_t () { return static_cast<uint8_t>(_value); }

const String RequestError::toString() const { return getArrayString(_value, errorName); }

ostream & operator << (ostream & os, const RequestError & error)
{
	return os << error.toString() << " (" << error._value << ")";
}

/****************************** RequestType ******************************/
RequestType::RequestType() : _value(static_cast<Enum>(0)) {}

RequestType::RequestType(uint8_t value) : _value(constrain(static_cast<Enum>(value), MIN, MAX)) {}

RequestType::operator uint8_t () { return static_cast<uint8_t>(_value); }

RequestType & RequestType::operator ++ (int)
{
	_value = static_cast<Enum>(static_cast<uint8_t>(_value) + 1);
	return *this;
}

Bounds RequestType::getComplementBounds()
{
	switch (_value) {
	case lightModeRgb: case lightModePower: case lightModeSpeed:
		return { LIGHT_MODE_MIN, LIGHT_MODE_MAX };

	case soundCommand:
		return{ 0, 0 };

	default:
		return{ 0, 0 };
	}
}

Bounds RequestType::getInformationBounds()
{
	switch (_value) {
	case unknown: case requestTime: case requestData: case N:
		return { 0, 0 };

	case provideTime:
		return { 0, -1UL };

	case lightModeRgb:
		return { 0, 0xFFFFFF };

	case lightOnOff:
	case soundOnOff:
		return{ 0, 1 };

	case lightModePower:
	case lightModeSpeed:
	case soundVolume:
	case alarmDawnVolume:
		return { 0, 255 };

	case lightMode:
		return { LIGHT_MODE_MIN, LIGHT_MODE_MAX };

	case soundMode:
		return { SoundMode::MIN, SoundMode::MAX };

	case soundCommand:
		return { 0, 0 };

	case alarmDawnTime:
		return { 0, 1440 };

	case alarmDawnDuration:
	case alarmSunsetDuration:
	case alarmSunsetDecreaseTime:
		return  { 0, 1440 };
	}

	return { 0, 0 };
} /* RequestType::getInformationBounds */

ComplementCategory RequestType::getComplementType()
{
	switch (_value) {
	case lightModeRgb: case lightModePower: case lightModeSpeed:
		return ComplementCategory::lightMode;

	case soundCommand:
		return ComplementCategory::soundCommand;

	default:
		return ComplementCategory::none;
	}
}

bool RequestType::needsComplement() { return getComplementType() != ComplementCategory::none; }

const String RequestType::toString(bool shortened) const
{
	return getArrayString(_value, (shortened ? messageTypeName : messageTypeDisplayName));
}

RequestType RequestType::operator = (const String & typeString)
{
	/* Test correspondance for each type */
	for (RequestType type; type < N; type++)
		if (typeString == type) /* If there is a match, we return it */
			return *this = type;

	return *this = RequestType::unknown;
}

bool operator == (const String & string, const RequestType & type) { return string.equals(type.toString(true)); }

ostream & operator << (ostream & os, const RequestType & type)
{
	return os << type.toString() << " (" << type._value << ")";
}
