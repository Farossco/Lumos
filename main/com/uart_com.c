#include "uart_com.h"
#include <string.h>
#include <urp.h>
#include "driver/uart.h"
#include "utils.h"
#include "light.h"
#include "kconfig_stub.h"

#define UART_COM_RX_BUF_SIZE 200
#define UART_COM_TX_BUF_SIZE 200

#define TAG                  "uart_com"

#define VALUE_TYPE_LON       "LON"
#define VALUE_TYPE_LMO       "LMO"
#define VALUE_TYPE_RGB       "RGB"
#define VALUE_TYPE_POW       "POW"
#define VALUE_TYPE_SPE       "SPE"
#define VALUE_TYPE_TZC       "TZC"

static void onLightOnOff(long value, void *arg)
{
	light_state_set(value == LIGHT_ON);
}

static void onLightMode(long value, void *arg)
{
	light_mode_set(value);
}

static void onLightModeRgb(long value, void *arg)
{
	if (value > 0xFFFFFF) {
		ESP_LOGE(TAG, "Incorrect RGB value: %lX", value);
		return;
	}

	light_color_set(rgb_from_code(value), LIGHT_MODE_CURRENT);
}

static void onLightModePower(long value, void *arg)
{
	light_power_set(value, LIGHT_MODE_CURRENT);
}

static void onLightModeSpeed(long value, void *arg)
{
	light_speed_set(value, LIGHT_MODE_CURRENT);
}

static void on_time_zone_change(const char *data, void *arg)
{
	char strftime_buf[64];
	time_t now;
	struct tm timeinfo;

	ESP_LOGI(TAG, "Setting timezone: \"%s\"", data);

	setenv("TZ", data, 1);
	tzset();

	time(&now);
	localtime_r(&now, &timeinfo);
	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	ESP_LOGI(TAG, "The current date/time in Paris is: %s", strftime_buf);
}

static struct urp_message_handler handlers[] = {
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_LON, onLightOnOff,     NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_LMO, onLightMode,      NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_RGB, onLightModeRgb,   NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_POW, onLightModePower, NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_SPE, onLightModeSpeed, NULL),
	URP_MESSAGE_STRING_HANDLER(VALUE_TYPE_TZC, on_time_zone_change, NULL),
};

static struct urp_config urp_config = {
	.handlers      = handlers,
	.handlers_size = ARRAY_SIZE(handlers),
	.uart_num      = UART_NUM_0,
	.uart_config   = {
		.baud_rate  = CONFIG_LUMOS_UART_BAUD_RATE,
		.data_bits  = UART_DATA_8_BITS,
		.parity     = UART_PARITY_DISABLE,
		.stop_bits  = UART_STOP_BITS_1,
		.flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_DEFAULT,
	},
	.tx_io_num = 1,
	.rx_io_num = 3
};

esp_err_t uart_com_init(void)
{
	ESP_LOGI(TAG, "Initializing uart");

	return urp_init(&urp_config);
}

void uart_com_register_callbacks(struct uart_com_callbacks cbks)
{}
