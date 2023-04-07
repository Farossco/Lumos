/**
 * @file light.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-03-31
 *
 * @brief This module handles the lights.
 */

#ifndef LUMOS_LIGHT_H
#define LUMOS_LIGHT_H

#include <Arduino.h>
#include "Utils.h"
#include "time.h"
#include "Types.h"

#define LIGHT_ON  (true)
#define LIGHT_OFF (false)

/**
 * @brief Initialize light module
 *
 */
void light_init(void);

/**
 * @brief Set RGB color for a specified mode
 *
 * @param rgb RGB value
 * @param mode Mode affected by the change
 */
void light_color_set(LightRgb rgb, uint8_t mode);

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
 * @brief Turn the lights on
 *
 */
void light_switch_on(void);

/**
 * @brief Turn the lights off
 *
 */
void light_switch_off(void);

/**
 * @brief Get the current RGB value for a given mode
 *
 * @param mode Desired mode
 * @return The RGB value
 */
LightRgb light_color_get(uint8_t mode);

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

#endif /* ifndef LUMOS_LIGHT_H */
