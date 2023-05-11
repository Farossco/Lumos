#include "light_strip.h"
#include <led_strip_spi.h>

#if CONFIG_LUMOS_LIGHT_DIMMED_MODE
#define DIV_FACTOR (5)
#else
#define DIV_FACTOR (1)
#endif

static led_strip_spi_t light_strip = {
	.buf             = NULL,
	.length          = LIGHT_STRIP_LENGTH,
	.host_device     = VSPI_HOST,
	.mosi_io_num     = 23,
	.sclk_io_num     = 18,
	.max_transfer_sz = 0,
	.clock_speed_hz  = 1000000,
	.queue_size      = 1,
	.device_handle   = NULL,
	.dma_chan        = LED_STRIP_SPI_DEFAULT_DMA_CHAN,
};

void light_strip_color_set(uint16_t index, rgb_t rgb, uint8_t power)
{
	rgb_t rgb_power = (rgb_t) {
		.r = ((uint16_t)rgb.r * power) / 255 / DIV_FACTOR,
		.g = ((uint16_t)rgb.g * power) / 255 / DIV_FACTOR,
		.b = ((uint16_t)rgb.b * power) / 255 / DIV_FACTOR
	};

	led_strip_spi_set_pixel(&light_strip, index, rgb_power);
}

void light_strip_color_all_set(rgb_t rgb, uint8_t power)
{
	rgb_t rgb_power = (rgb_t) {
		.r = ((uint16_t)rgb.r * power) / 255 / DIV_FACTOR,
		.g = ((uint16_t)rgb.g * power) / 255 / DIV_FACTOR,
		.b = ((uint16_t)rgb.b * power) / 255 / DIV_FACTOR
	};

	led_strip_spi_set_pixels(&light_strip, 0, LIGHT_STRIP_LENGTH, rgb_power);
}

void light_strip_update()
{
	led_strip_spi_flush(&light_strip);
}

void light_strip_init(void)
{
	ESP_ERROR_CHECK(led_strip_spi_install());

	led_strip_spi_init(&light_strip);
}
