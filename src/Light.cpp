#include "Light.h"
#include "Logger.h"
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

void Light::setRed (uint8_t newRed, uint8_t affectedMode)
{
	if (affectedMode == CURRENT_MODE)
		affectedMode = mode;

	red[affectedMode] = constrain (newRed, LIGHT_MIN_COLOR, LIGHT_MAX_COLOR);
}

void Light::setGreen (uint8_t newGreen, uint8_t affectedMode)
{
	if (affectedMode == CURRENT_MODE)
		affectedMode = mode;

	green[affectedMode] = constrain (newGreen, LIGHT_MIN_COLOR, LIGHT_MAX_COLOR);
}

void Light::setBlue (uint8_t newBlue, uint8_t affectedMode)
{
	if (affectedMode == CURRENT_MODE)
		affectedMode = mode;

	blue[affectedMode] = constrain (newBlue, LIGHT_MIN_COLOR, LIGHT_MAX_COLOR);
}

void Light::setRgb (uint32_t newRgb, uint8_t affectedMode)
{
	if (affectedMode == CURRENT_MODE)
		affectedMode = mode;

	newRgb = constrain (newRgb, LIGHT_MIN_RGB, LIGHT_MAX_RGB);

	red[affectedMode]   = constrain ((uint8_t) (newRgb >> 16), LIGHT_MIN_COLOR, LIGHT_MAX_COLOR);
	green[affectedMode] = constrain ((uint8_t) (newRgb >> 8), LIGHT_MIN_COLOR, LIGHT_MAX_COLOR);
	blue[affectedMode]  = constrain ((uint8_t) newRgb, LIGHT_MIN_COLOR, LIGHT_MAX_COLOR);
}

void Light::setPowerRaw (uint8_t newPower, uint8_t affectedMode)
{
	if (affectedMode == CURRENT_MODE)
		affectedMode = mode;

	power[affectedMode] = constrain (newPower, LIGHT_MIN_POWER, LIGHT_MAX_POWER);
}

void Light::setPowerPercent (uint8_t newPower, uint8_t affectedMode)
{
	if (affectedMode == CURRENT_MODE)
		affectedMode = mode;

	power[affectedMode] = utils.map (constrain (newPower, SEEKBAR_MIN, SEEKBAR_MAX), SEEKBAR_MIN, SEEKBAR_MAX, LIGHT_MIN_POWER, LIGHT_MAX_POWER);
}

void Light::setSpeedRaw (uint16_t newSpeed, uint8_t affectedMode)
{
	if (affectedMode == CURRENT_MODE)
		affectedMode = mode;

	if (LIGHT_MAX_SPEED[affectedMode] == 0)
		speed[affectedMode] = newSpeed;
	else
		speed[affectedMode] = constrain (newSpeed, LIGHT_MIN_SPEED[affectedMode], LIGHT_MAX_SPEED[affectedMode]);
}

void Light::setSpeedPercent (uint16_t newSpeed, uint8_t affectedMode)
{
	if (affectedMode == CURRENT_MODE)
		affectedMode = mode;

	if (LIGHT_MAX_SPEED[affectedMode] == 0)
		speed[affectedMode] = newSpeed;
	else
		speed[affectedMode] = utils.map (constrain (newSpeed, SEEKBAR_MIN, SEEKBAR_MAX), SEEKBAR_MIN, SEEKBAR_MAX, LIGHT_MIN_SPEED[affectedMode], LIGHT_MAX_SPEED[affectedMode]);
}

void Light::setMode (uint8_t newMode)
{
	mode = constrain (newMode, LIGHT_MODE_MIN, LIGHT_MODE_MAX);
}

void Light::switchOn ()
{
	on = true;
}

void Light::switchOff ()
{
	on = false;
}

uint8_t Light::addPower (uint8_t powerAdd, uint8_t affectedMode)
{
	if (affectedMode == CURRENT_MODE)
		affectedMode = mode;

	setPowerPercent (constrain (getPowerPercent() + powerAdd, SEEKBAR_MIN, SEEKBAR_MAX));

	return getPowerPercent();
}

uint8_t Light::subtractPower (uint8_t powerSub, uint8_t affectedMode)
{
	if (affectedMode == CURRENT_MODE)
		affectedMode = mode;

	setPowerPercent (constrain (getPowerPercent() - powerSub, SEEKBAR_MIN, SEEKBAR_MAX));

	return getPowerPercent();
}

uint8_t Light::getRed (uint8_t affectedMode)
{
	return red[affectedMode == CURRENT_MODE ? mode : affectedMode];
}

uint8_t Light::getGreen (uint8_t affectedMode)
{
	return green[affectedMode == CURRENT_MODE ? mode : affectedMode];
}

uint8_t Light::getBlue (uint8_t affectedMode)
{
	return blue[affectedMode == CURRENT_MODE ? mode : affectedMode];
}

uint32_t Light::getRgb (uint8_t affectedMode)
{
	return (((uint32_t) red[affectedMode == CURRENT_MODE ? mode : affectedMode]) << 16) + (((uint32_t) green[affectedMode == CURRENT_MODE ? mode : affectedMode]) << 8) + blue[affectedMode == CURRENT_MODE ? mode : affectedMode];
}

uint8_t Light::getPowerRaw (uint8_t affectedMode)
{
	return power[affectedMode == CURRENT_MODE ? mode : affectedMode];
}

uint8_t Light::getPowerPercent (uint8_t affectedMode)
{
	return utils.map (power[affectedMode == CURRENT_MODE ? mode : affectedMode], LIGHT_MIN_POWER, LIGHT_MAX_POWER, SEEKBAR_MIN, SEEKBAR_MAX);
}

uint16_t Light::getSpeedRaw (uint8_t affectedMode)
{
	return speed[affectedMode == CURRENT_MODE ? mode : affectedMode];
}

uint16_t Light::getSpeedPercent (uint8_t affectedMode)
{
	if (affectedMode == CURRENT_MODE)
		affectedMode = mode;

	if (LIGHT_MAX_SPEED[affectedMode] == 0)
		return getSpeedRaw (affectedMode);

	return utils.map (speed[affectedMode], LIGHT_MIN_SPEED[affectedMode], LIGHT_MAX_SPEED[affectedMode], SEEKBAR_MIN, SEEKBAR_MAX);
}

uint8_t Light::getMode ()
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

	lastMode = LIGHT_MODE_CONTINUOUS; // Initialiazing last mode as well

	if (LIGHT_START_ANIMATION_ENABLE)
	{
		mode = LIGHT_MODE_START_ANIM;
		switchOn();
	}
	else
		mode = LIGHT_MODE_CONTINUOUS;

	inf << "Light initialized." << dendl;
}

void Light::reset ()
{
	for (int i = LIGHT_MODE_MIN; i < LIGHT_MODE_N; i++)
	{
		red[i]   = (uint8_t) LIGHT_DEFAULT_RGB;         // Initialize colors to their default value
		green[i] = (uint8_t) (LIGHT_DEFAULT_RGB >> 8);  // Initialize colors to their default value
		blue[i]  = (uint8_t) (LIGHT_DEFAULT_RGB >> 16); // Initialize colors to their default value
		power[i] = LIGHT_DEFAULT_POWER;                 // Initializing powers their default value
		speed[i] = LIGHT_DEFAULT_SPEED[i];              // Initializing speeds their default value
	}

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
		lastMode = -1;
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