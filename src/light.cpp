#include "light.h"
#include <ArduinoLogger.h>
#include "light_mode.h"
#include "light_strip.h"
#include "Memory.h"
#include "Alarms.h"

#define LIGHT_RGB_DEFAULT   0xFFFFFF
#define LIGHT_POWER_DEFAULT 255
#define LIGHT_SPEED_DEFAULT 63

static bool light_state;                                     /* If the leds are ON or OFF (True: ON / False: OFF) */
static uint8_t light_mode;                                   /* Current lighting mode */
static struct light_mode_data light_mode_data[LIGHT_MODE_N]; /* Light modes data */

static void light_reset(void)
{
	for (uint8_t i = 0; i < LIGHT_MODE_N; i++) {
		light_mode_data[i].rgb   = LIGHT_RGB_DEFAULT;   /* Initializing rgbs to their default value */
		light_mode_data[i].power = LIGHT_POWER_DEFAULT; /* Initializing powers to their default value */
		light_mode_data[i].speed = LIGHT_SPEED_DEFAULT; /* Initializing speeds to their default value */
	}
}

static void on_light_mode_end(uint8_t mode)
{
	switch (mode) {
	case LIGHT_MODE_DAWN:
		/* Transferring RGB value and power from Dawn to continuous */
		light_mode_data[LIGHT_MODE_CONTINUOUS].rgb   = light_mode_data[LIGHT_MODE_DAWN].rgb;
		light_mode_data[LIGHT_MODE_CONTINUOUS].power = light_mode_data[LIGHT_MODE_DAWN].power;
		light_mode_set(LIGHT_MODE_CONTINUOUS);
		break;

	case LIGHT_MODE_SUNSET:
		break;

	case LIGHT_MODE_START:
		light_switch_off();
		light_mode_set(LIGHT_MODE_CONTINUOUS);
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

	light_mode  = LIGHT_MODE_START;
	light_state = LIGHT_ON;

	inf << "Light initialized." << dendl;

	light_mode_start(light_mode, &light_mode_data[light_mode]);
}

void light_color_set(LightRgb rgb, uint8_t mode)
{
	light_mode_data[mode].rgb = rgb;
}

void light_power_set(uint8_t power, uint8_t mode)
{
	light_mode_data[mode].power = power;
}

void light_speed_set(uint8_t speed, uint8_t mode)
{
	light_mode_data[mode].speed = speed;
}

void light_mode_set(uint8_t mode)
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
	light_state = LIGHT_ON;
}

void light_switch_off()
{
	if (!light_state_get()) {
		return;
	}

	/* Terminate the current light mode task */
	light_mode_stop();

	/* Turning lights off */
	light_state = LIGHT_OFF;
}

LightRgb light_color_get(uint8_t mode)
{
	return light_mode_data[mode].rgb;
}

uint8_t light_power_get(uint8_t mode)
{
	return light_mode_data[mode].power;
}

uint8_t light_speed_get(uint8_t mode)
{
	return light_mode_data[mode].speed;
}

uint8_t light_mode_get(void)
{
	return light_mode;
}

bool light_state_get(void)
{
	return light_state;
}
