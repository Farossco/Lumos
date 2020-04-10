#include "Light.h"
#include "Logger.h"
#include "Memory.h"
#include "Sound.h"

#if defined(LUMOS_ARDUINO_MEGA)

Light::Light() : strip (STRIP_LENGTH, PIN_DATA, PIN_CLOCK, DOTSTAR_BGR)
{ }

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

void Light::setSpeed (uint16_t newSpeed, uint8_t affectedMode)
{
	if (affectedMode == CURRENT_MODE)
		affectedMode = mode;

	speed[affectedMode] = constrain (newSpeed, LIGHT_MIN_SPEED[affectedMode], LIGHT_MAX_SPEED[affectedMode] == 0 ? 65535 : LIGHT_MAX_SPEED[affectedMode]);
}

void Light::setMode (uint8_t newMode)
{
	mode = constrain (newMode, LIGHT_MOD_MIN, LIGHT_MOD_MAX);
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

	return power[affectedMode] = constrain (power[affectedMode] + powerAdd, LIGHT_MIN_POWER, LIGHT_MAX_POWER);
}

uint8_t Light::subtractPower (uint8_t powerAdd, uint8_t affectedMode)
{
	if (affectedMode == CURRENT_MODE)
		affectedMode = mode;

	return power[affectedMode] = constrain (power[affectedMode] - powerAdd, LIGHT_MIN_POWER, LIGHT_MAX_POWER);
}

uint16_t Light::getDawnDuration ()
{
	return speed[LIGHT_MOD_DAWN];
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

uint16_t Light::getSpeed (uint8_t affectedMode)
{
	return speed[affectedMode == CURRENT_MODE ? mode : affectedMode];
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

	if (memory.readForLight()) // Returns True if EEPROM is not correctly initialized (This may be the first launch)
	{
		inf << "This is first launch, light variables will be initialized to their default values" << endl;

		reset();
	}

	light.lightAll (0x000000);
	strip.show();

	lastMode = LIGHT_MOD_CONTINUOUS; // Initialiazing last mode as well

	if (LIGHT_START_ANIMATION_ENABLE)
	{
		mode = LIGHT_MOD_START_ANIM;
		switchOn();
	}
	else
		mode = LIGHT_MOD_CONTINUOUS;

	inf << "Light initialized." << dendl;
}

void Light::reset ()
{
	for (int i = LIGHT_MOD_MIN; i < LIGHT_MOD_N; i++)
	{
		red[i]   = DEFAULT_RED[i];   // Initialize colors to their default value
		green[i] = DEFAULT_GREEN[i]; // Initialize colors to their default value
		blue[i]  = DEFAULT_BLUE[i];  // Initialize colors to their default value
		power[i] = DEFAULT_POWER[i]; // Initializing powers their default value
		speed[i] = DEFAULT_SPEED[i]; // Initializing speeds their default value
	}

	memory.writeForLight();
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
		strip.show();
		lastMode = -1;
		return;
	}

	if (mode != lastMode)
		inf << "Leaving " << utils.getLightModeName (lastMode, CAPS_FIRST) << " mode" << dendl;

	// Calling modes functions
	switch (mode)
	{
		case LIGHT_MOD_CONTINUOUS:
			if (lastMode != LIGHT_MOD_CONTINUOUS) // If this is first call of the function, we call init function (lastMode will be set in init function)
				initContinuous();
			continuous();
			break;

		case LIGHT_MOD_FLASH:
			if (lastMode != LIGHT_MOD_FLASH)
				initFlash();
			flash();
			break;

		case LIGHT_MOD_STROBE:
			if (lastMode != LIGHT_MOD_STROBE)
				initStrobe();
			strobe();
			break;

		case LIGHT_MOD_FADE:
			if (lastMode != LIGHT_MOD_FADE)
				initFade();
			fade();
			break;

		case LIGHT_MOD_SMOOTH:
			if (lastMode != LIGHT_MOD_SMOOTH)
				initSmooth();
			smooth();
			break;

		case LIGHT_MOD_DAWN:
			if (lastMode != LIGHT_MOD_DAWN)
				initDawn();
			dawn();
			break;

		case LIGHT_MOD_SUNSET:
			if (lastMode != LIGHT_MOD_SUNSET)
				initSunset();
			sunset();
			break;

		case LIGHT_MOD_START_ANIM:
			if (lastMode != LIGHT_MOD_START_ANIM)
				initStartAnimation();
			startAnimation();
			break;

		case LIGHT_MOD_MUSIC:
			if (lastMode != LIGHT_MOD_MUSIC)
				initMusic();
			music();
			break;
	}

	strip.setBrightness (power[mode]);
	strip.show();
} // action

// Flash mode initialization
void Light::initContinuous ()
{
	lastMode = LIGHT_MOD_CONTINUOUS; // Setting lastMode so we don't call init again

	inf << "Entering Default mode" << dendl;
}

void Light::continuous ()
{
	lightAll (red[LIGHT_MOD_CONTINUOUS], green[LIGHT_MOD_CONTINUOUS], blue[LIGHT_MOD_CONTINUOUS]);
}

// Flash mode initialization
void Light::initFlash ()
{
	state      = 0;               // Set initial state to 0
	delayCount = -1;              // Reseting milliseconds counter
	lastMode   = LIGHT_MOD_FLASH; // Setting lastMode so we don't call init again

	inf << "Entering Flash mode" << dendl;
}

// Flash mode
void Light::flash ()
{
	if (millis() - delayCount >= (uint32_t) (1000 / speed[LIGHT_MOD_FLASH]))
	{
		if (state >= 2)
			state = 0;
		else
			state++;  // Incrementing state

		lightAll (state == 0 ? 0xFF0000 : state == 1 ? 0x00FF00 : 0x0000FF);
		delayCount = millis();
	}
}

// Strobe mode initialization
void Light::initStrobe ()
{
	state      = 0;                // Set initial state to 0
	delayCount = -1;               // Reseting milliseconds counter
	lastMode   = LIGHT_MOD_STROBE; // Setting lastMode so we don't call init again

	inf << "Entering Strobe mode" << dendl;
}

// Strobe mode
void Light::strobe ()
{
	if (millis() - delayCount >= (uint32_t) (1000 / speed[LIGHT_MOD_STROBE]))
	{
		state = !state; // Inverting state

		if (state)
			lightAll (red[LIGHT_MOD_STROBE], green[LIGHT_MOD_STROBE], blue[LIGHT_MOD_STROBE]);
		else
			lightAll (0x000000);

		delayCount = millis();
	}
}

// Fade Mode initialization
void Light::initFade ()
{
	state      = 0;  // Setting state to Decreasing state
	delayCount = -1; // Reseting milliseconds counter
	counter    = 0;
	lastMode   = LIGHT_MOD_FADE; // Setting lastMode so we don't call init again

	inf << "Entering Fade mode" << dendl;
}

// Fade Mode
void Light::fade ()
{
	if (millis() - delayCount >= (uint32_t) (1000 / speed[LIGHT_MOD_FADE]))
	{
		if (state)
			counter++;  // Increasing all colors
		else
			counter--;  // Decreasing all colors

		if (counter >= LIGHT_MAX_POWER)      // If color reach white, we start to decrease
			state = 0;                       // Decreasing state
		else if (counter <= LIGHT_MIN_POWER) // If color reach black, we start to increase
			state = 1;                       // Increasing state

		lightAll (red[LIGHT_MOD_FADE] * (counter / (float) LIGHT_MAX_POWER), green[LIGHT_MOD_FADE] * (counter / (float) LIGHT_MAX_POWER), blue[LIGHT_MOD_FADE] * (counter / (float) LIGHT_MAX_POWER));

		delayCount = millis();
	}
}

// Smooth Mode Initialization
void Light::initSmooth ()
{
	state      = 0;  // Init state to 0
	delayCount = -1; // Reseting milliseconds counter
	counter    = 0;
	lastMode   = LIGHT_MOD_SMOOTH; // Setting lastMode so we don't call init again

	inf << "Entering Smooth mode" << dendl;
}

// Smooth Mode
void Light::smooth ()
{
	if (millis() - delayCount >= (uint32_t) (1000 / speed[LIGHT_MOD_SMOOTH]))
	{
		switch (state)
		{
			case 0:
				counter++;
				lightAll (0xFF, counter, 0);
				if (counter == 0xFF)
					state = 1;
				break;

			case 1:
				counter--;
				lightAll (counter, 0xFF, 0);
				if (counter == 0x00)
					state = 2;
				break;

			case 2:
				counter++;
				lightAll (0x00, 0xFF, counter);
				if (counter == 0xFF)
					state = 3;
				break;

			case 3:
				counter--;
				lightAll (0x00, counter, 0xFF);
				if (counter == 0x00)
					state = 4;
				break;

			case 4:
				counter++;
				lightAll (counter, 0x00, 0xFF);
				if (counter == 0xFF)
					state = 5;
				break;

			case 5:
				counter--;
				lightAll (0xFF, 0x00, counter);
				if (counter == 0x00)
					state = 0;
				break;

			default:
				state = 0;
				break;
		}
		delayCount = millis();
	}
} // Light::smooth

// Dawn Mode initialization
void Light::initDawn ()
{
	delayCount = -1;             // Reseting milliseconds counter
	lastMode   = LIGHT_MOD_DAWN; // Setting lastMode so we don't call init again
	counter    = 0;
	counter2   = 1;

	lightAll (0x000000);

	inf << "Entering Dawn mode for " << speed[LIGHT_MOD_DAWN] << " min." << dendl;
}

// Dawn Mode
void Light::dawn ()
{
	const uint32_t step = (((uint64_t) speed[LIGHT_MOD_DAWN] ) * 60000.) / ( (((uint64_t) STRIP_LENGTH) / 2.) * ((uint64_t) LIGHT_MAX_POWER));

	if (millis() - delayCount >= step)
	{
		strip.setPixelColor ((STRIP_LENGTH / 2 + counter), getRed (LIGHT_MOD_DAWN) * (counter2 / (float)  LIGHT_MAX_POWER), getGreen (LIGHT_MOD_DAWN) * (counter2 / (float) LIGHT_MAX_POWER), getBlue (LIGHT_MOD_DAWN) * (counter2 / (float) LIGHT_MAX_POWER));
		strip.setPixelColor ((STRIP_LENGTH / 2 - counter), getRed (LIGHT_MOD_DAWN) * (counter2 / (float)  LIGHT_MAX_POWER), getGreen (LIGHT_MOD_DAWN) * (counter2 / (float) LIGHT_MAX_POWER), getBlue (LIGHT_MOD_DAWN) * (counter2 / (float) LIGHT_MAX_POWER));

		if (counter >= (STRIP_LENGTH / 2.))
		{
			counter = 0;

			if (counter2 >= LIGHT_MAX_POWER)
			{
				red [LIGHT_MOD_CONTINUOUS]  = red [LIGHT_MOD_DAWN];  // Transfer RGB final value to default mode
				green[LIGHT_MOD_CONTINUOUS] = green[LIGHT_MOD_DAWN]; // Transfer RGB final value to default mode
				blue[LIGHT_MOD_CONTINUOUS]  = blue[LIGHT_MOD_DAWN];  // Transfer RGB final value to default mode
				power[LIGHT_MOD_CONTINUOUS] = power[LIGHT_MOD_DAWN]; // Same for power
				mode                        = LIGHT_MOD_CONTINUOUS;  // Leaving the mode
			}
			else
			{
				counter2++;
			}
		}
		else
		{
			counter++;
		}

		delayCount = millis();
	}
} // Light::dawn

// Sunset Mode initialization
void Light::initSunset ()
{
	delayCount = millis();         // Reseting milliseconds counter
	lastMode   = LIGHT_MOD_SUNSET; // Setting lastMode so we don't call init again
	counter    = 0;
	counter2   = LIGHT_MAX_POWER;
	state      = 0;

	lightAll (getRgb (LIGHT_MOD_SUNSET));

	inf << "Entering Sunset mode for " << speed[LIGHT_MOD_SUNSET] << " min." << dendl;
}

// Sunset Mode
void Light::sunset ()
{
	if (state == 0)
	{
		if ((millis() - delayCount) >= ((uint64_t) speed[LIGHT_MOD_SUNSET]) * 60000UL)
		{
			state = 1;
			trace << "Starting to shut down. Completely off in 1 minute" << dendl;
		}
	}
	else if (state == 1)
	{
		const uint32_t step = 60000.0 / (((uint64_t) STRIP_LENGTH / 2.) * ((uint64_t) LIGHT_MAX_POWER));

		if (millis() - delayCount >= step)
		{
			strip.setPixelColor ((STRIP_LENGTH / 2 + counter), getRed (LIGHT_MOD_SUNSET) * (counter2 / (float)  LIGHT_MAX_POWER), getGreen (LIGHT_MOD_SUNSET) * (counter2 / (float) LIGHT_MAX_POWER), getBlue (LIGHT_MOD_SUNSET) * (counter2 / (float) LIGHT_MAX_POWER));
			strip.setPixelColor ((STRIP_LENGTH / 2 - counter), getRed (LIGHT_MOD_SUNSET) * (counter2 / (float)  LIGHT_MAX_POWER), getGreen (LIGHT_MOD_SUNSET) * (counter2 / (float) LIGHT_MAX_POWER), getBlue (LIGHT_MOD_SUNSET) * (counter2 / (float) LIGHT_MAX_POWER));

			if (counter >= (STRIP_LENGTH / 2.))
			{
				counter = 0;

				if (counter2 <= LIGHT_MIN_POWER)
				{
					mode = LIGHT_MOD_CONTINUOUS; // Leaving the mode
					switchOff();
				}
				else
				{
					counter2--;
				}
			}
			else
			{
				counter++;
			}

			delayCount = millis();
		}
	}
} // Light::sunset

// Start animation mode initialization
void Light::initStartAnimation ()
{
	state = 0;

	tempRed   = 255;
	tempGreen = 0;
	tempBlue  = 0;

	counter = 0;

	delayCount = -1;
	lastMode   = LIGHT_MOD_START_ANIM; // Setting lastMode so we don't call init again

	lightAll (0x000000);

	inf << "Begin start animation" << dendl;
}

// Start animation mode
void Light::startAnimation ()
{
	if (millis() - delayCount >= ((speed[LIGHT_MOD_START_ANIM] / (STRIP_LENGTH)) + 0.5))
	{
		switch (state)
		{
			case 0:
				if (tempGreen >= 255)
				{
					state     = 1;
					tempGreen = 255;
				}
				else
					tempGreen = constrain (tempGreen + step, 0, 255);
				break;

			case 1:
				if (tempRed <= 0)
				{
					state   = 2;
					tempRed = 0;
				}
				else
					tempRed = constrain (tempRed - step, 0, 255);
				break;

			case 2:
				if (tempBlue >= 255)
				{
					state    = 3;
					tempBlue = 255;
				}
				else
					tempBlue = constrain (tempBlue + step, 0, 255);
				break;

			case 3:
				if (tempGreen <= 0)
				{
					state     = 4;
					tempGreen = 0;
				}
				else
					tempGreen = constrain (tempGreen - step, 0, 255);
				break;

			case 4:
				if (tempRed >= 255)
				{
					state   = 5;
					tempRed = 255;
				}
				else
					tempRed = constrain (tempRed + step, 0, 255);
				break;

			case 5:
				if (tempBlue <= 0)
				{
					state    = 0;
					tempBlue = 0;
				}
				else
					tempBlue = constrain (tempBlue - step, 0, 255);
				break;
		}

		strip.setPixelColor (STRIP_LENGTH / 2 - counter, tempRed, tempGreen, tempBlue);
		strip.setPixelColor (STRIP_LENGTH / 2 + counter, tempRed, tempGreen, tempBlue);
		if (counter >= (STRIP_LENGTH / 4))
		{
			strip.setPixelColor (STRIP_LENGTH / 2 + (counter - (STRIP_LENGTH / 4)), 0x000000);
			strip.setPixelColor (STRIP_LENGTH / 2 - ((counter - STRIP_LENGTH / 4)), 0x000000);
		}

		delayCount = millis();

		counter++;
	}

	if (counter >= STRIP_LENGTH)
	{
		mode = LIGHT_MOD_CONTINUOUS;
		switchOff();
		trace << "End of start animation" << dendl;
	}
} // Light::startAnimation

// Music Mode initialization
void Light::initMusic ()
{
	lastMode = LIGHT_MOD_MUSIC; // Setting lastMode so we don't call init again

	delayCount = -1;

	counter = 0;

	inf << "Entering Music mode" << dendl;
}

// Music Mode
void Light::music ()
{
	int32_t level = utils.map (abs (510 - analogRead (PIN_MUSIC_IN)), 0, 25, 0, STRIP_LENGTH);

	if (level < 0)
		level = 0;

	if (level >= STRIP_LENGTH)
		return;

	if (millis() - delayCount >= 20)
	{
		if (counter > level)
			counter--;

		if (counter < 0)
			counter = 0;

		delayCount = millis();
	}

	if (millis() - delayCount2 >= 2)
	{
		if (counter < level)
			counter++;

		delayCount2 = millis();
	}


	for (uint8_t i = 0; i < STRIP_LENGTH; i++)
	{
		if (i < counter)
		{
			strip.setPixelColor (i, 0xFFFF00);
		}
		else
		{
			strip.setPixelColor (i, 0x000000);
		}
	}

	strip.setPixelColor (counter, 0x0000FF);
} // Light::music

#endif // if defined(LUMOS_ARDUINO_MEGA)

Light light = Light();