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

void Light::setRed (LightColor newRed, LightMode affectedMode)
{
	rgbs[affectedMode] = LightRgb ((rgbs[affectedMode] & 0x00FFFF) | (newRed << 16));
}

void Light::setGreen (LightColor newGreen, LightMode affectedMode)
{
	rgbs[affectedMode] = LightRgb ((rgbs[affectedMode] & 0xFF00FF) | (newGreen << 8));
}

void Light::setBlue (LightColor newBlue, LightMode affectedMode)
{
	rgbs[affectedMode] = LightRgb ((rgbs[affectedMode] & 0xFFFF00) | (newBlue << 0));
}

void Light::setRgb (LightRgb newRgb, LightMode affectedMode)
{
	rgbs[affectedMode] = newRgb;
}

void Light::setPower (Percentage newPower, LightMode affectedMode)
{
	powers[affectedMode] = newPower;
}

void Light::setSpeed (Percentage newSpeed, LightMode affectedMode)
{
	speeds[affectedMode] = newSpeed;
}

void Light::setMode (LightMode newMode)
{
	mode = constrain (newMode, LightMode::MIN, LightMode::MAX);
}

void Light::switchOn ()
{
	on = LightOnOff (true);
}

void Light::switchOff ()
{
	on = LightOnOff (false);
}

void Light::addPower (Percentage powerAdd, LightMode affectedMode)
{
	powers[affectedMode] += powerAdd;
}

void Light::subtractPower (Percentage powerSub, LightMode affectedMode)
{
	powers[affectedMode] -= powerSub;
}

LightColor Light::getRed (LightMode affectedMode)
{
	return rgbs[affectedMode] >> 16;
}

LightColor Light::getGreen (LightMode affectedMode)
{
	return rgbs[affectedMode] >> 8;
}

LightColor Light::getBlue (LightMode affectedMode)
{
	return rgbs[affectedMode] >> 0;
}

LightRgb Light::getRgb (LightMode affectedMode)
{
	return rgbs[affectedMode];
}

LightPower Light::getPowerRaw (LightMode affectedMode)
{
	return powers[affectedMode];
}

Percentage Light::getPowerPercent (LightMode affectedMode)
{
	return powers[affectedMode].toPercent();
}

LightSpeed Light::getSpeedRaw (LightMode affectedMode)
{
	return speeds[affectedMode];
}

Percentage Light::getSpeedPercent (LightMode affectedMode)
{
	return speeds[affectedMode].toPercent();
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
	rgbs    = LightRgb::DEF;   // Initializing rgbs their default value
	powers  = LightPower::DEF; // Initializing powers their default value
	speeds  = LightSpeed::DEF; // Initializing speeds their default value
	timings = LightTiming::DEF;

	memory.writeLight();
}

void Light::lightAll (LightColor red, LightColor green, LightColor blue)
{
	for (int i = 0; i < STRIP_LENGTH; i++)
		strip.setPixelColor (i, red.value(), green.value(), blue.value());
}

void Light::lightAll (LightRgb rgb)
{
	for (int i = 0; i < STRIP_LENGTH; i++)
		strip.setPixelColor (i, rgb.value());
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

	strip.setBrightness (powers[mode].value());
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