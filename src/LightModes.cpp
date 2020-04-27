#if defined(LUMOS_ARDUINO_MEGA)

#include "Light.h"
#include "Logger.h"

void Light::modeActions ()
{
	// Calling modes functions
	switch (mode)
	{
		case LIGHT_MODE_CONTINUOUS:
			continuous();
			break;

		case LIGHT_MODE_FLASH:
			flash();
			break;

		case LIGHT_MODE_STROBE:
			strobe();
			break;

		case LIGHT_MODE_FADE:
			fade();
			break;

		case LIGHT_MODE_SMOOTH:
			smooth();
			break;

		case LIGHT_MODE_DAWN:
			dawn();
			break;

		case LIGHT_MODE_SUNSET:
			sunset();
			break;

		case LIGHT_MODE_START_ANIM:
			startAnimation();
			break;

		case LIGHT_MODE_MUSIC:
			music();
			break;
	}
} // Light::modeActions

void Light::continuous ()
{
	if (lastMode != LIGHT_MODE_CONTINUOUS)
	{
		lastMode = LIGHT_MODE_CONTINUOUS; // Setting lastMode so we don't call init again

		inf << "Entering Default mode" << dendl;
	}

	lightAll (red[LIGHT_MODE_CONTINUOUS], green[LIGHT_MODE_CONTINUOUS], blue[LIGHT_MODE_CONTINUOUS]);
}

// Flash mode
void Light::flash ()
{
	if (lastMode != LIGHT_MODE_FLASH)
	{
		state      = 0;                // Set initial state to 0
		delayCount = -1;               // Reseting milliseconds counter
		lastMode   = LIGHT_MODE_FLASH; // Setting lastMode so we don't call init again

		inf << "Entering Flash mode" << dendl;
	}

	if (millis() - delayCount >= (uint32_t) (1000 / speed[LIGHT_MODE_FLASH]))
	{
		if (state >= 2)
			state = 0;
		else
			state++;  // Incrementing state

		lightAll (state == 0 ? 0xFF0000 : state == 1 ? 0x00FF00 : 0x0000FF);
		delayCount = millis();
	}
}

// Strobe mode
void Light::strobe ()
{
	if (lastMode != LIGHT_MODE_STROBE)
	{
		state      = 0;                 // Set initial state to 0
		delayCount = -1;                // Reseting milliseconds counter
		lastMode   = LIGHT_MODE_STROBE; // Setting lastMode so we don't call init again

		inf << "Entering Strobe mode" << dendl;
	}

	if (millis() - delayCount >= (uint32_t) (1000 / speed[LIGHT_MODE_STROBE]))
	{
		state = !state; // Inverting state

		if (state)
			lightAll (red[LIGHT_MODE_STROBE], green[LIGHT_MODE_STROBE], blue[LIGHT_MODE_STROBE]);
		else
			lightAll (0x000000);

		delayCount = millis();
	}
}

// Fade Mode
void Light::fade ()
{
	if (lastMode != LIGHT_MODE_FADE)
	{
		state      = 0;  // Setting state to Decreasing state
		delayCount = -1; // Reseting milliseconds counter
		counter    = 0;
		lastMode   = LIGHT_MODE_FADE; // Setting lastMode so we don't call init again

		inf << "Entering Fade mode" << dendl;
	}

	if (millis() - delayCount >= (uint32_t) (1000 / speed[LIGHT_MODE_FADE]))
	{
		if (state)
			counter++;  // Increasing all colors
		else
			counter--;  // Decreasing all colors

		if (counter >= LIGHT_MAX_POWER)      // If color reach white, we start to decrease
			state = 0;                       // Decreasing state
		else if (counter <= LIGHT_MIN_POWER) // If color reach black, we start to increase
			state = 1;                       // Increasing state

		lightAll (red[LIGHT_MODE_FADE] * (counter / (float) LIGHT_MAX_POWER), green[LIGHT_MODE_FADE] * (counter / (float) LIGHT_MAX_POWER), blue[LIGHT_MODE_FADE] * (counter / (float) LIGHT_MAX_POWER));

		delayCount = millis();
	}
}

// Smooth Mode
void Light::smooth ()
{
	if (lastMode != LIGHT_MODE_SMOOTH)
	{
		state      = 0;  // Init state to 0
		delayCount = -1; // Reseting milliseconds counter
		counter    = 0;
		lastMode   = LIGHT_MODE_SMOOTH; // Setting lastMode so we don't call init again

		inf << "Entering Smooth mode" << dendl;
	}

	if (millis() - delayCount >= (uint32_t) (1000 / speed[LIGHT_MODE_SMOOTH]))
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

// Dawn Mode
void Light::dawn ()
{
	if (lastMode != LIGHT_MODE_DAWN)
	{
		delayCount = -1;              // Reseting milliseconds counter
		lastMode   = LIGHT_MODE_DAWN; // Setting lastMode so we don't call init again
		counter    = 0;
		counter2   = 1;

		lightAll (0x000000);

		inf << "Entering Dawn mode for " << speed[LIGHT_MODE_DAWN] << " min." << dendl;
	}

	const uint32_t step = (((uint64_t) speed[LIGHT_MODE_DAWN] ) * 60000.) / ( (((uint64_t) STRIP_LENGTH) / 2.) * ((uint64_t) LIGHT_MAX_POWER));

	if (millis() - delayCount >= step)
	{
		strip.setPixelColor ((STRIP_LENGTH / 2 + counter), getRed (LIGHT_MODE_DAWN) * (counter2 / (float)  LIGHT_MAX_POWER), getGreen (LIGHT_MODE_DAWN) * (counter2 / (float) LIGHT_MAX_POWER), getBlue (LIGHT_MODE_DAWN) * (counter2 / (float) LIGHT_MAX_POWER));
		strip.setPixelColor ((STRIP_LENGTH / 2 - counter), getRed (LIGHT_MODE_DAWN) * (counter2 / (float)  LIGHT_MAX_POWER), getGreen (LIGHT_MODE_DAWN) * (counter2 / (float) LIGHT_MAX_POWER), getBlue (LIGHT_MODE_DAWN) * (counter2 / (float) LIGHT_MAX_POWER));

		if (counter >= (STRIP_LENGTH / 2.))
		{
			counter = 0;

			if (counter2 >= LIGHT_MAX_POWER)
			{
				red [LIGHT_MODE_CONTINUOUS]  = red [LIGHT_MODE_DAWN];  // Transfer RGB final value to default mode
				green[LIGHT_MODE_CONTINUOUS] = green[LIGHT_MODE_DAWN]; // Transfer RGB final value to default mode
				blue[LIGHT_MODE_CONTINUOUS]  = blue[LIGHT_MODE_DAWN];  // Transfer RGB final value to default mode
				power[LIGHT_MODE_CONTINUOUS] = power[LIGHT_MODE_DAWN]; // Same for power
				mode                         = LIGHT_MODE_CONTINUOUS;  // Leaving the mode
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

// Sunset Mode
void Light::sunset ()
{
	if (lastMode != LIGHT_MODE_SUNSET)
	{
		delayCount = millis();          // Reseting milliseconds counter
		lastMode   = LIGHT_MODE_SUNSET; // Setting lastMode so we don't call init again
		counter    = 0;
		counter2   = LIGHT_MAX_POWER;
		state      = 0;

		lightAll (getRgb (LIGHT_MODE_SUNSET));

		inf << "Entering Sunset mode for " << speed[LIGHT_MODE_SUNSET] << " min." << dendl;
	}

	if (state == 0)
	{
		if ((millis() - delayCount) >= ((uint64_t) speed[LIGHT_MODE_SUNSET]) * 60000UL)
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
			strip.setPixelColor ((STRIP_LENGTH / 2 + counter), getRed (LIGHT_MODE_SUNSET) * (counter2 / (float)  LIGHT_MAX_POWER), getGreen (LIGHT_MODE_SUNSET) * (counter2 / (float) LIGHT_MAX_POWER), getBlue (LIGHT_MODE_SUNSET) * (counter2 / (float) LIGHT_MAX_POWER));
			strip.setPixelColor ((STRIP_LENGTH / 2 - counter), getRed (LIGHT_MODE_SUNSET) * (counter2 / (float)  LIGHT_MAX_POWER), getGreen (LIGHT_MODE_SUNSET) * (counter2 / (float) LIGHT_MAX_POWER), getBlue (LIGHT_MODE_SUNSET) * (counter2 / (float) LIGHT_MAX_POWER));

			if (counter >= (STRIP_LENGTH / 2.))
			{
				counter = 0;

				if (counter2 <= LIGHT_MIN_POWER)
				{
					mode = LIGHT_MODE_CONTINUOUS; // Leaving the mode
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

// Start animation mode
void Light::startAnimation ()
{
	if (lastMode != LIGHT_MODE_START_ANIM)
	{
		state = 0;

		tempRed   = 255;
		tempGreen = 0;
		tempBlue  = 0;

		counter = 0;

		delayCount = -1;
		lastMode   = LIGHT_MODE_START_ANIM; // Setting lastMode so we don't call init again

		lightAll (0x000000);

		inf << "Begin start animation" << dendl;
	}

	if (millis() - delayCount >= ((speed[LIGHT_MODE_START_ANIM] / (STRIP_LENGTH)) + 0.5))
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
		mode = LIGHT_MODE_CONTINUOUS;
		switchOff();
		trace << "End of start animation" << dendl;
	}
} // Light::startAnimation

// Music Mode
void Light::music ()
{
	if (lastMode != LIGHT_MODE_MUSIC)
	{
		lastMode   = LIGHT_MODE_MUSIC; // Setting lastMode so we don't call init again
		delayCount = -1;
		counter    = 0;

		inf << "Entering Music mode" << dendl;
	}

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

#endif // if defined LUMOS_ARDUINO