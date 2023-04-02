#include "light_strip.h"
#include <Adafruit_DotStar.h>

#define LIGHT_STRIP_PIN_CS 21

static Adafruit_DotStar light_strip(LIGHT_STRIP_LENGTH, DOTSTAR_BGR);

void light_strip_init(void)
{
	pinMode(LIGHT_STRIP_PIN_CS, OUTPUT);
	digitalWrite(LIGHT_STRIP_PIN_CS, LOW);

	light_strip.begin();
}

void light_strip_color_set(uint16_t index, LightRgb rgb)
{
	light_strip.setPixelColor(index, rgb.value());
}

void light_strip_color_all_set(LightRgb rgb)
{
	for (int i = 0; i < LIGHT_STRIP_LENGTH; i++)
		light_strip.setPixelColor(i, rgb.value());
}

void light_strip_update()
{
	digitalWrite(LIGHT_STRIP_PIN_CS, HIGH);
	light_strip.show();
	digitalWrite(LIGHT_STRIP_PIN_CS, LOW);
}

void light_strip_update(LightPower power)
{
	light_strip.setBrightness(power.value());

	light_strip_update();
}
