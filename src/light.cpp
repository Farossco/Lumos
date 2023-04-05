#include "light.h"
#include <ArduinoLogger.h>
#include "light_mode.h"
#include "light_strip.h"
#include "Memory.h"
#include "Alarms.h"

static LightOnOff light_on;                                  /* If the leds are ON or OFF (True: ON / False: OFF) */
static LightMode light_mode;                                 /* Current lighting mode */
static struct light_mode_data light_mode_data[LightMode::N]; /* Light modes data */

static void light_reset(void)
{
	for (uint8_t i = 0; i < LightMode::N; i++) {
		light_mode_data[i].rgb   = LightRgb::DEF;   /* Initializing rgbs to their default value */
		light_mode_data[i].power = LightPower::DEF; /* Initializing powers to their default value */
		light_mode_data[i].speed = LightSpeed::DEF; /* Initializing speeds to their default value */
	}

	memory.writeLight();
}

static void on_light_mode_end(LightMode mode)
{
	switch (mode) {
	case LightMode::dawn:
		/* Transferring RGB value and power from Dawn to continuous */
		light_mode_data[LightMode::continuous].rgb   = light_mode_data[LightMode::dawn].rgb;
		light_mode_data[LightMode::continuous].power = light_mode_data[LightMode::dawn].power;
		light_mode_set(LightMode::continuous);
		break;

	case LightMode::sunset:
		break;

	case LightMode::start:
		light_switch_off();
		light_mode_set(LightMode::continuous);
		break;

	default:
		break;
	}
}

static struct light_mode_callbacks light_mode_callbacks = {
	.on_mode_end = on_light_mode_end
};

void light_init(void)
{
	light_strip_init();

	light_mode_init();

	light_mode_register_callbacks((struct light_mode_callbacks *)&light_mode_callbacks);

	/* if (memory.readLight()) { / * Returns True if EEPROM is not correctly initialized (This may be the first launch) * /
	 * 	inf << "This is first launch, light variables will be initialized to their default values" << endl;
	 */

	light_reset();

	light_mode = LightMode::start;
	light_on   = LightOnOff(true);

	inf << "Light initialized." << dendl;

	light_mode_start(light_mode, &light_mode_data[light_mode]);
}

void light_color_set(LightRgb rgb, LightMode mode)
{
	light_mode_data[mode].rgb = rgb;
}

void light_power_set(Percentage power, LightMode mode)
{
	light_mode_data[mode].power = power;
}

void light_speed_set(Percentage speed, LightMode mode)
{
	light_mode_data[mode].speed = speed;
}

void light_mode_set(LightMode mode)
{
	if (mode == light_mode) {
		return;
	}

	light_mode = mode;

	/* if the lights are on */
	if (light_state_get()) {
		/* Restart the light mode */
		light_mode_start(light_mode, &light_mode_data[light_mode]);
	}
}

void light_switch_on()
{
	if (light_state_get()) {
		return;
	}

	verb << "Turning lights on" << dendl;

	/* Start a new task for the current mode */
	light_mode_start(light_mode, &light_mode_data[light_mode]);

	/* Turning lights on */
	light_on = LightOnOff(true);
}

void light_switch_off()
{
	if (!light_state_get()) {
		return;
	}

	/* Terminate the current light mode task */
	light_mode_stop();

	/* Turning lights off */
	light_on = LightOnOff(false);
}

LightRgb light_color_get(LightMode mode)
{
	return light_mode_data[mode].rgb;
}

LightPower light_power_raw_get(LightMode mode)
{
	return light_mode_data[mode].power;
}

Percentage light_power_get(LightMode mode)
{
	return light_mode_data[mode].power.toPercent();
}

LightSpeed light_speed_raw_get(LightMode mode)
{
	return light_mode_data[mode].speed;
}

Percentage light_speed_get(LightMode mode)
{
	return light_mode_data[mode].speed.toPercent();
}

LightMode light_mode_get(void)
{
	return light_mode;
}

bool light_state_get(void)
{
	return light_on.value();
}
