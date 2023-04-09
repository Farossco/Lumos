#include "light_strip.h"
#include <Adafruit_DotStar.h>

static Adafruit_DotStar light_strip(LIGHT_STRIP_LENGTH, DOTSTAR_BGR);

void light_strip_init(void)
{
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
	light_strip.show();
}

void light_strip_update(uint8_t power)
{
	light_strip.setBrightness(power);

	light_strip_update();
}
