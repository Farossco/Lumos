/**
 * @file wifi_com.cpp
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-04-09
 *
 * @brief
 */

#include "wifi_com.h"
#include <string.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_wifi.h>
#include <esp_wpa2.h>
#include <esp_event.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <esp_netif.h>
#include <esp_smartconfig.h>
#include <sntp.h>
#include "temp_log_util.h"
#include "http_server.h"
#include "kconfig_stub.h"

#define CONNECTED_BIT     BIT0
#define ESPTOUCH_DONE_BIT BIT1

static EventGroupHandle_t wifi_com_event_group_handle;

static const struct wifi_com_conn_callbacks *conn_callbacks;

static const char *TAG = "wifi_com";

static void on_sntp_time_sync(struct timeval *tv)
{
	char strftime_buf[64];
	struct tm timeinfo;

	setenv("TZ", TIME_ZONE, 1);
	tzset();

	localtime_r(&tv->tv_sec, &timeinfo);
	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	ESP_LOGI(TAG, "The current date/time in Paris is: %s", strftime_buf);
}

static void wifi_com_sntp_init(void)
{
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_set_time_sync_notification_cb(on_sntp_time_sync);
	sntp_init();

	ESP_LOGD(TAG, "SNTP initialized!");
}

static void wifi_com_smartconfig_task(void *parm)
{
	EventBits_t uxBits;
	ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH) );
	smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_smartconfig_start(&cfg) );
	while (1) {
		uxBits = xEventGroupWaitBits(wifi_com_event_group_handle, CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);
		if (uxBits & CONNECTED_BIT) {
			ESP_LOGI(TAG, "WiFi Connected to ap");
		}
		if (uxBits & ESPTOUCH_DONE_BIT) {
			ESP_LOGI(TAG, "smartconfig over");
			esp_smartconfig_stop();

			wifi_com_sntp_init();

			http_server_start(); /* TODO: replace with callback */

			if (conn_callbacks && conn_callbacks->on_connected) {
				conn_callbacks->on_connected();
			} else {
				ESP_LOGW(TAG, "on_connect callback not set\n");
			}

			vTaskDelete(NULL);
		}
	}
}

static void wifi_com_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	if (event_base == WIFI_EVENT) {
		if (event_id == WIFI_EVENT_STA_START) {
			ESP_LOGD(TAG, "Event: WIFI_EVENT_STA_START");
			ESP_LOGD(TAG, "Starting Smart Config task");
			xTaskCreate(wifi_com_smartconfig_task, "wifi_com_smartconfig_task", 4096, NULL, 3, NULL);
		} else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
			ESP_LOGD(TAG, "Event: WIFI_EVENT_STA_DISCONNECTED");
			ESP_LOGD(TAG, "Attempting reconnection");
			esp_wifi_connect();
			xEventGroupClearBits(wifi_com_event_group_handle, CONNECTED_BIT);
		}
	} else if (event_base == IP_EVENT) {
		if (event_id == IP_EVENT_STA_GOT_IP) {
			ESP_LOGD(TAG, "Event: IP_EVENT_STA_GOT_IP");
			xEventGroupSetBits(wifi_com_event_group_handle, CONNECTED_BIT);
		}
	} else if (event_base == SC_EVENT) {
		if (event_id == SC_EVENT_SCAN_DONE) {
			ESP_LOGI(TAG, "Event: SC_EVENT_SCAN_DONE");
		} else if (event_id == SC_EVENT_FOUND_CHANNEL) {
			ESP_LOGI(TAG, "Event: SC_EVENT_FOUND_CHANNEL");
		} else if (event_id == SC_EVENT_GOT_SSID_PSWD) {
			ESP_LOGI(TAG, "Even: SC_EVENT_GOT_SSID_PSWD");

			smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
			wifi_config_t wifi_config;

			ESP_LOGI(TAG, "SSID:%s",     evt->ssid);
			ESP_LOGI(TAG, "PASSWORD:%s", evt->password);

			memset(&wifi_config, 0, sizeof(wifi_config_t));
			memcpy(wifi_config.sta.ssid,     evt->ssid, sizeof(wifi_config.sta.ssid));
			memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
			wifi_config.sta.bssid_set = evt->bssid_set;
			if (wifi_config.sta.bssid_set == true) {
				memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
			}

			ESP_ERROR_CHECK(esp_wifi_disconnect());
			ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
			esp_wifi_connect();
		} else if (event_id == SC_EVENT_SEND_ACK_DONE) {
			ESP_LOGI(TAG, "Even: SC_EVENT_SEND_ACK_DONE");
			xEventGroupSetBits(wifi_com_event_group_handle, ESPTOUCH_DONE_BIT);
		}
	}
}

void wifi_com_init(void)
{
	ESP_LOGI(TAG, "Initializing wifi");

	wifi_com_event_group_handle = xEventGroupCreate();

	ESP_ERROR_CHECK(nvs_flash_init());

	ESP_ERROR_CHECK(esp_netif_init());

	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
	assert(sta_netif);

	wifi_init_config_t cfg = {
		.event_handler          = &esp_event_send_internal,
		.osi_funcs              = &g_wifi_osi_funcs,
		.wpa_crypto_funcs       = g_wifi_default_wpa_crypto_funcs,
		.static_rx_buf_num      = CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM,
		.dynamic_rx_buf_num     = CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM,
		.tx_buf_type            = CONFIG_ESP32_WIFI_TX_BUFFER_TYPE,
		.static_tx_buf_num      = WIFI_STATIC_TX_BUFFER_NUM,
		.dynamic_tx_buf_num     = WIFI_DYNAMIC_TX_BUFFER_NUM,
		.cache_tx_buf_num       = WIFI_CACHE_TX_BUFFER_NUM,
		.csi_enable             = WIFI_CSI_ENABLED,
		.ampdu_rx_enable        = WIFI_AMPDU_RX_ENABLED,
		.ampdu_tx_enable        = WIFI_AMPDU_TX_ENABLED,
		.amsdu_tx_enable        = WIFI_AMSDU_TX_ENABLED,
		.nvs_enable             = WIFI_NVS_ENABLED,
		.nano_enable            = WIFI_NANO_FORMAT_ENABLED,
		.rx_ba_win              = WIFI_DEFAULT_RX_BA_WIN,
		.wifi_task_core_id      = CONFIG_NETWORK_CORE_ID,
		.beacon_max_len         = WIFI_SOFTAP_BEACON_MAX_LEN,
		.mgmt_sbuf_num          = WIFI_MGMT_SBUF_NUM,
		.feature_caps           = g_wifi_feature_caps,
		.sta_disconnected_pm    = WIFI_STA_DISCONNECTED_PM_ENABLED,
		.espnow_max_encrypt_num = CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM,
		.magic                  = WIFI_INIT_CONFIG_MAGIC
	};

	ESP_ERROR_CHECK(esp_wifi_init(&cfg) );

	/* TODO: Maybe handle all events */
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_com_event_handler, NULL) );
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_com_event_handler, NULL) );
	ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &wifi_com_event_handler, NULL) );

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_ERROR_CHECK(esp_wifi_start() );
}

void wifi_com_register_conn_callbacks(const struct wifi_com_conn_callbacks *callbacks)
{
	conn_callbacks = callbacks;
}
