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

static void light_mode_continuous(void *pvParameters)
{
	inf << "Entering Continuous mode" << dendl;

	while (true) {
		strip_color_all_set(light_rgbs[light_mode]);

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

/* Flash mode */
static void light_mode_flash(void *pvParameters)
{
	inf << "Entering Flash mode" << dendl;

	while (true) {
		strip_color_all_set(0xFF0000);

		delay(1000U - light_speeds[light_mode] * 10);

		strip_color_all_set(0x00FF00);

		delay(1000U - light_speeds[light_mode] * 10);

		strip_color_all_set(0x0000FF);

		delay(1000U - light_speeds[light_mode] * 10);
	}
}

/* Strobe mode */
static void light_mode_strobe(void *pvParameters)
{
	inf << "Entering Strobe mode" << dendl;

	while (true) {
		strip_color_all_set(light_rgbs[light_mode]);

		vTaskDelay(pdMS_TO_TICKS(1000U - light_speeds[light_mode] * 10));

		strip_color_all_set(0);

		vTaskDelay(pdMS_TO_TICKS(1000U - light_speeds[light_mode] * 10));
	}
}

/* Fade Mode */
static void light_mode_fade(void *pvParameters)
{
	uint8_t state   = 0;
	int32_t counter = 0;

	inf << "Entering Fade mode" << dendl;

	while (true) {
		strip_color_all_set(light_color_get(light_mode) * (counter / 255.));

		if (counter >= 255)
			state = 0;
		else if (counter <= 0)
			state = 1;

		if (state)
			counter++;
		else
			counter--;

		vTaskDelay(pdMS_TO_TICKS((1000U - light_speeds[light_mode] * 10) / 50));
	}
}

/* Smooth Mode */
static void light_mode_smooth(void *pvParameters)
{
	uint8_t state = 0;
	uint8_t red   = 0xFF;
	uint8_t green = 0;
	uint8_t blue  = 0;

	inf << "Entering Smooth mode" << dendl;

	while (true) {
		strip_color_all_set(red, green, blue);

		switch (state) {
		case 0: if (++green == 0xFF) state = 1; break;
		case 1: if (--red == 0x00) state = 2; break;
		case 2: if (++blue == 0xFF) state = 3; break;
		case 3: if (--green == 0x00) state = 4; break;
		case 4: if (++red == 0xFF) state = 5; break;
		case 5: if (--blue == 0x00) state = 0; break;
		}

		vTaskDelay(pdMS_TO_TICKS((1000U - light_speeds[light_mode] * 10) / 8));
	}
}

/* Dawn Mode */
static void light_mode_dawn(void *pvParameters)
{
	int16_t counter1 = 0;
	int16_t counter2 = 1;
	uint32_t step    = alarms.getDawnDuration() * (1000.0 / (LIGHT_STRIP_HALF_LENGTH * 255));

	inf << "Entering Dawn mode for " << alarms.getDawnDuration() << dendl;

	strip_color_all_set(0);

	while (counter2 < 255) {
		strip.setPixelColor(LIGHT_STRIP_HALF_LENGTH + counter1, (light_color_get(light_mode) * (counter2 / 255.)).value());
		strip.setPixelColor(LIGHT_STRIP_HALF_LENGTH - counter1, (light_color_get(light_mode) * (counter2 / 255.)).value());

		if (counter1++ >= LIGHT_STRIP_HALF_LENGTH) {
			counter1 = 0;

			counter2++;
		}

		vTaskDelay(pdMS_TO_TICKS(step));
	}

	light_rgbs [LightMode::continuous]  = light_rgbs [light_mode];  /* Transfer RGB final value to default mode */
	light_powers[LightMode::continuous] = light_powers[light_mode]; /* Same for power */

	light_mode_set(LightMode::continuous); /* Leaving the mode */
}

/* Sunset Mode */
static void light_mode_sunset(void *pvParameters)
{
	int32_t counter1 = 0;
	int32_t counter2 = 255;

	inf << "Entering Sunset mode for " << alarms.getSunsetDuration() << dendl;

	strip_color_all_set(light_color_get(light_mode));

	delay(alarms.getSunsetDuration().value());

	trace << "Starting to shut down. Completely off in " << alarms.getSunsetDecreaseTime() << dendl;

	while (counter2 > 0) {
		strip.setPixelColor(counter1, (light_color_get(light_mode) * (counter2 / 255.)).value());
		strip.setPixelColor(LIGHT_STRIP_LENGTH - counter1, (light_color_get(light_mode) * (counter2 / 255.)).value());

		if (counter1++ >= LIGHT_STRIP_HALF_LENGTH) {
			counter1 = 0;

			counter2--;
		}

		vTaskDelay(pdMS_TO_TICKS(alarms.getSunsetDecreaseTime() * (1000.0 / (LIGHT_STRIP_HALF_LENGTH * 255))));
	}

	light_on = LightOnOff(false); /* Shutting down the lights */

	light_mode_set(LightMode::continuous); /* Leaving the mode */
}

/* Music Mode */
static void light_mode_music(void *pvParameters)
{
	int32_t counter = 0;
	int32_t level   = 0;

	inf << "Entering Music mode" << dendl;

	while (true) {
		level = utils.map(abs(510 - analogRead(LIGHT_PIN_MUSIC_IN)), 0, 25, 0, LIGHT_STRIP_LENGTH);

		level = constrain(level, 0, LIGHT_STRIP_LENGTH - 1);

		/* TODO: Create task inside the task
		 * if (millis() - delay_count1 >= 20) {
		 *  if (counter > level)
		 *      counter--;
		 *
		 *  if (counter < 0)
		 *      counter = 0;
		 *
		 *  delay_count1 = millis();
		 * }
		 *
		 * if (millis() - delay_count2 >= 2) {
		 *  if (counter < level)
		 *      counter++;
		 *
		 *  /*delay_count2 = millis();
		 * }
		 */

		for (uint8_t i = 0; i < LIGHT_STRIP_LENGTH; i++) {
			if (i == counter) {
				strip.setPixelColor(i, 0x0000FF);
			} else if (i < counter) {
				strip.setPixelColor(i, 0xFFFF00);
			} else {
				strip.setPixelColor(i, 0x000000);
			}
		}

		vTaskDelay(pdMS_TO_TICKS(1));
	}
}

static TaskFunction_t light_mode_funcs[] = { light_mode_continuous,
	                                         light_mode_flash,
	                                         light_mode_strobe,
	                                         light_mode_fade,
	                                         light_mode_smooth,
	                                         light_mode_dawn,
	                                         light_mode_sunset,
	                                         light_mode_music };
static TaskHandle_t xHandle;

static void light_mode_task_create(void)
{
	xTaskCreatePinnedToCore(light_mode_funcs[light_mode],
	                        "LightModeTask",
	                        10240,
	                        NULL,
	                        1,
	                        &xHandle,
	                        1);

	verb << "Task started, handle: " << hex << (uint32_t)xHandle << endl;
}

static void light_mode_task_delete(void)
{
	vTaskDelete(xHandle);
	xHandle = NULL;
	verb << "Light mode task deleted" << endl;
}

void light_init()
{
	pinMode(LIGHT_PIN_STRIP_CS, OUTPUT);
	digitalWrite(LIGHT_PIN_STRIP_CS, LOW);

	for (uint8_t i = 0; i < LIGHT_STRIP_HALF_LENGTH + 1; i++)
		light_rainbow[i].setHue(i * 360 / (LIGHT_STRIP_HALF_LENGTH + 1));

	strip.begin();

	/* if (memory.readLight()) { / * Returns True if EEPROM is not correctly initialized (This may be the first launch) * /
	 * 	inf << "This is first launch, light variables will be initialized to their default values" << endl;
	 */

	/* 	light_reset();
	 * }
	 */

	light_reset();

	strip_color_all_set(0);
	strip_update(0);

	light_mode = LightMode::continuous;
	light_on   = LightOnOff(false);

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
		return;
	}

	strip_update(light_powers[light_mode]);
}

void light_mode_set(LightMode mode)
{
	if (mode == light_mode) {
		return;
	}

	light_mode = mode;

	if (light_is_off()) {
		return;
	}

	/* Terminate the current light mode task*/
	if (xHandle) {
		light_mode_task_delete();
	} else {
		err << "There should be a task running!" << dendl;
	}

	light_mode_task_create();
}

void light_switch_on()
{
	if (light_is_on()) {
		return;
	}

	verb << "Turning lights on" << dendl;

	/* Terminate the current light mode task if running */
	if (xHandle) {
		err << "The light mode task shouldn't be running right now!" << dendl;
		light_mode_task_delete();
	}

	light_mode_task_create();

	light_on = LightOnOff(true);
}

void light_switch_off()
{
	if (light_is_off()) {
		return;
	}

	/* Terminate the current light mode task if running */
	if (xHandle) {
		light_mode_task_delete();
	} else {
		err << "There should be a light mode task running right now!" << dendl;
	}

	light_on = LightOnOff(false);
	strip_color_all_set(0);
	strip_update(0);
}
