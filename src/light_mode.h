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

/**
 * @brief light_mode data structure.
 *
 * This holds all needed data for one specific mode to operate
 *
 */
struct light_mode_data {
	LightPower power; /* Current lighting power for the mode */
	LightSpeed speed; /* Current speed for the mode */
	LightRgb   rgb;   /* Current RGB value for the mode */
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
	void (*on_mode_end)(LightMode mode);
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
void light_mode_start(LightMode mode, struct light_mode_data *data);

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

#endif /* ifndef LUMOS_LIGHT_MODE_H */
