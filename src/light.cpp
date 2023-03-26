#include "light.h"
#include "ArduinoLogger.h"
#include "Memory.h"
#include "Sound.h"
#include "serial_com.h"
#include "Alarms.h"

static LightOnOff light_on;          /* If the leds are ON or OFF (True: ON / False: OFF) */
static LightPowerArray light_powers; /* Current lightning power for each mode */
static LightSpeedArray light_speeds; /* Current mode speed for each mode */
static LightRgbArray light_rgbs;     /* Current RGB value for each mode */
static LightMode light_mode;         /* Current lighting mode */

static time_t light_delayCount, light_delayCount2;
static uint32_t light_step;
static LightMode light_mode_last; /* Mode in previous loop - Allows mode initializations */

static uint8_t light_temp_red;
static uint8_t light_temp_green;
static uint8_t light_temp_blue;
static LightRgb light_rainbow[LIGHT_STRIP_HALF_LENGTH + 1];

static Adafruit_DotStar strip(LIGHT_STRIP_LENGTH, DOTSTAR_BGR);

void light_color_set(LightRgb rgb, LightMode affectedMode)
{
	light_rgbs[affectedMode] = rgb;
}

void light_power_set(Percentage power, LightMode affectedMode)
{
	light_powers[affectedMode] = power;
}

void light_speed_set(Percentage speed, LightMode affectedMode)
{
	light_speeds[affectedMode] = speed;
}

void light_mode_set(LightMode mode)
{
	light_mode = mode;
}

void light_switch_on()
{
	light_on = LightOnOff(true);
}

void light_switch_off()
{
	light_on = LightOnOff(false);
}

void light_power_add(Percentage power, LightMode affected_mode)
{
	light_powers[affected_mode] += power;
}

void light_power_sub(Percentage power, LightMode affected_mode)
{
	light_powers[affected_mode] -= power;
}

LightRgb light_color_get(LightMode affectedMode)
{
	return light_rgbs[affectedMode];
}

LightPower light_power_raw_get(LightMode affectedMode)
{
	return light_powers[affectedMode];
}

Percentage light_power_get(LightMode affectedMode)
{
	return light_powers[affectedMode].toPercent();
}

LightSpeed light_speed_raw_get(LightMode affectedMode)
{
	return light_speeds[affectedMode];
}

Percentage light_speed_get(LightMode affectedMode)
{
	return light_speeds[affectedMode].toPercent();
}

LightMode light_mode_get()
{
	return light_mode;
}

bool light_is_on()
{
	return light_on != 0;
}

bool light_is_off()
{
	return light_on == 0;
}

static void strip_color_all_set(LightColor red, LightColor green, LightColor blue)
{
	for (int i = 0; i < LIGHT_STRIP_LENGTH; i++)
		strip.setPixelColor(i, red.value(), green.value(), blue.value());
}

static void strip_color_all_set(LightRgb rgb)
{
	for (int i = 0; i < LIGHT_STRIP_LENGTH; i++)
		strip.setPixelColor(i, rgb.value());
}

static void strip_shut_all()
{
	strip.setBrightness(0);
}

static void strip_update(LightPower power)
{
	strip.setBrightness(power.value());

	digitalWrite(LIGHT_PIN_STRIP_CS, HIGH);
	strip.show();
	digitalWrite(LIGHT_PIN_STRIP_CS, LOW);
}

static void strip_anim_wait_start()
{
	int32_t counter1 = 0;
	uint8_t state    = 0;

	while (counter1 >= 0) {
		for (int8_t i = 0; i < LIGHT_STRIP_LENGTH; i++)
			if (abs(i - LIGHT_STRIP_HALF_LENGTH) < counter1)
				strip.setPixelColor(i, light_rainbow[LIGHT_STRIP_HALF_LENGTH - abs(i - LIGHT_STRIP_HALF_LENGTH)].value());
			else
				strip.setPixelColor(i, 0);

		strip_update(LightPower::DEF);

		delay(abs(strip.sine8(counter1 * (8 * 16) / LIGHT_STRIP_HALF_LENGTH + 64) - 128) / 5);

		if (counter1 > LIGHT_STRIP_HALF_LENGTH) state = 1;

		if (state)
			counter1--;
		else
			counter1++;
	}
}

static void strip_anim_done_start()
{
	int32_t counter = 0;

	while (counter < LIGHT_STRIP_HALF_LENGTH + LIGHT_TAIL_LENGTH + 2) {
		for (int8_t i = 0; i < LIGHT_STRIP_LENGTH; i++)
			if (counter > abs(i - LIGHT_STRIP_HALF_LENGTH) && abs(i - LIGHT_STRIP_HALF_LENGTH) >= counter - LIGHT_TAIL_LENGTH)
				strip.setPixelColor(i, light_rainbow[LIGHT_STRIP_HALF_LENGTH - abs(i - LIGHT_STRIP_HALF_LENGTH)].value());
			else
				strip.setPixelColor(i, 0);

		strip_update(LightPower::DEF);

		delay(15);

		counter++;
	}
}

static void light_mode_continuous()
{
	if (light_mode_last != light_mode) {
		light_mode_last = light_mode; /* Setting light_mode_last so we don't call init again */

		inf << "Entering Default mode" << dendl;
	}

	strip_color_all_set(light_rgbs[light_mode]);
}

/* Flash mode */
static void light_mode_flash()
{
	static uint8_t state      = 0;
	static time_t delay_count = 0;

	if (light_mode_last != light_mode) {
		state       = 0; /* Set initial state to 0 */
		delay_count = 0; /* Reseting milliseconds counter */

		light_mode_last = light_mode; /* Setting light_mode_last so we don't call init again */

		inf << "Entering Flash mode" << dendl;
	}

	if (millis() - delay_count >= 1000U - light_speeds[light_mode] * 10) {
		if (state >= 2)
			state = 0;
		else
			state++;  /* Incrementing state */

		delay_count = millis();
	}

	strip_color_all_set(state == 0 ? 0xFF0000 : state == 1 ? 0x00FF00 : 0x0000FF);
}

/* Strobe mode */
static void light_mode_strobe()
{
	static uint8_t state      = 0;
	static time_t delay_count = -1;

	if (light_mode_last != light_mode) {
		state       = 0;  /* Set initial state to 0 */
		delay_count = -1; /* Reseting milliseconds counter */

		light_mode_last = light_mode; /* Setting light_mode_last so we don't call init again */

		inf << "Entering Strobe mode" << dendl;
	}

	if (millis() - delay_count >= 1000U - light_speeds[light_mode] * 10) {
		state = !state; /* Inverting state */

		delay_count = millis();
	}

	if (state)
		strip_color_all_set(light_rgbs[light_mode]);
	else
		strip_color_all_set(0x000000);
}

/* Fade Mode */
static void light_mode_fade()
{
	static uint8_t state      = 0;
	static time_t delay_count = -1;
	static int32_t counter    = 0;

	if (light_mode_last != light_mode) {
		state       = 0;  /* Setting state to Decreasing state */
		delay_count = -1; /* Reseting milliseconds counter */
		counter     = 0;

		light_mode_last = light_mode; /* Setting light_mode_last so we don't call init again */

		inf << "Entering Fade mode" << dendl;
	}

	if (millis() - delay_count >= (1000U - light_speeds[light_mode] * 10) / 10) {
		if (counter >= 255)
			state = 0;
		else if (counter <= 0)
			state = 1;

		if (state)
			counter++;
		else
			counter--;

		/* verb << "counter : " << counter << endl; */

		delay_count = millis();
	}

	strip_color_all_set(light_color_get(light_mode) * (counter / 255.));
}

/* Smooth Mode */
static void light_mode_smooth()
{
	static uint8_t state      = 0;
	static time_t delay_count = -1;
	static uint8_t temp_red   = 0xFF;
	static uint8_t temp_green = 0;
	static uint8_t temp_blue  = 0;

	if (light_mode_last != light_mode) {
		state       = 0;  /* Init state to 0 */
		delay_count = -1; /* Reseting milliseconds counter */
		temp_red    = 0xFF;
		temp_green  = 0;
		temp_blue   = 0;

		light_mode_last = light_mode; /* Setting light_mode_last so we don't call init again */

		inf << "Entering Smooth mode" << dendl;
	}

	if (millis() - delay_count >= (1000U - light_speeds[light_mode] * 10) / 8) {
		switch (state) {
		case 0: if (++temp_green == 0xFF) state = 1; break;
		case 1: if (--temp_red == 0x00) state = 2; break;
		case 2: if (++temp_blue == 0xFF) state = 3; break;
		case 3: if (--temp_green == 0x00) state = 4; break;
		case 4: if (++temp_red == 0xFF) state = 5; break;
		case 5: if (--temp_blue == 0x00) state = 0; break;
		}

		delay_count = millis();
	}

	strip_color_all_set(temp_red, temp_green, temp_blue);
} /* Light::smooth */

/* Dawn Mode */
static void light_mode_dawn()
{
	static time_t delay_count = -1;
	static int32_t counter1   = 0;
	static int32_t counter2   = 1;
	static uint32_t step      = alarms.getDawnDuration() * (1000.0 / (LIGHT_STRIP_HALF_LENGTH * 255));

	if (light_mode_last != light_mode) {
		delay_count = -1; /* Reseting milliseconds counter */
		counter1    = 0;
		counter2    = 1;
		step        = alarms.getDawnDuration() * (1000.0 / (LIGHT_STRIP_HALF_LENGTH * 255));

		light_mode_last = light_mode; /* Setting light_mode_last so we don't call init again */

		strip_color_all_set(0);

		inf << "Entering Dawn mode for " << alarms.getDawnDuration() << dendl;
	}

	if (millis() - delay_count >= step) {
		strip.setPixelColor(LIGHT_STRIP_HALF_LENGTH + counter1, (light_color_get(light_mode) * (counter2 / 255.)).value());
		strip.setPixelColor(LIGHT_STRIP_HALF_LENGTH - counter1, (light_color_get(light_mode) * (counter2 / 255.)).value());

		if (counter1 >= LIGHT_STRIP_HALF_LENGTH) {
			counter1 = 0;

			if (counter2 >= 255) {
				light_rgbs [LightMode::continuous]  = light_rgbs [light_mode];  /* Transfer RGB final value to default mode */
				light_powers[LightMode::continuous] = light_powers[light_mode]; /* Same for power */
				light_mode                          = LightMode::continuous;    /* Leaving the mode */
			} else
				counter2++;
		} else
			counter1++;

		delay_count = millis();
	}
} /* Light::dawn */

/* Sunset Mode */
static void light_mode_sunset()
{
	static time_t delay_count = millis();
	static uint8_t state      = 0;
	static int32_t counter1   = 0;
	static int32_t counter2   = 255;
	static uint32_t step      = alarms.getDawnDuration() * (1000.0 / (LIGHT_STRIP_HALF_LENGTH * 255));

	if (light_mode_last != light_mode) {
		delay_count = millis(); /* Reseting milliseconds counter */
		state       = 0;
		counter1    = 0;
		counter2    = 255;
		step        = alarms.getSunsetDuration() * 1000; /* Step for state 0 */

		light_mode_last = light_mode; /* Setting light_mode_last so we don't call init again */

		strip_color_all_set(light_color_get(light_mode));

		inf << "Entering Sunset mode for " << alarms.getSunsetDuration() << dendl;
	}

	if (state == 0) { /* Waiting until sunsetTime has passed */
		if (millis() - delay_count >= step) {
			state = 1;
			trace << "Starting to shut down. Completely off in " << alarms.getSunsetDecreaseTime() << dendl;
			delay_count = millis();
			step        = alarms.getSunsetDecreaseTime() * (1000.0 / (LIGHT_STRIP_HALF_LENGTH * 255)); /* Changing the step for state 1 */
			counter1    = 0;
			counter2    = 255;
		}
	} else if (state == 1) { /* Progressively decreasing power */
		if (millis() - delay_count >= step) {
			strip.setPixelColor(0 + counter1,                  (light_color_get(light_mode) * (counter2 / 255.)).value());
			strip.setPixelColor(LIGHT_STRIP_LENGTH - counter1, (light_color_get(light_mode) * (counter2 / 255.)).value());

			if (counter1 >= LIGHT_STRIP_HALF_LENGTH) {
				counter1 = 0;

				if (counter2 <= 0) {
					light_mode = LightMode::continuous; /* Leaving the mode */
					light_switch_off();
				} else
					counter2--;
			} else
				counter1++;

			delay_count = millis();
		}
	}
} /* Light::sunset */

/* Music Mode */
static void light_mode_music()
{
	static time_t delay_count1 = -1;
	static time_t delay_count2 = -1;
	static int32_t counter     = 0;

	if (light_mode_last != light_mode) {
		delay_count1 = -1;
		delay_count2 = -1;
		counter      = 0;

		light_mode_last = light_mode; /* Setting light_mode_last so we don't call init again */

		inf << "Entering Music mode" << dendl;
	}

	int32_t level = utils.map(abs(510 - analogRead(LIGHT_PIN_MUSIC_IN)), 0, 25, 0, LIGHT_STRIP_LENGTH);

	if (level < 0)
		level = 0;

	if (level >= LIGHT_STRIP_LENGTH)
		return;

	if (millis() - delay_count1 >= 20) {
		if (counter > level)
			counter--;

		if (counter < 0)
			counter = 0;

		delay_count1 = millis();
	}

	if (millis() - delay_count2 >= 2) {
		if (counter < level)
			counter++;

		delay_count2 = millis();
	}


	for (uint8_t i = 0; i < LIGHT_STRIP_LENGTH; i++) {
		if (i < counter) {
			strip.setPixelColor(i, 0xFFFF00);
		} else {
			strip.setPixelColor(i, 0x000000);
		}
	}

	strip.setPixelColor(counter, 0x0000FF);
} /* Light::music */

static void light_mode_actions()
{
	/* Calling mode functions */
	switch (light_mode) {
	case LightMode::continuous:
		light_mode_continuous();
		break;

	case LightMode::flash:
		light_mode_flash();
		break;

	case LightMode::strobe:
		light_mode_strobe();
		break;

	case LightMode::fade:
		light_mode_fade();
		break;

	case LightMode::smooth:
		light_mode_smooth();
		break;

	case LightMode::dawn:
		light_mode_dawn();
		break;

	case LightMode::sunset:
		light_mode_sunset();
		break;

	case LightMode::music:
		light_mode_music();
		break;
	}
}

void light_init()
{
	pinMode(LIGHT_PIN_STRIP_CS, OUTPUT);
	digitalWrite(LIGHT_PIN_STRIP_CS, LOW);

	for (uint8_t i = 0; i < LIGHT_STRIP_HALF_LENGTH + 1; i++)
		light_rainbow[i].setHue(i * 360 / (LIGHT_STRIP_HALF_LENGTH + 1));

	strip.begin();

	if (memory.readLight()) { /* Returns True if EEPROM is not correctly initialized (This may be the first launch) */
		inf << "This is first launch, light variables will be initialized to their default values" << endl;

		light_reset();
	}

	strip_color_all_set(0);
	strip_update(0);

	light_mode      = LightMode::continuous;
	light_mode_last = LightMode::continuous; /* Initialiazing last mode as well */

	/* TODO: animation during Wi-Fi connection
	 *  while (!serial.checkTime())
	 *      startAnimWait();
	 */

	strip_anim_done_start();

	inf << "Light initialized." << dendl;
}

void light_reset()
{
	light_rgbs   = LightRgb::DEF;   /* Initializing rgbs their default value */
	light_powers = LightPower::DEF; /* Initializing powers their default value */
	light_speeds = LightSpeed::DEF; /* Initializing speeds their default value */

	memory.writeLight();
}

/* Perform mode action */
void light_action()
{
	/* If lighting is off, shut all lights */
	if (light_is_off()) {
		strip_color_all_set(0);
		strip_update(0);
		light_mode_last = LightMode::continuous;
		return;
	}

	light_mode_actions();

	strip_update(light_powers[light_mode]);
}
