#include "Types.h"
#include "Resources.h"
#include "Utils.h"
#include "ArduinoLogger.h"
#include "light_mode.h"

#define getArrayString(in, array) ((in < (sizeof(array) / sizeof(*array))) ? array[in] : nameUnknown)

/****************************** Percentage ******************************/
Percentage::Percentage (uint8_t value) : _value(constrain(value, MIN, MAX)) {}


/****************************** LightRgb ******************************/
LightRgb::LightRgb() {}

LightRgb::LightRgb (uint32_t rgb) { _value.rgb = rgb; }

LightRgb::LightRgb (uint8_t red, uint8_t green, uint8_t blue) { setRed(red); setGreen(green); setBlue(blue); }

LightRgb LightRgb::setRed(uint8_t red) { _value.r = red; return *this; }

LightRgb LightRgb::setGreen(uint8_t green) { _value.g = green; return *this; }

LightRgb LightRgb::setBlue(uint8_t blue) { _value.b = blue; return *this; }

LightRgb LightRgb::setHue(uint8_t hue)
{
	hue %= 360;
	uint8_t x = (60 - abs(((hue % 120)) - 60)) * 255 / 60;

	if (0 <= hue && hue < 60) {
		setRed(100);
		setGreen(x);
		setBlue(0);
	} else if (60 <= hue && hue < 120) {
		setRed(x);
		setGreen(100);
		setBlue(0);
	} else if (120 <= hue && hue < 180) {
		setRed(0);
		setGreen(100);
		setBlue(x);
	} else if (180 <= hue && hue < 240) {
		setRed(0);
		setGreen(x);
		setBlue(100);
	} else if (240 <= hue && hue < 300) {
		setRed(x);
		setGreen(0);
		setBlue(100);
	} else if (300 <= hue && hue <= 360) {
		setRed(100);
		setGreen(0);
		setBlue(x);
	}

	return *this;
} /* LightRgb::setHue */

uint32_t LightRgb::value() const { return _value.rgb; }

uint8_t LightRgb::getRed() const { return _value.r; }

uint8_t LightRgb::getGreen() const { return _value.g; }

uint8_t LightRgb::getBlue() const { return _value.b; }

LightRgb LightRgb::operator * (double value) const
{
	uint8_t r, g, b;

	if (_value.r * value > 255) {
		r = 255;
	} else {
		r = (uint8_t)(_value.r * value);
	}

	if (_value.g * value > 255) {
		g = 255;
	} else {
		g = (uint8_t)(_value.g * value);
	}

	if (_value.b * value > 255) {
		b = 255;
	} else {
		b = (uint8_t)(_value.b * value);
	}
	return LightRgb(r, g, b);
}

LightRgb LightRgb::operator / (double value) const
{
	if (value == 0) {
		return LightRgb(0xFFFFFF);
	} else {
		return LightRgb(_value.r / value,
		                _value.g / value,
		                _value.b / value);
	}
}

ostream & operator << (ostream & os, const LightRgb & rgb)
{
	return os << "0x" << hex << noshowbase << setfill('0') << setw(6) << rgb._value.rgb <<
	       dec << " (R" << rgb._value.r << "/G" << rgb._value.g << "/B" << rgb._value.b << ")";
}

/****************************** Timing ******************************/
Timing::Timing () : SettingBase() {}

Timing::Timing (uint32_t time) : SettingBase(time) {}

Timing::Timing (uint8_t minute, uint8_t second) { _value = minute * 60 + second; }

uint8_t Timing::minute() const { return _value / 60; }

uint8_t Timing::second() const { return _value % 60; }

ostream & operator << (ostream & os, const Timing & timing)
{
	return os << timing.minute() << "min " << timing.second() << "s (" << timing._value << ")";
}

/****************************** Time ******************************/
Time::Time () : SettingBase() {}

Time::Time (uint16_t time) : SettingBase(time) {}

Time::Time (uint8_t hour, uint8_t minute) { _value = hour * 60 + minute; }

uint8_t Time::hour() const { return _value / 60; }

uint8_t Time::minute() const { return _value % 60; }

ostream & operator << (ostream & os, const Time & time)
{
	return os << time.hour() << "h" << time.minute() << " (" << time._value << ")";
}

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
		return{ SoundCommand::MIN, SoundCommand::MAX };

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
		return { LightRgb::MIN, LightRgb::MAX };

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
		return { SoundCommand::MIN, SoundCommand::MAX };

	case alarmDawnTime:
		return { Time::MIN, Time::MAX };

	case alarmDawnDuration:
	case alarmSunsetDuration:
	case alarmSunsetDecreaseTime:
		return  { Timing::MIN, Timing::MAX };
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

/****************************** Sound Mode ******************************/
SoundMode::SoundMode() : _value(static_cast<Enum>(0)) {}

SoundMode::SoundMode (uint8_t value) : _value(constrain(static_cast<Enum>(value), MIN, MAX)) {}

SoundMode::operator uint8_t () { return (uint8_t)_value; }

const String SoundMode::toString() const { return getArrayString(_value, soundModeName); }

ostream & operator << (ostream & os, const SoundMode & mode)
{
	return os << mode.toString() << " (" << mode._value << ")";
}

/****************************** Sound Command ******************************/
SoundCommand::SoundCommand() : _value(static_cast<Enum>(0)) {}

SoundCommand::SoundCommand (uint8_t value) : _value(constrain(static_cast<Enum>(value), MIN, MAX)) {}

SoundCommand::operator uint8_t () { return (uint8_t)_value; }

const String SoundCommand::toString() const { return getArrayString(_value, soundCommandName); }

ostream & operator << (ostream & os, const SoundCommand & command)
{
	return os << command.toString() << " (" << command._value << ")";
}
