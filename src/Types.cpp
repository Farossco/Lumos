#include "Types.h"

/****************************** LightMode ******************************/
LightMode::LightMode() : _value (MIN){ }

LightMode::LightMode (uint8_t value){ _value = static_cast<LightModeEnum>(value); }

LightMode::operator uint8_t (){ return _value; }


/****************************** LightSetting ******************************/
uint8_t & LightSetting::operator [] (LightMode mode)
{
	return _value[constrain (mode, LightMode::MIN, LightMode::MAX)];
}

LightSetting LightSetting::operator = (uint8_t value)
{
	for (LightMode i = LightMode::MIN; i <= LightMode::MAX; i++)
		_value[i] = value;

	return *this;
}

LightSetting LightSetting::operator = (LightSetting & copy)
{
	for (LightMode i = LightMode::MIN; i <= LightMode::MAX; i++)
		_value[i] = copy._value[i];

	return *this;
}

/****************************** RequestError ******************************/
RequestError::RequestError (){ }

RequestError::RequestError(uint8_t value){ _value = static_cast<RequestErrorEnum>(value); }

RequestError::operator uint8_t (){ return (uint8_t) _value; }

/****************************** RequestType ******************************/
RequestType::RequestType(){ }

RequestType::RequestType(uint8_t value){ _value = static_cast<RequestTypeEnum>(value); }

RequestType::operator uint8_t (){ return static_cast<uint8_t>(_value); }

RequestType & RequestType::operator ++ (int){ _value = static_cast<RequestTypeEnum>(static_cast<uint8_t>(_value) + 1); return *this; }

Bounds RequestType::getComplementBounds ()
{
	switch (_value)
	{
		case lightRgb: case lightPower: case lightModeSpeed:
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
		case unknown: case requestTime: case requestInfos: case N:
			return { 0, 0 };

		case provideTime:
			return { 0, -1UL };

		case lightRgb:
			return { LightSetting::MIN_RGB, LightSetting::MAX_RGB };

		case lightOnOff:
		case soundOnOff:
			return{ 0, 1 };

		case lightPower:
		case lightModeSpeed:
			return { LightSetting::MIN_PERCENT, LightSetting::MAX_PERCENT };

		case lightMode:
			return { LightMode::MIN, LightMode::MAX };

		case soundMode:
			return { SoundMode::MIN, SoundMode::MAX };

		case soundVolume:
			return { SoundSetting::MIN_SOUND, SoundSetting::MAX_SOUND };

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
		case lightRgb: case lightPower: case lightModeSpeed:
			return ComplementCategory::lightMode;

		case soundCommand:
			return ComplementCategory::soundCommand;

		default:
			return ComplementCategory::none;
	}
}

/****************************** Sound Mode ******************************/
SoundMode::SoundMode(){ }

SoundMode::SoundMode (uint8_t value){ _value = static_cast<SoundModeEnum>(value); }

SoundMode::operator uint8_t (){ return (uint8_t) _value; }


/****************************** Sound Command ******************************/
SoundCommand::SoundCommand() : _value (MIN){ }

SoundCommand::SoundCommand (uint8_t value){ _value = static_cast<SoundCommandEnum>(value); }

SoundCommand::operator uint8_t (){ return (uint8_t) _value; }


/****************************** Sound Setting ******************************/
SoundSetting::SoundSetting(){ }

SoundSetting::SoundSetting (uint8_t value){ _value = value; }

SoundSetting::operator uint8_t (){ return _value; }