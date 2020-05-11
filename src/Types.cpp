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


/****************************** LightRgb ******************************/
LightRgb::LightRgb() : SettingBase(){ }

LightRgb::LightRgb (uint32_t value) : SettingBase (value){ }

LightRgb LightRgb::setRed (LightColor red){ *(((uint8_t *) &_value) + 2) = red.value(); return *this; }

LightRgb LightRgb::setGreen (LightColor green){ *(((uint8_t *) &_value) + 1) = green.value(); return *this; }

LightRgb LightRgb::setBlue (LightColor blue){ *(((uint8_t *) &_value) + 0) = blue.value(); return *this; }

LightRgb LightRgb::setHue (uint8_t hue)
{
	hue %= 360;
	uint8_t x = (60 - abs (((hue % 120)) - 60)) * 255 / 60;

	if (0 <= hue && hue < 60)
	{
		setRed (100);
		setGreen (x);
		setBlue (0);
	}
	else if (60 <= hue && hue < 120)
	{
		setRed (x);
		setGreen (100);
		setBlue (0);
	}
	else if (120 <= hue && hue < 180)
	{
		setRed (0);
		setGreen (100);
		setBlue (x);
	}
	else if (180 <= hue && hue < 240)
	{
		setRed (0);
		setGreen (x);
		setBlue (100);
	}
	else if (240 <= hue && hue < 300)
	{
		setRed (x);
		setGreen (0);
		setBlue (100);
	}
	else if (300 <= hue && hue <= 360)
	{
		setRed (100);
		setGreen (0);
		setBlue (x);
	}

	return *this;
} // LightRgb::setHue

LightColor LightRgb::getRed () const { return *(((uint8_t *) &_value) + 2); }

LightColor LightRgb::getGreen () const { return *(((uint8_t *) &_value) + 1); }

LightColor LightRgb::getBlue () const { return *(((uint8_t *) &_value) + 0); }

LightRgb LightRgb::operator * (double value) const
{
	LightRgb rgb;

	*(&rgb._value + 0) = *(&_value + 0) * value;
	*(&rgb._value + 1) = *(&_value + 1) * value;
	*(&rgb._value + 2) = *(&_value + 2) * value;

	return rgb;
}

LightRgb LightRgb::operator / (double value) const
{
	LightRgb rgb;
	uint8_t * pSelf = (uint8_t *) &_value;
	uint8_t * pNew  = (uint8_t *) &rgb._value;

	for (int i = 0; i < 3; i++)
		*(pNew + i) = *(pSelf + i) / value;

	return rgb;
}

ostream & operator << (ostream & os, const LightRgb & rgb){ return os << "0x" << hex << noshowbase << setfill ('0') << setw (6) << rgb._value << dec << " (R" << rgb.getRed().value() << "/G" << rgb.getGreen().value() << "/B" << rgb.getBlue().value() << ")"; }


/****************************** Timing ******************************/
Timing::Timing () : SettingBase(){ }

Timing::Timing (uint32_t time) : SettingBase (time){ }

Timing::Timing (uint8_t minute, uint8_t second){ _value = minute * 60 + second; }

uint8_t Timing::minute () const { return _value / 60; }

uint8_t Timing::second () const { return _value % 60; }

ostream & operator << (ostream & os, const Timing & timing){ return os << timing.minute() << "min " << timing.second() << "s (" << timing._value << ")"; }


/****************************** Time ******************************/
Time::Time () : SettingBase(){ }

Time::Time (uint16_t time) : SettingBase (time){ }

Time::Time (uint8_t hour, uint8_t minute){ _value = hour * 60 + minute; }

uint8_t Time::hour () const { return _value / 60; }

uint8_t Time::minute () const { return _value % 60; }

ostream & operator << (ostream & os, const Time & time){ return os << time.hour() << "h" << time.minute() << " (" << time._value << ")"; }


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
		case soundVolume:
		case alarmDawnVolume:
			return { Percentage::MIN, Percentage::MAX };

		case lightMode:
			return { LightMode::MIN, LightMode::MAX };

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

bool RequestType::needsComplement (){ return getComplementType() != ComplementCategory::none; }

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