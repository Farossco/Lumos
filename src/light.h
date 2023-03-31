#ifndef LUMOS_LIGHT_H
#define LUMOS_LIGHT_H

#include <Arduino.h>
#include "Utils.h"
#include "time.h"
#include "Types.h"

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
void light_color_set(LightRgb rgb, LightMode mode);

/**
 * @brief Set power value for a specified mode
 *
 * @param power Power value
 * @param mode Mode affected by the change
 */
void light_power_set(Percentage power, LightMode mode);

/**
 * @brief Set speed value for a specified mode
 *
 * @param speed Speed value
 * @param mode Mode affected by the change
 */
void light_speed_set(Percentage speed, LightMode mode);

/**
 * @brief Change the current light mode
 *
 * @param mode Mode value
 */
void light_mode_set(LightMode mode);

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
LightRgb light_color_get(LightMode mode);

/**
 * @brief Get the current raw power value for a given mode
 *
 * @param mode Desired mode
 * @return The raw power value
 */
LightPower light_power_raw_get(LightMode mode);

/**
 * @brief Get the current power value for a given mode in percentage
 *
 * @param mode Desired mode
 * @return The power value in percentage
 */
Percentage light_power_get(LightMode mode);

/**
 * @brief Get the current raw speed value for a given mode
 *
 * @param mode Desired mode
 * @return The raw speed value
 */
LightSpeed light_speed_raw_get(LightMode mode);

/**
 * @brief Get the current speed value for a given mode in percentage
 *
 * @param mode Desired mode
 * @return The speed value in percentage
 */
Percentage light_speed_get(LightMode mode);

/**
 * @brief Get the current light mode
 *
 * @return LightMode
 */
LightMode light_mode_get(void);

/**
 * @brief Get light ON/OFF state
 *
 * @return true if lights are on
 * @return false if lights are off
 */
bool light_state_get(void);

#endif /* ifndef LUMOS_LIGHT_H */
