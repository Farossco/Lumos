#include "Light.h"
#include "ArduinoLogger.h"
#include "Memory.h"
#include "Sound.h"

#if defined(LUMOS_ARDUINO_MEGA)

Light::Light() : strip (STRIP_LENGTH, DOTSTAR_BGR)
{
	pinMode (PIN_STRIP_CS, OUTPUT);
	digitalWrite (PIN_STRIP_CS, LOW);
}

#endif

#if defined(LUMOS_ESP8266)

Light::Light()
{ }

#endif

void Light::setRed (uint8_t newRed, LightMode affectedMode)
{
	red[affectedMode] = constrain (newRed, LightSetting::MIN_COLOR, LightSetting::MAX_COLOR);
}

void Light::setGreen (uint8_t newGreen, LightMode affectedMode)
{
	green[affectedMode] = constrain (newGreen, LightSetting::MIN_COLOR, LightSetting::MAX_COLOR);
}

void Light::setBlue (uint8_t newBlue, LightMode affectedMode)
{
	blue[affectedMode] = constrain (newBlue, LightSetting::MIN_COLOR, LightSetting::MAX_COLOR);
}

void Light::setRgb (uint32_t newRgb, LightMode affectedMode)
{
	newRgb = constrain (newRgb, LightSetting::MIN_RGB, LightSetting::MAX_RGB);

	red[affectedMode]   = constrain ((uint8_t) (newRgb >> 16), LightSetting::MIN_COLOR, LightSetting::MAX_COLOR);
	green[affectedMode] = constrain ((uint8_t) (newRgb >> 8), LightSetting::MIN_COLOR, LightSetting::MAX_COLOR);
	blue[affectedMode]  = constrain ((uint8_t) newRgb, LightSetting::MIN_COLOR, LightSetting::MAX_COLOR);
}

void Light::setPowerRaw (uint8_t newPower, LightMode affectedMode)
{
	power[affectedMode] = constrain (newPower, LightSetting::MIN_POWER, LightSetting::MAX_POWER);
}

void Light::setPowerPercent (uint8_t newPower, LightMode affectedMode)
{
	power[affectedMode] = utils.map (constrain (newPower, LightSetting::MIN_PERCENT, LightSetting::MAX_PERCENT), LightSetting::MIN_PERCENT, LightSetting::MAX_PERCENT, LightSetting::MIN_POWER, LightSetting::MAX_POWER);
}

void Light::setSpeedRaw (uint16_t newSpeed, LightMode affectedMode)
{
	speed[affectedMode] = constrain (newSpeed, LightSetting::MIN_SPEED, LightSetting::MAX_SPEED);
}

void Light::setSpeedPercent (uint16_t newSpeed, LightMode affectedMode)
{
	speed[affectedMode] = utils.map (constrain (newSpeed, LightSetting::MIN_PERCENT, LightSetting::MAX_PERCENT), LightSetting::MIN_PERCENT, LightSetting::MAX_PERCENT, LightSetting::MIN_SPEED, LightSetting::MAX_SPEED);
}

void Light::setMode (LightMode newMode)
{
	mode = constrain (newMode, LightMode::MIN, LightMode::MAX);
}

void Light::switchOn ()
{
	on = true;
}

void Light::switchOff ()
{
	on = false;
}

uint8_t Light::addPower (uint8_t powerAdd, LightMode affectedMode)
{
	setPowerPercent (constrain (getPowerPercent() + powerAdd, LightSetting::MIN_PERCENT, LightSetting::MAX_PERCENT));

	return getPowerPercent();
}

uint8_t Light::subtractPower (uint8_t powerSub, LightMode affectedMode)
{
	setPowerPercent (constrain (getPowerPercent() - powerSub, LightSetting::MIN_PERCENT, LightSetting::MAX_PERCENT));

	return getPowerPercent();
}

uint8_t Light::getRed (LightMode affectedMode)
{
	return red[affectedMode];
}

uint8_t Light::getGreen (LightMode affectedMode)
{
	return green[affectedMode];
}

uint8_t Light::getBlue (LightMode affectedMode)
{
	return blue[affectedMode];
}

uint32_t Light::getRgb (LightMode affectedMode)
{
	return (((uint32_t) red[affectedMode]) << 16) + (((uint32_t) green[affectedMode]) << 8) + blue[affectedMode];
}

uint8_t Light::getPowerRaw (LightMode affectedMode)
{
	return power[affectedMode];
}

uint8_t Light::getPowerPercent (LightMode affectedMode)
{
	return utils.map (power[affectedMode], LightSetting::MIN_POWER, LightSetting::MAX_POWER, LightSetting::MIN_PERCENT, LightSetting::MAX_PERCENT);
}

uint8_t Light::getSpeedRaw (LightMode affectedMode)
{
	return speed[affectedMode];
}

uint8_t Light::getSpeedPercent (LightMode affectedMode)
{
	return utils.map (speed[affectedMode], LightSetting::MIN_SPEED, LightSetting::MAX_SPEED, LightSetting::MIN_PERCENT, LightSetting::MAX_PERCENT);
}

LightMode Light::getMode ()
{
	return mode;
}

bool Light::isOn ()
{
	return on != 0;
}

bool Light::isOff ()
{
	return on == 0;
}

#if defined(LUMOS_ARDUINO_MEGA)

void Light::init ()
{
	strip.begin();

	if (memory.readLight()) // Returns True if EEPROM is not correctly initialized (This may be the first launch)
	{
		inf << "This is first launch, light variables will be initialized to their default values" << endl;

		reset();
	}

	light.lightAll (0x000000);
	show();

	lastMode = LightMode::continuous; // Initialiazing last mode as well

	if (LIGHT_START_ANIMATION_ENABLE)
	{
		mode = LightMode::startAnimation;
		switchOn();
	}
	else
		mode = LightMode::continuous;

	inf << "Light initialized." << dendl;
}

void Light::reset ()
{
	red   = (uint8_t) LightSetting::DEF_RGB;         // Initialize colors to their default value
	green = (uint8_t) (LightSetting::DEF_RGB >> 8);  // Initialize colors to their default value
	blue  = (uint8_t) (LightSetting::DEF_RGB >> 16); // Initialize colors to their default value
	power = LightSetting::DEF_POWER;                 // Initializing powers their default value
	speed = LightSetting::DEF_SPEED;                 // Initializing speeds their default value

	memory.writeLight();
}

void Light::lightAll (uint8_t red, uint8_t green, uint8_t blue)
{
	for (int i = 0; i < STRIP_LENGTH; i++)
		strip.setPixelColor (i, red, green, blue);
}

void Light::lightAll (uint32_t rgb)
{
	for (int i = 0; i < STRIP_LENGTH; i++)
		strip.setPixelColor (i, rgb);
}

// Perform mode action
void Light::action ()
{
	// If lightning is off, shut all lights
	if (isOff())
	{
		lightAll (0);
		show();
		lastMode = LightMode::continuous;
		return;
	}

	modeActions();

	strip.setBrightness (power[mode]);
	show();
} // action

void Light::show ()
{
	digitalWrite (PIN_STRIP_CS, HIGH);
	strip.show();
	digitalWrite (PIN_STRIP_CS, LOW);
}

#endif // if defined(LUMOS_ARDUINO_MEGA)

Light light = Light();