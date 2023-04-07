/**
 * @file light_mode.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-03-31
 *
 * @brief Module handling all light modes executions
 */

#ifndef LUMOS_LIGHT_MODE_H
#define LUMOS_LIGHT_MODE_H

#include <Arduino.h>
#include "Types.h"

enum {
	LIGHT_MODE_CONTINUOUS, /* Continuous lightning mode */
	LIGHT_MODE_FLASH,      /* Flash mode */
	LIGHT_MODE_STROBE,     /* Strobe mode */
	LIGHT_MODE_FADE,       /* Fade mode */
	LIGHT_MODE_SMOOTH,     /* Smooth mode */
	LIGHT_MODE_DAWN,       /* Dawn mode */
	LIGHT_MODE_SUNSET,     /* Sunset mode */
	LIGHT_MODE_START,      /* Start mode */

	LIGHT_MODE_N,
	LIGHT_MODE_MIN = 0,
	LIGHT_MODE_MAX = LIGHT_MODE_N - 1
};

/**
 * @brief light_mode data structure.
 *
 * This holds all needed data for one specific mode to operate
 *
 */
struct light_mode_data {
	uint8_t  power; /* Current lighting power for the mode */
	uint8_t  speed; /* Current speed for the mode */
	LightRgb rgb;   /* Current RGB value for the mode */
};

/**
 * @brief Callback functions for this module
 *
 */
struct light_mode_callbacks {
	/**
	 * @brief Callback function.
	 *
	 * Called every time a light mode ends.
	 *
	 */
	void (*on_mode_end)(uint8_t mode);
};

/**
 * @brief Initialize light_mode module
 *
 */
void light_mode_init(void);

/**
 * @brief Start the task for the given light mode
 *
 * @param mode Light mode
 * @param data The data of the mode
 */
void light_mode_start(uint8_t mode, struct light_mode_data *data);

/**
 * @brief Stop the current running mode
 *
 */
void light_mode_stop(void);

/**
 * @brief Register light_mode callbacks
 *
 * @param cbks Callback structure pointer
 */
void light_mode_register_callbacks(struct light_mode_callbacks *cbks);

const char * light_mode_string_get(uint8_t mode);

#endif /* ifndef LUMOS_LIGHT_MODE_H */
