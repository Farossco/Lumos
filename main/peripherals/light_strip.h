/**
 * @file light_strip.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-05-11
 * 
 * @brief 
 */

#pragma once

#include "color.h"

/**
 * @brief The length of the LED strip
 *
 */
#define LIGHT_STRIP_LENGTH 72

/**
 * @brief Half of the length of the LED strip
 *
 */
#define LIGHT_STRIP_HALF_LENGTH ((uint8_t)(LIGHT_STRIP_LENGTH / 2. + 0.5))

/**
 * @brief Initialize light_strip module
 *
 */
void light_strip_init(void);

/**
 * @brief Sets a given color to a specified LED of the strip
 *
 * @param index The index of the LED, starting from 0
 * @param rgb The color to be set
 */
void light_strip_color_set(uint16_t index, rgb_t rgb, uint8_t power);

/**
 * @brief Sets a unique given color for all LEDs of the strip
 *
 * @param rgb The color to be set
 */
void light_strip_color_all_set(rgb_t rgb, uint8_t power);

/**
 * @brief Updates the LED strip.
 *
 * Note: Any modification done to the color of the strip is
 * local only until the function is called
 *
 */
void light_strip_update(void);
