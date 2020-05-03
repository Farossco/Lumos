#if defined(LUMOS_ARDUINO_MEGA)

#include "Light.h"
#include "ArduinoLogger.h"

void Light::modeActions ()
{
	// Calling mode functions
	switch (mode)
	{
		case LightMode::continuous:
			continuous();
			break;

		case LightMode::flash:
			flash();
			break;

		case LightMode::strobe:
			strobe();
			break;

		case LightMode::fade:
			fade();
			break;

		case LightMode::smooth:
			smooth();
			break;

		case LightMode::dawn:
			dawn();
			break;

		case LightMode::sunset:
			sunset();
			break;

		case LightMode::startAnimation:
			startAnimation();
			break;

		case LightMode::music:
			music();
			break;
	}
} // Light::modeActions

void Light::continuous ()
{
	if (lastMode != mode)
	{
		lastMode = mode; // Setting lastMode so we don't call init again

		inf << "Entering Default mode" << dendl;
	}

	lightAll (rgbs[mode]);
}

// Flash mode
void Light::flash ()
{
	if (lastMode != mode)
	{
		state      = 0;    // Set initial state to 0
		delayCount = 0;    // Reseting milliseconds counter
		lastMode   = mode; // Setting lastMode so we don't call init again

		inf << "Entering Flash mode" << dendl;
	}

	if (millis() - delayCount >= 1000U - speeds[mode] * 10)
	{
		if (state >= 2)
			state = 0;
		else
			state++;  // Incrementing state

		delayCount = millis();
	}

	lightAll (state == 0 ? 0xFF0000 : state == 1 ? 0x00FF00 : 0x0000FF);
}

// Strobe mode
void Light::strobe ()
{
	if (lastMode != mode)
	{
		state      = 0;    // Set initial state to 0
		delayCount = -1;   // Reseting milliseconds counter
		lastMode   = mode; // Setting lastMode so we don't call init again

		inf << "Entering Strobe mode" << dendl;
	}

	if (millis() - delayCount >= 1000U - speeds[mode] * 10)
	{
		state = !state; // Inverting state

		delayCount = millis();
	}

	if (state)
		lightAll (rgbs[mode]);
	else
		lightAll (0x000000);
}

// Fade Mode
void Light::fade ()
{
	if (lastMode != mode)
	{
		state      = 0;  // Setting state to Decreasing state
		delayCount = -1; // Reseting milliseconds counter
		counter    = 0;
		lastMode   = mode; // Setting lastMode so we don't call init again

		inf << "Entering Fade mode" << dendl;
	}

	if (millis() - delayCount >= (1000U - speeds[mode] * 10) / 10)
	{
		if (counter >= LightPower::MAX)      // If color reach white, we start to decrease
			state = 0;                       // Decreasing state
		else if (counter <= LightPower::MIN) // If color reach black, we start to increase
			state = 1;                       // Increasing state

		if (state)
			counter++;  // Increasing all colors
		else
			counter--;  // Decreasing all colors

		delayCount = millis();
	}

	lightAll (getRed (mode) * (counter / (float) LightPower::MAX), getGreen (mode) * (counter / (float) LightPower::MAX), getBlue (mode) * (counter / (float)  LightPower::MAX));
}

// Smooth Mode
void Light::smooth ()
{
	if (lastMode != mode)
	{
		state      = 0;  // Init state to 0
		delayCount = -1; // Reseting milliseconds counter
		tempRed    = 0xFF;
		tempGreen  = 0;
		tempBlue   = 0;
		lastMode   = mode; // Setting lastMode so we don't call init again

		inf << "Entering Smooth mode" << dendl;
	}

	if (millis() - delayCount >= (1000U - speeds[mode] * 10) / 8)
	{
		switch (state)
		{
			case 0: if (++tempGreen == 0xFF) state = 1; break;
			case 1: if (--tempRed == 0x00) state = 2; break;
			case 2: if (++tempBlue == 0xFF) state = 3; break;
			case 3: if (--tempGreen == 0x00) state = 4; break;
			case 4: if (++tempRed == 0xFF) state = 5; break;
			case 5: if (--tempBlue == 0x00) state = 0; break;
		}

		delayCount = millis();
	}

	lightAll (tempRed, tempGreen, tempBlue);
} // Light::smooth

// Dawn Mode
void Light::dawn ()
{
	if (lastMode != mode)
	{
		delayCount = -1;   // Reseting milliseconds counter
		lastMode   = mode; // Setting lastMode so we don't call init again
		counter    = 0;
		counter2   = 1;

		lightAll (0x000000);

		inf << "Entering Dawn mode for " << speeds[mode] << " min." << dendl;
	}

	const uint32_t step = speeds[mode] / (STRIP_LENGTH / 2. * LightPower::MAX / 60000.);

	if (millis() - delayCount >= step)
	{
		strip.setPixelColor ((STRIP_LENGTH / 2 + counter), getRed (mode) * (counter2 / (float) LightPower::MAX), getGreen (mode) * (counter2 / (float) LightPower::MAX), getBlue (mode) * (counter2 / (float) LightPower::MAX));
		strip.setPixelColor ((STRIP_LENGTH / 2 - counter), getRed (mode) * (counter2 / (float) LightPower::MAX), getGreen (mode) * (counter2 / (float) LightPower::MAX), getBlue (mode) * (counter2 / (float) LightPower::MAX));

		if (counter >= (STRIP_LENGTH / 2.))
		{
			counter = 0;

			if (counter2 >= LightPower::MAX)
			{
				rgbs [LightMode::continuous]  = rgbs [mode];           // Transfer RGB final value to default mode
				powers[LightMode::continuous] = powers[mode];          // Same for power
				mode                          = LightMode::continuous; // Leaving the mode
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
	if (lastMode != mode)
	{
		delayCount = millis(); // Reseting milliseconds counter
		lastMode   = mode;     // Setting lastMode so we don't call init again
		counter    = 0;
		counter2   = LightPower::MAX;
		state      = 0;

		lightAll (getRgb (mode));

		inf << "Entering Sunset mode for " << speeds[mode] << " min." << dendl;
	}

	if (state == 0)
	{
		if ((millis() - delayCount) >= (uint64_t) (speeds[mode] * 60000UL))
		{
			state = 1;
			trace << "Starting to shut down. Completely off in 1 minute" << dendl;
		}
	}
	else if (state == 1)
	{
		const uint32_t step = 60000.0 / (STRIP_LENGTH / 2. * LightPower::MAX);

		if (millis() - delayCount >= step)
		{
			strip.setPixelColor ((STRIP_LENGTH / 2 + counter), getRed (mode) * (counter2 / (float) LightPower::MAX), getGreen (mode) * (counter2 / (float) LightPower::MAX), getBlue (mode) * (counter2 / (float) LightPower::MAX));
			strip.setPixelColor ((STRIP_LENGTH / 2 - counter), getRed (mode) * (counter2 / (float) LightPower::MAX), getGreen (mode) * (counter2 / (float) LightPower::MAX), getBlue (mode) * (counter2 / (float) LightPower::MAX));

			if (counter >= (STRIP_LENGTH / 2.))
			{
				counter = 0;

				if (counter2 <= LightPower::MIN)
				{
					mode = LightMode::continuous; // Leaving the mode
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
	if (lastMode != mode)
	{
		state = 0;

		tempRed   = 255;
		tempGreen = 0;
		tempBlue  = 0;

		counter = 0;

		delayCount = -1;
		lastMode   = mode; // Setting lastMode so we don't call init again

		lightAll (0x000000);

		inf << "Begin start animation" << dendl;
	}

	if (millis() - delayCount >= ((speeds[mode] / (STRIP_LENGTH)) + 0.5))
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
					tempGreen = constrain (tempGreen + LIGHT_SA_COLOR_STEP, 0, 255);
				break;

			case 1:
				if (tempRed <= 0)
				{
					state   = 2;
					tempRed = 0;
				}
				else
					tempRed = constrain (tempRed - LIGHT_SA_COLOR_STEP, 0, 255);
				break;

			case 2:
				if (tempBlue >= 255)
				{
					state    = 3;
					tempBlue = 255;
				}
				else
					tempBlue = constrain (tempBlue + LIGHT_SA_COLOR_STEP, 0, 255);
				break;

			case 3:
				if (tempGreen <= 0)
				{
					state     = 4;
					tempGreen = 0;
				}
				else
					tempGreen = constrain (tempGreen - LIGHT_SA_COLOR_STEP, 0, 255);
				break;

			case 4:
				if (tempRed >= 255)
				{
					state   = 5;
					tempRed = 255;
				}
				else
					tempRed = constrain (tempRed + LIGHT_SA_COLOR_STEP, 0, 255);
				break;

			case 5:
				if (tempBlue <= 0)
				{
					state    = 0;
					tempBlue = 0;
				}
				else
					tempBlue = constrain (tempBlue - LIGHT_SA_COLOR_STEP, 0, 255);
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
		mode = LightMode::continuous;
		switchOff();
		trace << "End of start animation" << dendl;
	}
} // Light::startAnimation

// Music Mode
void Light::music ()
{
	if (lastMode != mode)
	{
		lastMode   = mode; // Setting lastMode so we don't call init again
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