/**
 * @file wifi_com.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-05-11
 * 
 * @brief 
 */

#pragma once

#include <esp_err.h>

struct wifi_com_conn_callbacks {
	void (*on_connected)(void);
};

esp_err_t wifi_com_init(void);

void wifi_com_register_conn_callbacks(const struct wifi_com_conn_callbacks *callbacks);
