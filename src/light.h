/**
 * @file light.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-03-31
 *
 * @brief This module handles the lights.
 */

#ifndef LUMOS_LIGHT_H
#define LUMOS_LIGHT_H

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

#include "time.h"
#include "color.h"

#define LIGHT_ON  (true)
#define LIGHT_OFF (false)

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
	LIGHT_MODE_MIN     = 0,
	LIGHT_MODE_MAX     = LIGHT_MODE_N - 1,
	LIGHT_MODE_CURRENT = 255 /* Used to target the current running mode */
};

/**
 * @brief light_mode data structure.
 *
 * This holds all needed data for one specific mode to operate
 *
 */
struct light_mode_data {
	uint8_t power; /* Current lighting power for the mode */
	uint8_t speed; /* Current speed for the mode */
	rgb_t   rgb;   /* Current RGB value for the mode */
};

/**
 * @brief Set RGB color for a specified mode
 *
 * @param rgb RGB value
 * @param mode Mode affected by the change
 */
void light_color_set(rgb_t rgb, uint8_t mode);


/**
 * @brief Set power value for a specified mode
 *
 * @param power Power value
 * @param mode Mode affected by the change
 */
void light_power_set(uint8_t power, uint8_t mode);

/**
 * @brief Set speed value for a specified mode
 *
 * @param speed Speed value
 * @param mode Mode affected by the change
 */
void light_speed_set(uint8_t speed, uint8_t mode);

/**
 * @brief Change the current light mode
 *
 * @param mode Mode value
 */
void light_mode_set(uint8_t mode);

/**
 * @brief Turn the lights ON or OFF
 *
 * @param state LIGHT_ON or LIGHT_OFF
 */
void light_state_set(bool state);

/**
 * @brief Get the current RGB value for a given mode
 *
 * @param mode Desired mode
 * @return The RGB value
 */
rgb_t light_color_get(uint8_t mode);

/**
 * @brief Get the current power value for a given mode in percentage
 *
 * @param mode Desired mode
 * @return The power value in percentage
 */
uint8_t light_power_get(uint8_t mode);

/**
 * @brief Get the current speed value for a given mode in percentage
 *
 * @param mode Desired mode
 * @return The speed value in percentage
 */
uint8_t light_speed_get(uint8_t mode);

/**
 * @brief Get the current light mode
 *
 * @return uint8_t - The current light mode
 */
uint8_t light_mode_get(void);

/**
 * @brief Get light ON/OFF state
 *
 * @return true if lights are on
 * @return false if lights are off
 */
bool light_state_get(void);

const char * light_mode_string_get(uint8_t mode);

/**
 * @brief Initialize light module
 *
 */
int light_init(void);

#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* ifndef LUMOS_LIGHT_H */
