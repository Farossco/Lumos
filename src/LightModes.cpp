#include "Light.h"
#include "ArduinoLogger.h"
#include "Alarms.h"

void Light::modeActions()
{
	/* Calling mode functions */
	switch (mode) {
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

	case LightMode::music:
		music();
		break;
	}
} /* Light::modeActions */

void Light::continuous()
{
	if (lastMode != mode) {
		lastMode = mode; /* Setting lastMode so we don't call init again */

		inf << "Entering Default mode" << dendl;
	}

	lightAll(rgbs[mode]);
}

/* Flash mode */
void Light::flash()
{
	if (lastMode != mode) {
		state      = 0;    /* Set initial state to 0 */
		delayCount = 0;    /* Reseting milliseconds counter */
		lastMode   = mode; /* Setting lastMode so we don't call init again */

		inf << "Entering Flash mode" << dendl;
	}

	if (millis() - delayCount >= 1000U - speeds[mode] * 10) {
		if (state >= 2)
			state = 0;
		else
			state++;  /* Incrementing state */

		delayCount = millis();
	}

	lightAll(state == 0 ? 0xFF0000 : state == 1 ? 0x00FF00 : 0x0000FF);
}

/* Strobe mode */
void Light::strobe()
{
	if (lastMode != mode) {
		state      = 0;    /* Set initial state to 0 */
		delayCount = -1;   /* Reseting milliseconds counter */
		lastMode   = mode; /* Setting lastMode so we don't call init again */

		inf << "Entering Strobe mode" << dendl;
	}

	if (millis() - delayCount >= 1000U - speeds[mode] * 10) {
		state = !state; /* Inverting state */

		delayCount = millis();
	}

	if (state)
		lightAll(rgbs[mode]);
	else
		lightAll(0x000000);
}

/* Fade Mode */
void Light::fade()
{
	if (lastMode != mode) {
		state      = 0;  /* Setting state to Decreasing state */
		delayCount = -1; /* Reseting milliseconds counter */
		counter1   = 0;
		lastMode   = mode; /* Setting lastMode so we don't call init again */

		inf << "Entering Fade mode" << dendl;
	}

	if (millis() - delayCount >= (1000U - speeds[mode] * 10) / 10) {
		if (counter1 >= 255)
			state = 0;
		else if (counter1 <= 0)
			state = 1;

		if (state)
			counter1++;
		else
			counter1--;

		/* verb << "counter : " << counter << endl; */

		delayCount = millis();
	}

	lightAll(getRgb(mode) * (counter1 / 255.));
}

/* Smooth Mode */
void Light::smooth()
{
	if (lastMode != mode) {
		state      = 0;  /* Init state to 0 */
		delayCount = -1; /* Reseting milliseconds counter */
		tempRed    = 0xFF;
		tempGreen  = 0;
		tempBlue   = 0;
		lastMode   = mode; /* Setting lastMode so we don't call init again */

		inf << "Entering Smooth mode" << dendl;
	}

	if (millis() - delayCount >= (1000U - speeds[mode] * 10) / 8) {
		switch (state) {
		case 0: if (++tempGreen == 0xFF) state = 1; break;
		case 1: if (--tempRed == 0x00) state = 2; break;
		case 2: if (++tempBlue == 0xFF) state = 3; break;
		case 3: if (--tempGreen == 0x00) state = 4; break;
		case 4: if (++tempRed == 0xFF) state = 5; break;
		case 5: if (--tempBlue == 0x00) state = 0; break;
		}

		delayCount = millis();
	}

	lightAll(tempRed, tempGreen, tempBlue);
} /* Light::smooth */

/* Dawn Mode */
void Light::dawn()
{
	if (lastMode != mode) {
		delayCount = -1;   /* Reseting milliseconds counter */
		lastMode   = mode; /* Setting lastMode so we don't call init again */
		counter1   = 0;
		counter2   = 1;
		step       = alarms.getDawnDuration() * (1000.0 / (LIGHT_STRIP_HALF_LENGTH * 255));

		lightAll(0);

		inf << "Entering Dawn mode for " << alarms.getDawnDuration() << dendl;
	}

	if (millis() - delayCount >= step) {
		strip.setPixelColor(LIGHT_STRIP_HALF_LENGTH + counter1, (getRgb(mode) * (counter2 / 255.)).value());
		strip.setPixelColor(LIGHT_STRIP_HALF_LENGTH - counter1, (getRgb(mode) * (counter2 / 255.)).value());

		if (counter1 >= LIGHT_STRIP_HALF_LENGTH) {
			counter1 = 0;

			if (counter2 >= 255) {
				rgbs [LightMode::continuous]  = rgbs [mode];           /* Transfer RGB final value to default mode */
				powers[LightMode::continuous] = powers[mode];          /* Same for power */
				mode                          = LightMode::continuous; /* Leaving the mode */
			} else
				counter2++;
		} else
			counter1++;

		delayCount = millis();
	}
} /* Light::dawn */

/* Sunset Mode */
void Light::sunset()
{
	if (lastMode != mode) {
		delayCount = millis(); /* Reseting milliseconds counter */
		lastMode   = mode;     /* Setting lastMode so we don't call init again */
		state      = 0;
		step       = alarms.getSunsetDuration() * 1000; /* Step for state 0 */

		lightAll(getRgb(mode));

		inf << "Entering Sunset mode for " << alarms.getSunsetDuration() << dendl;
	}

	if (state == 0) { /* Waiting until sunsetTime has passed */
		if (millis() - delayCount >= step) {
			state = 1;
			trace << "Starting to shut down. Completely off in " << alarms.getSunsetDecreaseTime() << dendl;
			delayCount = millis();
			step       = alarms.getSunsetDecreaseTime() * (1000.0 / (LIGHT_STRIP_HALF_LENGTH * 255)); /* Changing the step for state 1 */
			counter1   = 0;
			counter2   = 255;
		}
	} else if (state == 1) { /* Progressively decreasing power */
		if (millis() - delayCount >= step) {
			strip.setPixelColor(0 + counter1,                  (getRgb(mode) * (counter2 / 255.)).value());
			strip.setPixelColor(LIGHT_STRIP_LENGTH - counter1, (getRgb(mode) * (counter2 / 255.)).value());

			if (counter1 >= LIGHT_STRIP_HALF_LENGTH) {
				counter1 = 0;

				if (counter2 <= 0) {
					mode = LightMode::continuous; /* Leaving the mode */
					switchOff();
				} else
					counter2--;
			} else
				counter1++;

			delayCount = millis();
		}
	}
} /* Light::sunset */

/* Music Mode */
void Light::music()
{
	if (lastMode != mode) {
		lastMode   = mode; /* Setting lastMode so we don't call init again */
		delayCount = -1;
		counter1   = 0;

		inf << "Entering Music mode" << dendl;
	}

	int32_t level = utils.map(abs(510 - analogRead(LIGHT_PIN_MUSIC_IN)), 0, 25, 0, LIGHT_STRIP_LENGTH);

	if (level < 0)
		level = 0;

	if (level >= LIGHT_STRIP_LENGTH)
		return;

	if (millis() - delayCount >= 20) {
		if (counter1 > level)
			counter1--;

		if (counter1 < 0)
			counter1 = 0;

		delayCount = millis();
	}

	if (millis() - delayCount2 >= 2) {
		if (counter1 < level)
			counter1++;

		delayCount2 = millis();
	}


	for (uint8_t i = 0; i < LIGHT_STRIP_LENGTH; i++) {
		if (i < counter1) {
			strip.setPixelColor(i, 0xFFFF00);
		} else {
			strip.setPixelColor(i, 0x000000);
		}
	}

	strip.setPixelColor(counter1, 0x0000FF);
} /* Light::music */
