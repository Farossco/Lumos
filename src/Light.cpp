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

void Light::setRed (uint8_t newRed, uint8_t affectedMod)
{
	if (affectedMod == CURRENT_MOD)
		affectedMod = mod;

	red[affectedMod] = constrain (newRed, LIGHT_MIN_COLOR, LIGHT_MAX_COLOR);
}

void Light::setGreen (uint8_t newGreen, uint8_t affectedMod)
{
	if (affectedMod == CURRENT_MOD)
		affectedMod = mod;

	green[affectedMod] = constrain (newGreen, LIGHT_MIN_COLOR, LIGHT_MAX_COLOR);
}

void Light::setBlue (uint8_t newBlue, uint8_t affectedMod)
{
	if (affectedMod == CURRENT_MOD)
		affectedMod = mod;

	blue[affectedMod] = constrain (newBlue, LIGHT_MIN_COLOR, LIGHT_MAX_COLOR);
}

void Light::setRgb (uint32_t newRgb, uint8_t affectedMod)
{
	if (affectedMod == CURRENT_MOD)
		affectedMod = mod;

	newRgb = constrain (newRgb, LIGHT_MIN_RGB, LIGHT_MAX_RGB);

	red[affectedMod]   = constrain ((uint8_t) (newRgb >> 16), LIGHT_MIN_COLOR, LIGHT_MAX_COLOR);
	green[affectedMod] = constrain ((uint8_t) (newRgb >> 8), LIGHT_MIN_COLOR, LIGHT_MAX_COLOR);
	blue[affectedMod]  = constrain ((uint8_t) newRgb, LIGHT_MIN_COLOR, LIGHT_MAX_COLOR);
}

void Light::setPower (uint8_t newPower, uint8_t affectedMod)
{
	if (affectedMod == CURRENT_MOD)
		affectedMod = mod;

	power[affectedMod] = constrain (newPower, LIGHT_MIN_POWER, LIGHT_MAX_POWER);
}

void Light::setSpeed (uint16_t newSpeed, uint8_t affectedMod)
{
	if (affectedMod == CURRENT_MOD)
		affectedMod = mod;

	speed[affectedMod] = constrain (newSpeed, LIGHT_MIN_SPEED[affectedMod], LIGHT_MAX_SPEED[affectedMod] == 0 ? 65535 : LIGHT_MAX_SPEED[affectedMod]);
}

void Light::setMod (uint8_t newMod)
{
	mod = constrain (newMod, LIGHT_MOD_MIN, LIGHT_MOD_MAX);
}

void Light::switchOn ()
{
	on = true;
}

void Light::switchOff ()
{
	on = false;
}

uint8_t Light::addPower (uint8_t powerAdd, uint8_t affectedMod)
{
	if (affectedMod == CURRENT_MOD)
		affectedMod = mod;

	return power[affectedMod] = constrain (power[affectedMod] + powerAdd, LIGHT_MIN_POWER, LIGHT_MAX_POWER);
}

uint8_t Light::subtractPower (uint8_t powerAdd, uint8_t affectedMod)
{
	if (affectedMod == CURRENT_MOD)
		affectedMod = mod;

	return power[affectedMod] = constrain (power[affectedMod] - powerAdd, LIGHT_MIN_POWER, LIGHT_MAX_POWER);
}

uint16_t Light::getDawnDuration ()
{
	return speed[LIGHT_MOD_DAWN];
}

uint8_t Light::getRed (uint8_t affectedMod)
{
	return red[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

uint8_t Light::getGreen (uint8_t affectedMod)
{
	return green[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

uint8_t Light::getBlue (uint8_t affectedMod)
{
	return blue[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

uint32_t Light::getRgb (uint8_t affectedMod)
{
	return (((uint32_t) red[affectedMod == CURRENT_MOD ? mod : affectedMod]) << 16) + (((uint32_t) green[affectedMod == CURRENT_MOD ? mod : affectedMod]) << 8) + blue[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

uint8_t Light::getPower (uint8_t affectedMod)
{
	return power[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

uint16_t Light::getSpeed (uint8_t affectedMod)
{
	return speed[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

uint8_t Light::getMod ()
{
	return mod;
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
		Log.info ("This is first launch, light variables will be initialized to their default values" dendl);

		reset();
	}

	light.lightAll (0x000000);
	strip.show();

	lastMod = LIGHT_MOD_CONTINUOUS; // Initialiazing last mod as well

	if (LIGHT_START_ANIMATION_ENABLE)
		mod = LIGHT_MOD_START_ANIM;
	else
		mod = LIGHT_MOD_CONTINUOUS;

	switchOn();
}

void Light::reset ()
{
	for (int i = LIGHT_MOD_MIN; i < LIGHT_N_MOD; i++)
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

// Perform mod action
void Light::action ()
{
	// If lightning is off, shut all lights
	if (isOff())
	{
		lightAll (0);
		strip.show();
		lastMod = -1;
		return;
	}

	if (mod != lastMod)
		Log.info ("Leaving %s mod" dendl, utils.lightModName (lastMod, CAPS_FIRST));

	// Calling mods functions
	switch (mod)
	{
		case LIGHT_MOD_CONTINUOUS:
			if (lastMod != LIGHT_MOD_CONTINUOUS) // If this is first call of the function, we call init function (lastMod will be set in init function)
				initContinuous();
			continuous();
			break;

		case LIGHT_MOD_FLASH:
			if (lastMod != LIGHT_MOD_FLASH)
				initFlash();
			flash();
			break;

		case LIGHT_MOD_STROBE:
			if (lastMod != LIGHT_MOD_STROBE)
				initStrobe();
			strobe();
			break;

		case LIGHT_MOD_FADE:
			if (lastMod != LIGHT_MOD_FADE)
				initFade();
			fade();
			break;

		case LIGHT_MOD_SMOOTH:
			if (lastMod != LIGHT_MOD_SMOOTH)
				initSmooth();
			smooth();
			break;

		case LIGHT_MOD_DAWN:
			if (lastMod != LIGHT_MOD_DAWN)
				initDawn();
			dawn();
			break;

		case LIGHT_MOD_SUNSET:
			if (lastMod != LIGHT_MOD_SUNSET)
				initSunset();
			sunset();
			break;

		case LIGHT_MOD_START_ANIM:
			if (lastMod != LIGHT_MOD_START_ANIM)
				initStartAnimation();
			startAnimation();
			break;

		case LIGHT_MOD_MUSIC:
			if (lastMod != LIGHT_MOD_MUSIC)
				initMusic();
			music();
			break;
	}

	strip.setBrightness (power[mod]);
	strip.show();
} // action

// Flash mod initialization
void Light::initContinuous ()
{
	lastMod = LIGHT_MOD_CONTINUOUS; // Setting lastMod so we don't call init again

	Log.info ("Entering Default mod" dendl);
}

void Light::continuous ()
{
	lightAll (red[LIGHT_MOD_CONTINUOUS], green[LIGHT_MOD_CONTINUOUS], blue[LIGHT_MOD_CONTINUOUS]);
}

// Flash mod initialization
void Light::initFlash ()
{
	state      = 0;               // Set initial state to 0
	delayCount = -1;              // Reseting milliseconds counter
	lastMod    = LIGHT_MOD_FLASH; // Setting lastMod so we don't call init again

	Log.info ("Entering Flash mod" dendl);
}

// Flash mod
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

// Strobe mod initialization
void Light::initStrobe ()
{
	state      = 0;                // Set initial state to 0
	delayCount = -1;               // Reseting milliseconds counter
	lastMod    = LIGHT_MOD_STROBE; // Setting lastMod so we don't call init again

	Log.info ("Entering Strobe mod" dendl);
}

// Strobe mod
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

// Fade Mod initialization
void Light::initFade ()
{
	state      = 0;  // Setting state to Decreasing state
	delayCount = -1; // Reseting milliseconds counter
	counter    = 0;
	lastMod    = LIGHT_MOD_FADE; // Setting lastMod so we don't call init again

	Log.info ("Entering Fade mod" dendl);
}

// Fade Mod
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

// Smooth Mod Initialization
void Light::initSmooth ()
{
	state      = 0;  // Init state to 0
	delayCount = -1; // Reseting milliseconds counter
	counter    = 0;
	lastMod    = LIGHT_MOD_SMOOTH; // Setting lastMod so we don't call init again

	Log.info ("Entering Smooth mod" dendl);
}

// Smooth Mod
void Light::smooth ()
{
	if (millis() - delayCount >= (uint32_t) (1000 / speed[LIGHT_MOD_SMOOTH]))
	{
		switch (state)
		{
			case 0:
				counter++;
				lightAll (counter, 0x00, 0x00);
				if (counter == 0xFF)
				{
					state   = 1;
					counter = 0;
				}

			case 1:
				counter++;
				lightAll (0xFF, counter, 0);
				if (counter == 0xFF)
					state = 2;
				break;

			case 2:
				counter--;
				lightAll (counter, 0xFF, 0);
				if (counter == 0x00)
					state = 3;
				break;

			case 3:
				counter++;
				lightAll (0x00, 0xFF, counter);
				if (counter == 0xFF)
					state = 4;
				break;

			case 4:
				counter--;
				lightAll (0x00, counter, 0xFF);
				if (counter == 0x00)
					state = 5;
				break;

			case 5:
				counter++;
				lightAll (counter, 0x00, 0xFF);
				if (counter == 0xFF)
					state = 6;
				break;

			case 6:
				counter--;
				lightAll (0xFF, 0x00, counter);
				if (counter == 0x00)
					state = 1;
				break;

			default:
				state = 0;
				break;
		}
		delayCount = millis();
	}
} // Light::smooth

// Dawn Mod initialization
void Light::initDawn ()
{
	delayCount = -1;             // Reseting milliseconds counter
	lastMod    = LIGHT_MOD_DAWN; // Setting lastMod so we don't call init again
	counter    = 0;
	counter2   = 1;

	lightAll (0x000000);

	Log.info ("Entering Dawn mod for %d min." dendl, speed[LIGHT_MOD_DAWN]);
}

// Dawn Mod
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
				red [LIGHT_MOD_CONTINUOUS]  = red [LIGHT_MOD_DAWN];  // Transfer RGB final value to default mod
				green[LIGHT_MOD_CONTINUOUS] = green[LIGHT_MOD_DAWN]; // Transfer RGB final value to default mod
				blue[LIGHT_MOD_CONTINUOUS]  = blue[LIGHT_MOD_DAWN];  // Transfer RGB final value to default mod
				power[LIGHT_MOD_CONTINUOUS] = power[LIGHT_MOD_DAWN]; // Same for power
				mod                         = LIGHT_MOD_CONTINUOUS;  // Leaving the mod
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

// Sunset Mod initialization
void Light::initSunset ()
{
	delayCount = millis();         // Reseting milliseconds counter
	lastMod    = LIGHT_MOD_SUNSET; // Setting lastMod so we don't call init again
	counter    = 0;
	counter2   = LIGHT_MAX_POWER;
	state      = 0;

	lightAll (getRgb (LIGHT_MOD_SUNSET));

	Log.info ("Entering Sunset mod for %d min." dendl, speed[LIGHT_MOD_SUNSET]);
}

// Sunset Mod
void Light::sunset ()
{
	if (state == 0)
	{
		if ((millis() - delayCount) >= ((uint64_t) speed[LIGHT_MOD_SUNSET]) * 60000UL)
		{
			state = 1;
			Log.trace ("Starting to shut down. Completely off in 1 minute" dendl);
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
					mod = LIGHT_MOD_CONTINUOUS; // Leaving the mod
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

// Start animation mod initialization
void Light::initStartAnimation ()
{
	state = 0;

	tempRed   = 255;
	tempGreen = 0;
	tempBlue  = 0;

	counter = 0;

	delayCount = -1;
	lastMod    = LIGHT_MOD_START_ANIM; // Setting lastMod so we don't call init again

	lightAll (0x000000);

	Log.info ("Begin start animation" dendl);
}

// Start animation mod
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
		mod = LIGHT_MOD_CONTINUOUS;
		switchOff();
		Log.trace ("End of start animation" dendl);
	}
} // Light::startAnimation

// Music Mod initialization
void Light::initMusic ()
{
	lastMod = LIGHT_MOD_MUSIC; // Setting lastMod so we don't call init again

	delayCount = -1;

	counter = 0;

	Log.info ("Entering Music mod" dendl);
}

// Music Mod
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
