#ifndef UART_COM_H
#define UART_COM_H

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

#include <stdint.h>
#include <stddef.h>

struct uart_com_callbacks {
	void (*on_recv)(uint8_t *buf, size_t size);
};

void uart_com_init(void);

void uart_com_register_callbacks(struct uart_com_callbacks cbks);

#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* ifndef UART_COM_H */
