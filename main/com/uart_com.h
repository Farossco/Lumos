/**
 * @file uart_com.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-05-11
 * 
 * @brief 
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <esp_err.h>

struct uart_com_callbacks {
	void (*on_recv)(uint8_t *buf, size_t size);
};

esp_err_t uart_com_init(void);

void uart_com_register_callbacks(struct uart_com_callbacks cbks);
