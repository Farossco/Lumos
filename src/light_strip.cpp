#include "light_strip.h"
#include <Adafruit_DotStar.h>

#define LIGHT_STRIP_PIN_CS 21
#define LIGHT_TAIL_LENGTH  LIGHT_STRIP_HALF_LENGTH

static Adafruit_DotStar light_strip(LIGHT_STRIP_LENGTH, DOTSTAR_BGR);
static LightRgb light_strip_rainbow[LIGHT_STRIP_HALF_LENGTH + 1];

void light_strip_init(void)
{
	pinMode(LIGHT_STRIP_PIN_CS, OUTPUT);
	digitalWrite(LIGHT_STRIP_PIN_CS, LOW);

	for (uint8_t i = 0; i < LIGHT_STRIP_HALF_LENGTH + 1; i++)
		light_strip_rainbow[i].setHue(i * 360 / (LIGHT_STRIP_HALF_LENGTH + 1));

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

static void light_strip_anim_wait_start()
{
	int32_t counter1 = 0;
	uint8_t state    = 0;

	while (counter1 >= 0) {
		for (int8_t i = 0; i < LIGHT_STRIP_LENGTH; i++)
			if (abs(i - LIGHT_STRIP_HALF_LENGTH) < counter1)
				light_strip.setPixelColor(i, light_strip_rainbow[LIGHT_STRIP_HALF_LENGTH - abs(i - LIGHT_STRIP_HALF_LENGTH)].value());
			else
				light_strip.setPixelColor(i, 0);

		light_strip_update(LightPower::DEF);

		delay(abs(light_strip.sine8(counter1 * (8 * 16) / LIGHT_STRIP_HALF_LENGTH + 64) - 128) / 5);

		if (counter1 > LIGHT_STRIP_HALF_LENGTH) state = 1;

		if (state)
			counter1--;
		else
			counter1++;
	}
}

static void light_strip_anim_done_start()
{
	int32_t counter = 0;

	while (counter < LIGHT_STRIP_HALF_LENGTH + LIGHT_TAIL_LENGTH + 2) {
		for (int8_t i = 0; i < LIGHT_STRIP_LENGTH; i++)
			if (counter > abs(i - LIGHT_STRIP_HALF_LENGTH) && abs(i - LIGHT_STRIP_HALF_LENGTH) >= counter - LIGHT_TAIL_LENGTH)
				light_strip.setPixelColor(i, light_strip_rainbow[LIGHT_STRIP_HALF_LENGTH - abs(i - LIGHT_STRIP_HALF_LENGTH)].value());
			else
				light_strip.setPixelColor(i, 0);

		light_strip_update(LightPower::DEF);

		delay(15);

		counter++;
	}
}
