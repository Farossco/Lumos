#include "Light.h"
#include "ArduinoLogger.h"
#include "Memory.h"
#include "Sound.h"
#include "SerialCom.h"

#if defined(LUMOS_ARDUINO_MEGA)

Light::Light() : strip (LIGHT_STRIP_LENGTH, DOTSTAR_BGR)
{
	pinMode (LIGHT_PIN_STRIP_CS, OUTPUT);
	digitalWrite (LIGHT_PIN_STRIP_CS, LOW);

	for (uint8_t i = 0; i < LIGHT_STRIP_HALF_LENGTH + 1; i++)
		rainbow[i].setHue (i * 360 / (LIGHT_STRIP_HALF_LENGTH + 1));
}

#endif // if defined(LUMOS_ARDUINO_MEGA)

#if defined(LUMOS_ESP8266)

Light::Light()
{ }

#endif

void Light::setRed (LightColor newRed, LightMode affectedMode)
{
	rgbs[affectedMode].setRed (newRed);
}

void Light::setGreen (LightColor newGreen, LightMode affectedMode)
{
	rgbs[affectedMode].setGreen (newGreen);
}

void Light::setBlue (LightColor newBlue, LightMode affectedMode)
{
	rgbs[affectedMode].setBlue (newBlue);
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
	return rgbs[affectedMode].getRed();
}

LightColor Light::getGreen (LightMode affectedMode)
{
	return rgbs[affectedMode].getGreen();
}

LightColor Light::getBlue (LightMode affectedMode)
{
	return rgbs[affectedMode].getBlue();
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

	stripUpdateOff();

	mode     = LightMode::continuous;
	lastMode = LightMode::continuous; // Initialiazing last mode as well

	while (!serial.checkTime())
		startAnimWait();

	startAnimDone();

	inf << "Light initialized." << dendl;
}

void Light::reset ()
{
	rgbs   = LightRgb::DEF;   // Initializing rgbs their default value
	powers = LightPower::DEF; // Initializing powers their default value
	speeds = LightSpeed::DEF; // Initializing speeds their default value

	memory.writeLight();
}

void Light::lightAll (LightColor red, LightColor green, LightColor blue)
{
	for (int i = 0; i < LIGHT_STRIP_LENGTH; i++)
		strip.setPixelColor (i, red.value(), green.value(), blue.value());
}

void Light::lightAll (LightRgb rgb)
{
	for (int i = 0; i < LIGHT_STRIP_LENGTH; i++)
		strip.setPixelColor (i, rgb.value());
}

void Light::lightAllOff ()
{
	strip.setBrightness (0);
}

// Perform mode action
void Light::action ()
{
	// If lightning is off, shut all lights
	if (isOff())
	{
		stripUpdateOff();
		lastMode = LightMode::continuous;
		return;
	}

	modeActions();

	stripUpdate (powers[mode]);
}

void Light::stripUpdate (LightPower power)
{
	strip.setBrightness (power.value());

	digitalWrite (LIGHT_PIN_STRIP_CS, HIGH);
	strip.show();
	digitalWrite (LIGHT_PIN_STRIP_CS, LOW);
}

void Light::stripUpdateOff ()
{
	strip.setBrightness (0);

	digitalWrite (LIGHT_PIN_STRIP_CS, HIGH);
	strip.show();
	digitalWrite (LIGHT_PIN_STRIP_CS, LOW);
}

void Light::startAnimWait ()
{
	counter1 = 0;
	state   = 0;

	while (counter1 >= 0)
	{
		for (int8_t i = 0; i < LIGHT_STRIP_LENGTH; i++)
			if (abs (i - LIGHT_STRIP_HALF_LENGTH) < counter1)
				strip.setPixelColor (i, rainbow[LIGHT_STRIP_HALF_LENGTH - abs (i - LIGHT_STRIP_HALF_LENGTH)].value());
			else
				strip.setPixelColor (i, 0);

		stripUpdate (LightPower::DEF);

		delay (abs (strip.sine8 (counter1 * (8 * 16) / LIGHT_STRIP_HALF_LENGTH + 64) - 128) / 5);

		if (counter1 > LIGHT_STRIP_HALF_LENGTH) state = 1;

		if (state)
			counter1--;
		else
			counter1++;
	}
}

void Light::startAnimDone ()
{
	counter1 = 0;

	while (counter1 < LIGHT_STRIP_HALF_LENGTH + LIGHT_TAIL_LENGTH + 2)
	{
		for (int8_t i = 0; i < LIGHT_STRIP_LENGTH; i++)
			if (counter1 > abs (i - LIGHT_STRIP_HALF_LENGTH) && abs (i - LIGHT_STRIP_HALF_LENGTH) >= counter1 - LIGHT_TAIL_LENGTH)
				strip.setPixelColor (i, rainbow[LIGHT_STRIP_HALF_LENGTH - abs (i - LIGHT_STRIP_HALF_LENGTH)].value());
			else
				strip.setPixelColor (i, 0);

		stripUpdate (LightPower::DEF);

		delay (15);

		counter1++;
	}
}

#endif // if defined(LUMOS_ARDUINO_MEGA)

Light light = Light();