#include "uart_com.h"
#include <string.h>
#include <urp.h>
#include "driver/uart.h"
#include "utils_c.h"
#include "light.h"
#include "temp_log_util.h"

#define UART_COM_RX_BUF_SIZE 200
#define UART_COM_TX_BUF_SIZE 200

#define TAG                  "uart_com"

#define VALUE_TYPE_SCO       "SCO"
#define VALUE_TYPE_TIM       "TIM"
#define VALUE_TYPE_LON       "LON"
#define VALUE_TYPE_RGB       "RGB"
#define VALUE_TYPE_POW       "POW"
#define VALUE_TYPE_LMO       "LMO"
#define VALUE_TYPE_SPE       "SPE"
#define VALUE_TYPE_SON       "SON"
#define VALUE_TYPE_SMO       "SMO"
#define VALUE_TYPE_VOL       "VOL"
#define VALUE_TYPE_DVO       "DVO"
#define VALUE_TYPE_DTM       "DTM"
#define VALUE_TYPE_DDU       "DDU"
#define VALUE_TYPE_SDU       "SDU"
#define VALUE_TYPE_SDT       "SDT"
#define VALUE_TYPE_TZC       "TZC"

static void onLightOnOff(long value, void *arg) {}

static void onLightMode(long value, void *arg) {}

static void onLightModeRgb(long value, void *arg) {}

static void onLightModePower(long value, void *arg) {}

static void onLightModeSpeed(long value, void *arg) {}

static void onSoundCommand(long value, void *arg) {}

static void onSoundMode(long value, void *arg) {}

static void onSoundVolume(long value, void *arg) {}

static void onSoundOnOff(long value, void *arg) {}

static void onAlarmDawnVolume(long value, void *arg) {}

static void onAlarmDawnTime(long value, void *arg) {}

static void onAlarmDawnDuration(long value, void *arg) {}

static void onAlarmSunsetDuration(long value, void *arg) {}

static void onAlarmSunsetDecreaseTime(long value, void *arg) {}

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
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_LON, onLightOnOff,              NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_LMO, onLightMode,               NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_RGB, onLightModeRgb,            NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_POW, onLightModePower,          NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_SPE, onLightModeSpeed,          NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_SON, onSoundOnOff,              NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_SMO, onSoundMode,               NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_VOL, onSoundVolume,             NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_SCO, onSoundCommand,            NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_DVO, onAlarmDawnVolume,         NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_DTM, onAlarmDawnTime,           NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_DDU, onAlarmDawnDuration,       NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_SDU, onAlarmSunsetDuration,     NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_SDT, onAlarmSunsetDecreaseTime, NULL),
	URP_MESSAGE_STRING_HANDLER(VALUE_TYPE_TZC, on_time_zone_change, NULL),
};

static struct urp_config urp_config = {
	.handlers      = handlers,
	.handlers_size = ARRAY_SIZE(handlers),
	.uart_num      = UART_NUM_0,
	.uart_config   = {
		.baud_rate = ESP_DEBUG_BAUD_RATE,
		.data_bits = UART_DATA_8_BITS,
		.parity    = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	},
	.tx_io_num = 1,
	.rx_io_num = 3,
};

void uart_com_init(void)
{
	urp_init(&urp_config);
}

void uart_com_register_callbacks(struct uart_com_callbacks cbks)
{}
