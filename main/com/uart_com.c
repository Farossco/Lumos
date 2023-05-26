#include "uart_com.h"
#include <string.h>
#include <urp.h>
#include <settings.h>
#include "alarms.h"
#include "driver/uart.h"
#include "utils.h"
#include "light.h"

#define UART_COM_RX_BUF_SIZE 200
#define UART_COM_TX_BUF_SIZE 200

#define TAG                  "uart_com"

#define VALUE_TYPE_LON       "LON"
#define VALUE_TYPE_LMO       "LMO"
#define VALUE_TYPE_RGB       "RGB"
#define VALUE_TYPE_POW       "POW"
#define VALUE_TYPE_SPE       "SPE"
#define VALUE_TYPE_TZC       "TZC"
#define VALUE_TYPE_AAD       "AAD"
#define VALUE_TYPE_ARM       "ARM"
#define VALUE_TYPE_ALS       "ALS"
#define VALUE_TYPE_ACL       "ACL"

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

static void on_time_zone_change(const char *timezone, void *arg)
{
	esp_err_t err;
	time_t now;
	struct tm timeinfo;

	if (strlen(timezone) >= sizeof(stgs_timezone_t)) {
		ESP_LOGE(TAG, "Incorrect timezone length for %s (max is %d)", timezone, sizeof(stgs_timezone_t));
		return;
	}

	ESP_LOGI(TAG, "Setting timezone: \"%s\"", timezone);
	ESP_LOGI(TAG, "Saving timezone to settings");

	err = settings_set(SETTINGS_TIMEZONE, timezone, sizeof(stgs_timezone_t));
	if (err) {
		ESP_LOGE(TAG, "Failed to save timezone to settings: %s", esp_err_to_name(err));
	}

	setenv("TZ", timezone, 1);
	tzset();

	time(&now);
	localtime_r(&now, &timeinfo);
	ESP_LOGI(TAG, "Timezone set");
}

static void on_alarm_add(const char *data, void *arg)
{
	esp_err_t err;
	struct alarm_schedule schedule = { .active_on.u8 = 0xFF };
	char *endptr;

	if (strlen(data) != 8) {
		ESP_LOGE(TAG, "Wrong time data");
		return;
	}

	schedule.time.hour = strtod(data, &endptr);
	schedule.time.min  = strtod(endptr, &endptr);
	schedule.time.sec  = strtod(endptr, &endptr);

	err = alarm_create(ALARM_TYPE_WAKEUP, &schedule, true);
	if (err) {
		ESP_LOGE(TAG, "on_alarm_add err %s", esp_err_to_name(err));
	}

	ESP_LOGI(TAG, "Alarm added - time : %d:%d:%d", schedule.time.hour, schedule.time.min, schedule.time.sec);
}

static void on_alarm_remove(long id, void *arg)
{
	esp_err_t err;

	err = alarm_delete(id);
	if (err) {
		ESP_LOGE(TAG, "on_alarm_remove err %s", esp_err_to_name(err));
	}
}

static bool on_alarm_iterate(alarm_id_t id, alarm_type_id_t type_id, bool active,
                             struct alarm_schedule schedule, const void *user_data)
{
	ESP_LOGI(TAG, "\tAlarm %ld - Active: %s - Type: %d - Time: %dh %dmin %dsec", id,
	         active ? "True " : "False", type_id, schedule.time.hour, schedule.time.min,
	         schedule.time.sec);

	return ESP_OK;
}

static void on_alarm_list(const void *data, void *arg)
{
	alarm_iterate(on_alarm_iterate, NULL);
}

static void on_alarm_clear(const void *data, void *arg)
{
	esp_err_t err;

	err = alarm_clear();
	if (err) {
		ESP_LOGE(TAG, "Failed to clear alarms: %s", esp_err_to_name(err));
	}

	ESP_LOGI(TAG, "Alarms cleared");
}

static struct urp_message_handler handlers[] = {
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_LON, onLightOnOff,     NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_LMO, onLightMode,      NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_RGB, onLightModeRgb,   NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_POW, onLightModePower, NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_SPE, onLightModeSpeed, NULL),
	URP_MESSAGE_STRING_HANDLER(VALUE_TYPE_TZC, on_time_zone_change, NULL),
	URP_MESSAGE_STRING_HANDLER(VALUE_TYPE_AAD, on_alarm_add, NULL),
	URP_MESSAGE_INT_HANDLER(VALUE_TYPE_ARM, on_alarm_remove, NULL),
	URP_MESSAGE_FIXED_HANDLER(VALUE_TYPE_ALS, 0, on_alarm_list,  NULL),
	URP_MESSAGE_FIXED_HANDLER(VALUE_TYPE_ACL, 0, on_alarm_clear, NULL),
};

static struct urp_config urp_config = {
	.handlers      = handlers,
	.handlers_size = ARRAY_SIZE(handlers),
	.uart_num      = UART_NUM_0,
	.uart_config   = {
		.baud_rate  = CONFIG_LUMOS_UART_COM_BAUD_RATE,
		.data_bits  = UART_DATA_8_BITS,
		.parity     = UART_PARITY_DISABLE,
		.stop_bits  = UART_STOP_BITS_1,
		.flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_DEFAULT,
	},
	.tx_io_num = 1,
	.rx_io_num = 3,
	.core_id   = CONFIG_LUMOS_COM_CORE_ID
};

esp_err_t uart_com_init(void)
{
	ESP_LOGI(TAG, "Initializing uart");

	return urp_init(&urp_config);
}

void uart_com_register_callbacks(struct uart_com_callbacks cbks)
{}
