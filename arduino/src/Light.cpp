#include "Light.h"
#include "Logger.h"
#include "Memory.h"
#include "Sound.h"

Light::Light() : strip (STRIP_LENGTH, PIN_DATA, PIN_CLOCK, DOTSTAR_BGR)
{ }

void Light::init ()
{
	strip.begin();

	if (memory.read()) // Returns True if EEPROM is not correctly initialized (This may be the first launch)
	{
		Log.info ("This is first launch, variables will be initialized to their default values" dendl);

		reset();
	}

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

		memory.write();
}

void Light::lightAll (unsigned char red, unsigned char green, unsigned char blue)
{
	for (int i = 0; i < STRIP_LENGTH; i++)
		strip.setPixelColor (i, red, green, blue);
}

void Light::lightAll (unsigned long rgb)
{
	for (int i = 0; i < STRIP_LENGTH; i++)
		strip.setPixelColor (i, rgb);
}

void Light::setRed (unsigned char newRed, unsigned char affectedMod)
{
	red[affectedMod == CURRENT_MOD ? mod : affectedMod] = newRed;
}

void Light::setGreen (unsigned char newGreen, unsigned char affectedMod)
{
	green[affectedMod == CURRENT_MOD ? mod : affectedMod] = newGreen;
}

void Light::setBlue (unsigned char newBlue, unsigned char affectedMod)
{
	blue[affectedMod == CURRENT_MOD ? mod : affectedMod] = newBlue;
}

void Light::setRgb (unsigned long newRgb, unsigned char affectedMod)
{
	red[affectedMod == CURRENT_MOD ? mod : affectedMod]   = (unsigned char) (newRgb >> 16);
	green[affectedMod == CURRENT_MOD ? mod : affectedMod] = (unsigned char) (newRgb >> 8);
	blue[affectedMod == CURRENT_MOD ? mod : affectedMod]  = (unsigned char) newRgb;
}

void Light::setPower (unsigned char newPower, unsigned char affectedMod)
{
	power[affectedMod == CURRENT_MOD ? mod : affectedMod] = newPower;
}

void Light::setSpeed (int newSpeed, unsigned char affectedMod)
{
	speed[affectedMod == CURRENT_MOD ? mod : affectedMod] = newSpeed;
}

void Light::setMod (unsigned char newMod)
{
	mod = newMod;
}

void Light::switchOn ()
{
	on = true;
}

void Light::switchOff ()
{
	on = false;
}

int Light::getDawnDuration ()
{
	return speed[LIGHT_MOD_DAWN];
}

unsigned char Light::getRed (unsigned char affectedMod)
{
	return red[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

unsigned char Light::getGreen (unsigned char affectedMod)
{
	return green[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

unsigned char Light::getBlue (unsigned char affectedMod)
{
	return blue[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

unsigned long Light::getRgb (unsigned char affectedMod)
{
	return (((unsigned long) red[affectedMod == CURRENT_MOD ? mod : affectedMod]) << 16) + (((unsigned long) green[affectedMod == CURRENT_MOD ? mod : affectedMod]) << 8) + blue[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

unsigned char Light::getPower (unsigned char affectedMod)
{
	return power[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

unsigned int Light::getSpeed (unsigned char affectedMod)
{
	return speed[affectedMod == CURRENT_MOD ? mod : affectedMod];
}

unsigned char Light::getMod ()
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

	switch (lastMod)
	{
		case LIGHT_MOD_CONTINUOUS:
			if (mod != LIGHT_MOD_CONTINUOUS)
			{
				Log.info ("Leaving Default mod" dendl);
			}
			break;

		case LIGHT_MOD_FLASH:
			if (mod != LIGHT_MOD_FLASH)
			{
				Log.info ("Leaving Flash mod" dendl);
			}
			break;

		case LIGHT_MOD_STROBE:
			if (mod != LIGHT_MOD_STROBE)
			{
				Log.info ("Leaving Strobe mod" dendl);
			}
			break;

		case LIGHT_MOD_FADE:
			if (mod != LIGHT_MOD_FADE)
			{
				Log.info ("Leaving Fade mod" dendl);
			}
			break;

		case LIGHT_MOD_SMOOTH:
			if (mod != LIGHT_MOD_SMOOTH)
			{
				Log.info ("Leaving Smooth mod" dendl);
			}
			break;

		case LIGHT_MOD_DAWN:
			if (mod != LIGHT_MOD_DAWN)
			{
				Log.info ("Leaving Dawn mod" dendl);
			}
			break;

		case LIGHT_MOD_START_ANIM:
			if (mod != LIGHT_MOD_START_ANIM)
			{
				Log.info ("End of start animation" dendl);
			}
			break;

		case LIGHT_MOD_MUSIC:
			if (mod != LIGHT_MOD_MUSIC)
			{
				Log.info ("Leaving Music mod" dendl);
			}
			break;
	}

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
} // action

// Flash mod initialization
void Light::initContinuous ()
{
	lastMod = LIGHT_MOD_CONTINUOUS; // Setting lastMod so we don't call init again

	Log.info ("Entering Default mod" dendl);
}

void Light::continuous ()
{
	strip.setBrightness (power[LIGHT_MOD_CONTINUOUS]);
	lightAll (red[LIGHT_MOD_CONTINUOUS], green[LIGHT_MOD_CONTINUOUS], blue[LIGHT_MOD_CONTINUOUS]);
	strip.show();
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
	if (millis() - delayCount >= (unsigned int) (1000 / speed[LIGHT_MOD_FLASH]))
	{
		if (state >= 2)
			state = 0;
		else
			state++;  // Incrementing state

		lightAll (state == 0 ? 0xFF0000 : state == 1 ? 0x00FF00 : 0x0000FF);
		delayCount = millis();
	}

	strip.setBrightness (power[LIGHT_MOD_FLASH]);
	strip.show();
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
	if (millis() - delayCount >= (unsigned int) (1000 / speed[LIGHT_MOD_STROBE]))
	{
		state = !state; // Inverting state

		if (state)
			lightAll (red[LIGHT_MOD_STROBE], green[LIGHT_MOD_STROBE], blue[LIGHT_MOD_STROBE]);
		else
			lightAll (0x000000);

		delayCount = millis();
	}

	strip.setBrightness (power[LIGHT_MOD_STROBE]);
	strip.show();
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
	if (millis() - delayCount >= (unsigned int) (1000 / speed[LIGHT_MOD_FADE]))
	{
		if (state)
			counter++;  // Increasing all colors
		else
			counter--;  // Decreasing all colors

		if (counter >= LIGHT_MAX_POWER)      // If color reach white, we start to decrease
			state = 0;                       // Decreasing state
		else if (counter <= LIGHT_MIN_POWER) // If color reach black, we start to increase
			state = 1;                       // Increasing state

		lightAll (red[LIGHT_MOD_FADE], green[LIGHT_MOD_FADE], blue[LIGHT_MOD_FADE]);

		delayCount = millis();
	}

	strip.setBrightness (counter * (power[LIGHT_MOD_FADE]) / LIGHT_MAX_POWER);
	strip.show();
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
	if (millis() - delayCount >= (unsigned int) (1000 / speed[LIGHT_MOD_SMOOTH]))
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

	strip.setBrightness (power[LIGHT_MOD_SMOOTH]);
	strip.show();
} // Light::smooth

// Dawn Mod initialization
void Light::initDawn ()
{
	delayCount = -1;             // Reseting milliseconds counter
	lastMod    = LIGHT_MOD_DAWN; // Setting lastMod so we don't call init again

	Log.info ("Entering Dawn mod for %d min." dendl, speed[LIGHT_MOD_DAWN]);
}

// Dawn Mod
void Light::dawn ()
{
	// x60 to convert in seconds and x1000 to convert in milliseconds
	if (millis() - delayCount >= (((unsigned long) speed[LIGHT_MOD_DAWN]) * 60 * 1000) / LIGHT_MAX_POWER)
	{
		counter++;

		lightAll (red[LIGHT_MOD_DAWN], green[LIGHT_MOD_DAWN], blue[LIGHT_MOD_DAWN]);

		if (counter >= LIGHT_MAX_POWER) // When max power is reached
		{
			red [LIGHT_MOD_CONTINUOUS]  = red [LIGHT_MOD_DAWN];  // Transfer RGB final value to default mod
			green[LIGHT_MOD_CONTINUOUS] = green[LIGHT_MOD_DAWN]; // Transfer RGB final value to default mod
			blue[LIGHT_MOD_CONTINUOUS]  = blue[LIGHT_MOD_DAWN];  // Transfer RGB final value to default mod
			power[LIGHT_MOD_CONTINUOUS] = power[LIGHT_MOD_DAWN]; // Same for power
			mod                         = LIGHT_MOD_CONTINUOUS;  // Leaving the mod

			Log.info ("Leaving Dawn mod" dendl);
		}

		delayCount = millis();
	}

	strip.setBrightness (counter * (power[LIGHT_MOD_DAWN] / LIGHT_MAX_POWER));
	strip.show();
}

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
	if (millis() - delayCount >= speed[LIGHT_MOD_START_ANIM] / (STRIP_LENGTH) +0.5)
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
					tempGreen = (tempGreen + step) > 255 ? 255 : tempGreen + step;
				break;

			case 1:
				if (tempRed <= 0)
				{
					state   = 2;
					tempRed = 0;
				}
				else
					tempRed = (tempRed - step) < 0 ? 0 : tempRed - step;
				break;

			case 2:
				if (tempBlue >= 255)
				{
					state    = 3;
					tempBlue = 255;
				}
				else
					tempBlue = (tempBlue + step) > 255 ? 255 : tempBlue + step;
				break;

			case 3:
				if (tempGreen <= 0)
				{
					state     = 4;
					tempGreen = 0;
				}
				else
					tempGreen = (tempGreen - step) < 0 ? 0 : tempGreen - step;
				break;

			case 4:
				if (tempRed >= 255)
				{
					state   = 5;
					tempRed = 255;
				}
				else
					tempRed = (tempRed + step) > 255 ? 255 : tempRed + step;
				break;

			case 5:
				if (tempBlue <= 0)
				{
					state    = 0;
					tempBlue = 0;
				}
				else
					tempBlue = (tempBlue - step) < 0 ? 0 : tempBlue - step;
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

	strip.setBrightness (power[LIGHT_MOD_START_ANIM]);
	strip.show();

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

	counter = 100;

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

	strip.setBrightness (power[LIGHT_MOD_MUSIC]);
	strip.show();
} // Light::music

Light light = Light();
