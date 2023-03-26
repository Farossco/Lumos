#ifndef LIGHT_H
#define LIGHT_H

#include <Adafruit_DotStar.h>

#include <Arduino.h>
#include "Utils.h"
#include "time.h"
#include "Types.h"

const uint8_t LIGHT_STRIP_LENGTH      = 72;
const uint8_t LIGHT_STRIP_HALF_LENGTH = LIGHT_STRIP_LENGTH / 2. + 0.5;
const uint8_t LIGHT_PIN_DATA          = 51;
const uint8_t LIGHT_PIN_CLOCK         = 52;
const uint8_t LIGHT_PIN_MUSIC_IN      = 0;
const uint8_t LIGHT_PIN_STRIP_CS      = 21;

const uint8_t LIGHT_TAIL_LENGTH = LIGHT_STRIP_HALF_LENGTH;

const uint16_t LIGHT_SA_COLOR_STEP = 6 * 255 / (LIGHT_STRIP_LENGTH / 4);

/* TODO: Document */
void light_color_set(LightRgb rgb, LightMode affected_mode);
void light_power_set(Percentage power, LightMode affected_mode);
void light_speed_set(Percentage speed, LightMode affected_mode);
void light_mode_set(LightMode mode);
void light_power_add(Percentage power, LightMode affected_mode);
void light_power_sub(Percentage power, LightMode affected_mode);
void light_switch_on();
void light_switch_off();
LightRgb light_color_get(LightMode affected_mode);
LightPower light_power_raw_get(LightMode affected_mode);
Percentage light_power_get(LightMode affected_mode);
LightSpeed light_speed_raw_get(LightMode affected_mode);
Percentage light_speed_get(LightMode affected_mode);
LightMode light_mode_get();
bool light_is_on();
bool light_is_off();
void light_init();
void light_reset();
void light_action();

#endif // ifndef LIGHT_H
