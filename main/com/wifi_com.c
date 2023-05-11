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
#include <esp_netif.h>
#include <esp_netif_sntp.h>
#include <time.h>
#include <esp_smartconfig.h>
#include <settings.h>
#include "http/http_server.h"
#include "utils.h"

struct wifi_com_task_queue_data {
	enum {
		WIFI_COM_EVENT_WIFI_START,
		WIFI_COM_EVENT_WIFI_CONNECTED,
		WIFI_COM_EVENT_WIFI_DISCONNECTED
	} event_type;
};

static QueueHandle_t wifi_com_task_queue;

static const struct wifi_com_conn_callbacks *conn_callbacks;

static const char *TAG = "wifi_com";

static void on_sntp_time_sync(struct timeval *tv)
{
	char strftime_buf[64];
	struct tm timeinfo;

	setenv("TZ", CONFIG_LUMOS_TIME_ZONE, 1);
	tzset();

	localtime_r(&tv->tv_sec, &timeinfo);
	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	ESP_LOGI(TAG, "The current date/time in Paris is: %s", strftime_buf);
}

static esp_err_t wifi_com_initiate_smartconfig(void)
{
	smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();

	ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
	ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));

	return ESP_OK;
}

static esp_err_t wifi_com_connect_from_smartconfig(smartconfig_event_got_ssid_pswd_t *evt)
{
	wifi_config_t wifi_config;
	esp_err_t err;

	ESP_LOGI(TAG, "Connecting using:");
	ESP_LOGI(TAG, "SSID: \"%s\"",     evt->ssid);
	ESP_LOGI(TAG, "PASSWORD: \"%s\"", evt->password);

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

	ESP_LOGI(TAG, "Writing SSID and PASS to settings");

	err = SETTINGS_SET(WIFI_SSID, evt->ssid);
	if (err) {
		ESP_LOGE(TAG, "Failed to write SSID to settings");
		return err;
	}

	err = SETTINGS_SET(WIFI_PASS, evt->password);
	if (err) {
		ESP_LOGE(TAG, "Failed to write PASS to settings");
		return err;
	}

	return ESP_OK;
}

static esp_err_t wifi_com_connect_from_settings(void)
{
	wifi_config_t wifi_config = { 0 };
	esp_err_t err;

	err = SETTINGS_GET(WIFI_SSID, wifi_config.sta.ssid);
	if (err) {
		ESP_LOGE(TAG, "Failed to read SSID from settings");
		return err;
	}

	if (strlen((char *)wifi_config.sta.ssid) == 0) {
		ESP_LOGW(TAG, "No SSID stored in settings");
		return ESP_ERR_NOT_FOUND;
	}

	err = SETTINGS_GET(WIFI_PASS, wifi_config.sta.password);
	if (err) {
		ESP_LOGE(TAG, "Failed to read PASS from settings");
		return err;
	}

	ESP_LOGI(TAG, "Attempting connection from settings");

	ESP_LOGI(TAG, "SSID: \"%s\"",     wifi_config.sta.ssid);
	ESP_LOGI(TAG, "PASSWORD: \"%s\"", wifi_config.sta.password);

	err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
	if (err) {
		ESP_LOGE(TAG, "Failed to set wifi config: %s", err2str(err));
		return err;
	}

	err = esp_wifi_connect();
	if (err) {
		ESP_LOGE(TAG, "Failed to initiate wifi connection: %s", err2str(err));
		return err;
	}

	return ESP_OK;
}

static void wifi_com_conn_task(void *parm)
{
	esp_err_t err;
	struct wifi_com_task_queue_data queue_data;
	uint8_t connect_attempts = 0;

	while (1) {
		if (xQueueReceive(wifi_com_task_queue, &queue_data, portMAX_DELAY) == false) {
			continue;
		}

		switch (queue_data.event_type) {
		case WIFI_COM_EVENT_WIFI_START:
			err = wifi_com_connect_from_settings();
			if (err) {
				ESP_LOGW(TAG, "Could not connect from settings: %s", err2str(err));
				wifi_com_initiate_smartconfig();
			}
			break;

		case WIFI_COM_EVENT_WIFI_CONNECTED:
			wifi_ap_record_t ap_info;

			connect_attempts = 0;

			err = esp_wifi_sta_get_ap_info(&ap_info);
			if (err) {
				ESP_LOGE(TAG, "Failed to get AP info: %s", err2str(err));
			} else {
				ESP_LOGI(TAG, "WiFi successfully connected to %s", ap_info.ssid);
			}

			http_server_start(); /* TODO: replace with callback */

			if (conn_callbacks && conn_callbacks->on_connected) {
				conn_callbacks->on_connected();
			} else {
				ESP_LOGW(TAG, "on_connect callback not set\n");
			}
			break;

		case WIFI_COM_EVENT_WIFI_DISCONNECTED:
			if (connect_attempts++ >= 5) {
				ESP_LOGI(TAG, "Failed to connect to AP, initiating smartconfig");
				wifi_com_initiate_smartconfig();
				connect_attempts = 0;
			} else {
				ESP_LOGD(TAG, "Attempting reconnection");
				err = esp_wifi_connect();
				if (err) {
					ESP_LOGE(TAG, "Failed to initiate wifi connection: %s", err2str(err));
					err = wifi_com_initiate_smartconfig();
					{
						ESP_LOGE(TAG, "FATAL: Failed to configure smartconfig!: %s", err2str(err));
					}
				}
			}
			break;
		}
	}
}

static void wifi_com_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	struct wifi_com_task_queue_data queue_data;

	ESP_LOGV(TAG, "WIFI event: base: %s, id: %ld", event_base, event_id);

	if (event_base == WIFI_EVENT) {
		if (event_id == WIFI_EVENT_STA_START) {
			ESP_LOGD(TAG, "Event: WIFI_EVENT_STA_START");

			queue_data.event_type = WIFI_COM_EVENT_WIFI_START;
			if (!xQueueSendToBack(wifi_com_task_queue, &queue_data, 0)) {
				ESP_LOGE(TAG, "Failed to send item to queue!");
			}
		} else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
			ESP_LOGD(TAG, "Event: WIFI_EVENT_STA_DISCONNECTED");

			queue_data.event_type = WIFI_COM_EVENT_WIFI_DISCONNECTED;
			if (!xQueueSendToBack(wifi_com_task_queue, &queue_data, 0)) {
				ESP_LOGE(TAG, "Failed to send item to queue!");
			}
		}
	} else if (event_base == IP_EVENT) {
		if (event_id == IP_EVENT_STA_GOT_IP) {
			ESP_LOGD(TAG, "Event: IP_EVENT_STA_GOT_IP");

			queue_data.event_type = WIFI_COM_EVENT_WIFI_CONNECTED;
			if (!xQueueSendToBack(wifi_com_task_queue, &queue_data, 0)) {
				ESP_LOGE(TAG, "Failed to send item to queue!");
			}
		}
	} else if (event_base == SC_EVENT) {
		if (event_id == SC_EVENT_SCAN_DONE) {
			ESP_LOGV(TAG, "Event: SC_EVENT_SCAN_DONE");
		} else if (event_id == SC_EVENT_FOUND_CHANNEL) {
			ESP_LOGV(TAG, "Event: SC_EVENT_FOUND_CHANNEL");
		} else if (event_id == SC_EVENT_GOT_SSID_PSWD) {
			ESP_LOGV(TAG, "Event: SC_EVENT_GOT_SSID_PSWD");
			wifi_com_connect_from_smartconfig(event_data);
		} else if (event_id == SC_EVENT_SEND_ACK_DONE) {
			ESP_LOGD(TAG, "Even: SC_EVENT_SEND_ACK_DONE");

			ESP_LOGI(TAG, "smartconfig over");
			esp_smartconfig_stop();
		}
	}
}

esp_err_t wifi_com_init(void)
{
	esp_err_t err;
	esp_netif_t *sta_netif;
	wifi_init_config_t wifi_config = {
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
		.wifi_task_core_id      = CONFIG_LUMOS_COM_CORE_ID,
		.beacon_max_len         = WIFI_SOFTAP_BEACON_MAX_LEN,
		.mgmt_sbuf_num          = WIFI_MGMT_SBUF_NUM,
		.feature_caps           = g_wifi_feature_caps,
		.sta_disconnected_pm    = WIFI_STA_DISCONNECTED_PM_ENABLED,
		.espnow_max_encrypt_num = CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM,
		.magic                  = WIFI_INIT_CONFIG_MAGIC
	};

	esp_sntp_config_t sntp_config = {
		.smooth_sync                = false,
		.server_from_dhcp           = false,
		.wait_for_sync              = true,
		.start                      = true,
		.sync_cb                    = on_sntp_time_sync,
		.renew_servers_after_new_IP = false,
		.ip_event_to_renew          = 0,
		.index_of_first_server      = 0,
		.num_of_servers             = 1,
		.servers                    = { "pool.ntp.org" }
	};

	esp_log_level_set(TAG, ESP_LOG_VERBOSE);

	ESP_LOGI(TAG, "Initializing wifi");

	wifi_com_task_queue = xQueueCreate(10, sizeof(struct wifi_com_task_queue_data));
	if (wifi_com_task_queue == NULL) {
		ESP_LOGE(TAG, "Failed to create queue");
		return ESP_ERR_NO_MEM;
	}

	err = esp_netif_init();
	if (err) {
		ESP_LOGE(TAG, "Failed to initialize netif: %s", err2str(err));
		return err;
	}

	err = esp_event_loop_create_default();
	if (err) {
		ESP_LOGE(TAG, "Failed to create default event loop: %s", err2str(err));
		return err;
	}

	sta_netif = esp_netif_create_default_wifi_sta();
	if (!sta_netif) {
		ESP_LOGE(TAG, "Failed to create netif default wifi sta");
		return ESP_ERR_NO_MEM;
	}

	err = esp_wifi_init(&wifi_config);
	if (err) {
		ESP_LOGE(TAG, "Failed to initialize wifi: %s", err2str(err));
		return err;
	}

	err = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_com_event_handler, NULL);
	if (err) {
		ESP_LOGE(TAG, "Failed to register handler: %s", err2str(err));
		return err;
	}

	err = esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_com_event_handler, NULL);
	if (err) {
		ESP_LOGE(TAG, "Failed to register handler: %s", err2str(err));
		return err;
	}

	err = esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &wifi_com_event_handler, NULL);
	if (err) {
		ESP_LOGE(TAG, "Failed to register handler: %s", err2str(err));
		return err;
	}

	err = esp_wifi_set_mode(WIFI_MODE_STA);
	if (err) {
		ESP_LOGE(TAG, "Failed to set wifi STA mode: %s", err2str(err));
		return err;
	}

	err = esp_wifi_start();
	if (err) {
		ESP_LOGE(TAG, "Failed to start wifi: %s", err2str(err));
		return err;
	}

	err = esp_netif_sntp_init(&sntp_config);
	if (err) {
		ESP_LOGE(TAG, "Failed to initialize sntp: %s", err2str(err));
		return err;
	}

	if (!xTaskCreate(wifi_com_conn_task, "wifi_com_conn_task", 4096, NULL, 3, NULL)) {
		ESP_LOGE(TAG, "Failed to create wifi_com task");
	}

	return ESP_OK;
}

void wifi_com_register_conn_callbacks(const struct wifi_com_conn_callbacks *callbacks)
{
	conn_callbacks = callbacks;
}
