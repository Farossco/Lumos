/**
 * @file urp.c
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-04-07
 *
 * @brief
 */
#include "urp.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

#define URP_CHAR_START         CONFIG_URP_CHAR_START
#define URP_CHAR_START_NC      CONFIG_URP_CHAR_START_NC
#define URP_CHAR_END           CONFIG_URP_CHAR_END
#define URP_CHAR_ESCAPE        CONFIG_URP_CHAR_ESCAPE
#define URP_VALUE_TYPE_LENGTH  CONFIG_URP_VALUE_TYPE_LENGTH
#define URP_RECV_THREAD_PRIO   CONFIG_URP_RECV_THREAD_PRIO
#define URP_RECV_BUF_SIZE      CONFIG_URP_RECV_BUF_SIZE
#define URP_RECV_DATA_SIZE_MAX CONFIG_URP_RECV_DATA_SIZE_MAX


#define TAG "urp"

#define ESP_LOGE(TAG, ...) printf("["TAG "] [ERR] "); printf(__VA_ARGS__); printf("\n")
#define ESP_LOGD(TAG, ...) printf("["TAG "] [DBG] "); printf(__VA_ARGS__); printf("\n")
#define ESP_LOGW(TAG, ...) printf("["TAG "] [WRN] "); printf(__VA_ARGS__); printf("\n")

#define URP_VALUE_NEEDS_ESCAPE(_value)                                  \
	(((_value) == URP_CHAR_START) || ((_value) == URP_CHAR_START_NC) || \
	 ((_value) == URP_CHAR_END) || ((_value) == URP_CHAR_ESCAPE))

/**
 * @brief Data structure for URP message decoding
 *
 */
struct urp_recv_message {
	uint16_t     cursor;
	bool         has_checksum;

	char         data_type_buf[URP_VALUE_TYPE_LENGTH];
	uint16_t     checksum;
	uint8_t      *data_buf;
	size_t       data_buf_size;
	const size_t data_buf_max_size;
	bool         escaped;
};

/**
 * Data structure for URP message encoding
 */
struct urp_send_message {
	const uint8_t *data_buf;
	size_t        data_size;
	const char    *data_type;
};

/**
 * Calls the configured handler for the data type
 */
static void handle_data(const struct urp_recv_message    *message,
                        const struct urp_message_handler *handler)
{
	if (handler->data_size == URP_SIZE_VARIABLE) {
		if (handler->cb_variable) {
			return handler->cb_variable(message->data_buf, message->data_buf_size, handler->arg);
		}
	} else if (handler->data_size == URP_SIZE_INT) {
		long data;
		char *end_ptr;

		message->data_buf[message->data_buf_size] = '\0';

		data = strtol(message->data_buf, &end_ptr, 10);
		if (end_ptr == (char *)message->data_buf) {
			ESP_LOGE(TAG, "Received incorrect int data: \"%s\" (%d)", message->data_buf, errno);
			return;
		}

		if (handler->cb_int) {
			return handler->cb_int(data, handler->arg);
		}
	} else if (handler->data_size == URP_SIZE_FLOAT) {
		double data;
		char *end_ptr;

		message->data_buf[message->data_buf_size] = '\0';

		data = strtod(message->data_buf, &end_ptr);
		if (end_ptr == (char *)message->data_buf) {
			ESP_LOGE(TAG, "Received incorrect float data: \"%s\" (%d)", message->data_buf, errno);
			return;
		}

		if (handler->cb_float) {
			return handler->cb_float(data, handler->arg);
		}
	} else if (handler->data_size < 0) { /* Any other data lower than 0 is invalid */
		char data_type_str[URP_VALUE_TYPE_LENGTH + 1];

		memcpy(data_type_str, handler->data_type_buf, URP_VALUE_TYPE_LENGTH);
		data_type_str[URP_VALUE_TYPE_LENGTH] = '\0';
		ESP_LOGE(TAG, "Incorrect data size %d for mode \"%s\"", handler->data_size, message->data_buf, errno);
		return;
	} else {
		if (message->data_buf_size != handler->data_size) {
			char data_type_str[URP_VALUE_TYPE_LENGTH + 1];

			memcpy(data_type_str, message->data_type_buf, URP_VALUE_TYPE_LENGTH);
			data_type_str[URP_VALUE_TYPE_LENGTH] = '\0';

			ESP_LOGE(TAG, "Received URP data of type \"%s\" with incorrect size (%d should be %d)",
			         data_type_str, message->data_buf_size, handler->data_size);
			return;
		}

		if (handler->cb_fixed) {
			return handler->cb_fixed(message->data_buf, handler->arg);
		}
	}

	char data_type_str[URP_VALUE_TYPE_LENGTH + 1];

	memcpy(data_type_str, handler->data_type_buf, URP_VALUE_TYPE_LENGTH);
	data_type_str[URP_VALUE_TYPE_LENGTH] = '\0';

	ESP_LOGE(TAG, "No callback defined for %s", data_type_str);
}

/**
 * @brief Get a handler from a data type buffer.
 *
 * The buffer needs to have at least a size of URP_VALUE_TYPE_LENGTH.
 *
 * @param data_type_buf The data type in buffer (not NULL-terminated) format.
 *
 * @return The matching handler or NULL if no match could be found.
 */
static struct urp_message_handler * handler_from_buf(struct urp_config *config, const uint8_t *data_type_buf)
{
	if (!config->handlers) {
		ESP_LOGE(TAG, "No handler defined");
		return NULL;
	}

	for (int i = 0; i < config->handlers_size; i++) {
		if (memcmp(data_type_buf, config->handlers[i].data_type_buf, URP_VALUE_TYPE_LENGTH) == 0) {
			return &config->handlers[i];
		}
	}

	char data_type_str[URP_VALUE_TYPE_LENGTH + 1];

	memcpy(data_type_str, data_type_buf, URP_VALUE_TYPE_LENGTH);
	data_type_str[URP_VALUE_TYPE_LENGTH] = '\0';

	ESP_LOGE(TAG, "Given data type \"%s\" is not registered", data_type_str);

	return NULL;
}

/**
 * @brief Get a handler from a data type string.
 *
 * @param data_type_str The data type in string (NULL-terminated) format.
 *
 * @return The matching handler or NULL if no match could be found.
 */
static struct urp_message_handler * handler_from_str(struct urp_config *config, const char *data_type_str)
{
	size_t len = strlen(data_type_str);

	if (len != URP_VALUE_TYPE_LENGTH) {
		ESP_LOGE(TAG, "Invalid data type, length is %d, must be %d", len, URP_VALUE_TYPE_LENGTH);
		return NULL;
	}

	return handler_from_buf(config, (const uint8_t *)data_type_str);
}

/**
 * Calculate checksum from message data type and message data
 * @param data_type_str The message type string
 * @param data_str The message data array
 * @param data_size The size of the data array
 * @param checksum The outputted checksum
 * @return 0 if success, negative data otherwise
 */
static int calculate_checksum(const char *data_type_str, const uint8_t *data_buf,
                              size_t data_size, uint16_t *checksum)
{
	uint32_t sum = 0;

	if (!data_type_str || !data_buf || !checksum) {
		return -EINVAL;
	}

	for (int i = 0; i < URP_VALUE_TYPE_LENGTH; i++) {
		sum += data_type_str[i];
	}

	for (int i = 0; i < data_size; i++) {
		sum += data_buf[i];
	}

	while (sum >> 16) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}

	*checksum = ~sum;

	ESP_LOGD(TAG, "Checksum: 0x%X", *checksum);

	return 0;
}

/**
 * Verify the integrity of the message with the checksum
 * @param message The message conainig the data to verify
 * @return 0 if success,
 * -EBADMSG if checksum verification failed
 */
static int verify_checksum(struct urp_recv_message *message)
{
	int err;
	uint16_t calculated_checksum;

	err = calculate_checksum(message->data_type_buf, message->data_buf,
	                         message->data_buf_size, &calculated_checksum);
	if (err) {
		ESP_LOGE(TAG, "Error while calculating checksum: %d", err);
		return err;
	}

	if (calculated_checksum != message->checksum) {
		ESP_LOGD(TAG, "Checksums don't match! (%d (%04X) != %d (%04X))",
		         calculated_checksum, calculated_checksum,
		         message->checksum, message->checksum);
		return -EBADMSG;
	}

	return 0;
}

/**
 * Fills the decoded data structure with the content of the ring buffer
 * while decoding it
 */
static int urp_data_fill(struct urp_config *config, struct urp_recv_message *message, uart_event_t *event)
{
	while (1) {
		uint8_t c;
		uint8_t cs_offset = message->has_checksum ? 2 : 0;
		uint16_t data_cursor;
		size_t rx;

		rx = uart_read_bytes(config->uart_num, &c, 1, 0);

		if (rx == 0) {
			return -EAGAIN;
		}

		ESP_LOGD(TAG, "Character: %03d - 0x%02X - \'%c\' - cursor: %d", c, c, c, message->cursor);

		if (!message->escaped && c == URP_CHAR_END) {
			ESP_LOGD(TAG, "End of message");

			if (message->cursor <=
			    URP_VALUE_TYPE_LENGTH + cs_offset) {
				ESP_LOGD(TAG, "Message incomplete (cursor: %d)", message->cursor);
				return -EIO;
			}

			if (message->has_checksum) {
				return verify_checksum(message);
			}

			return 0;
		}

		if (!message->escaped && c == URP_CHAR_START) {
			message->has_checksum  = true;
			message->cursor        = 0;
			message->data_buf_size = 0;
			ESP_LOGD(TAG, "Start char, expecting checksum");
		} else if (!message->escaped && c == URP_CHAR_START_NC) {
			message->has_checksum  = false;
			message->cursor        = 0;
			message->data_buf_size = 0;
			ESP_LOGD(TAG, "Start char, expecting no checksum");
		} else if (!message->escaped && c == URP_CHAR_ESCAPE) {
			ESP_LOGD(TAG, "Escape char, escaping next character");
			message->escaped = true;
			continue; /* Do not increment cursor */
		} else if (message->cursor == 0) {
			ESP_LOGD(TAG, "The first received character is not %c nor %c!", URP_CHAR_START, URP_CHAR_START_NC);
			return -EIO;
		} else if (message->cursor <= URP_VALUE_TYPE_LENGTH) {
			message->data_type_buf[message->cursor - 1] = c;
			ESP_LOGD(TAG, "data_type_buf[%d] = %c (%d)", message->cursor - 1, c, c);
		} else if (message->has_checksum && message->cursor == URP_VALUE_TYPE_LENGTH + 1) {
			message->checksum = (((uint16_t)c) << 8) & 0xFF00;
			ESP_LOGD(TAG, "Checksum MSB");
		} else if (message->has_checksum && message->cursor == URP_VALUE_TYPE_LENGTH + 2) {
			message->checksum |= c;
			ESP_LOGD(TAG, "Checksum LSB - CS: %d (0x%04X)", message->checksum, message->checksum);
		} else {
			data_cursor = message->cursor - URP_VALUE_TYPE_LENGTH - 1 - cs_offset;

			message->data_buf[data_cursor] = c;
			message->data_buf_size++;

			/* Leaving 1 byte of data available in the buffer to add a \0 character if needed */
			if (data_cursor > message->data_buf_max_size) {
				ESP_LOGE(TAG, "Not enough space in buffer");
				return -ENOMEM;
			}

			ESP_LOGD(TAG, "data_buf[%d] = %c (%d)", data_cursor, c, c);
			ESP_LOGD(TAG, "data_buf_size = %d",     message->data_buf_size);
		}

		message->cursor++;
		message->escaped = false;
	}
}

/**
 * Data receiver thread. This thread waits for new data on the input ring buffer
 * and calls urp_data_fill to decode the data and fill de decode_data
 * structure
 */
static void urp_recv_task(void *arg)
{
	struct urp_config *config              = (struct urp_config *)arg;
	uint8_t rx_buf[URP_RECV_DATA_SIZE_MAX] = { 0 };
	uart_event_t event;
	struct urp_recv_message message = {
		.cursor            = 0,
		.data_buf          = rx_buf,
		.data_buf_size     = 0,
		.data_buf_max_size = sizeof(rx_buf),
		.has_checksum      = false,
		.escaped           = false
	};
	int err;

	while (true) {
		if (!xQueueReceive(config->task_queue, &event, portMAX_DELAY)) {
			continue;
		}
		if (event.type == UART_DATA) {
			/* Carry on */
		} else if (event.type == UART_FIFO_OVF) {
			ESP_LOGE(TAG, "hw fifo overflow");

			uart_flush_input(config->uart_num);
			xQueueReset(config->task_queue);
			continue;
		} else if (event.type == UART_BUFFER_FULL) {
			ESP_LOGE(TAG, "ring buffer full");

			uart_flush_input(config->uart_num);
			xQueueReset(config->task_queue);
			continue;
		} else {
			/* Log event */
			continue;
		}

fill:
		err = urp_data_fill(config, &message, &event);

		if (err == -EAGAIN) {
			taskYIELD();
			continue;
		} else if (err < 0) {
			message.cursor  = 0;
			message.escaped = false;
			taskYIELD();
			goto fill;
		}

		const struct urp_message_handler *handler = handler_from_buf(config, message.data_type_buf);

		if (handler) {
			handle_data(&message, handler);
			goto clear;
		}

		char data_type_str[URP_VALUE_TYPE_LENGTH + 1];

		memcpy(data_type_str, message.data_type_buf, URP_VALUE_TYPE_LENGTH);
		data_type_str[URP_VALUE_TYPE_LENGTH] = '\0';

		ESP_LOGW(TAG, "No match for %s", data_type_str);

clear:
		message.cursor = 0;

		taskYIELD();
		goto fill;
	}
}

/**
 * Creates the output message from the provided data
 *
 * @param data The data to encode
 *
 * @return 0 if success, negative data otherwise
 */
static int urp_data_send(struct urp_config *config, struct urp_send_message *message)
{
	uint16_t checksum;
	uint8_t checksum_buf[sizeof(checksum)];
	char start, end, escape;

	start  = URP_CHAR_START;
	end    = URP_CHAR_END;
	escape = URP_CHAR_ESCAPE;

	calculate_checksum(message->data_type, message->data_buf, message->data_size, &checksum);
	checksum_buf[0] = (uint8_t)((checksum & 0xFF00) >> 8);
	checksum_buf[1] = (uint8_t)(checksum & 0x00FF);

	uart_tx_chars(config->uart_num, &start, 1);
	uart_tx_chars(config->uart_num, message->data_type, URP_VALUE_TYPE_LENGTH);
	uart_tx_chars(config->uart_num, (const char *)checksum_buf, sizeof(checksum_buf));
	for (int i = 0; i < message->data_size; i++) {
		if (URP_VALUE_NEEDS_ESCAPE(message->data_buf[i])) {
			uart_tx_chars(config->uart_num, &escape, 1);
		}
		uart_tx_chars(config->uart_num, (const char *)&message->data_buf[i], 1);
	}
	uart_tx_chars(config->uart_num, &end, 1);

	return 0;
}

/** Public functions */

int urp_send(struct urp_config *config, const char *data_type_str, const void *data_buf)
{
	struct urp_send_message message;
	const struct urp_message_handler *handler;

	handler = handler_from_str(config, data_type_str);
	if (!handler) {
		return -EINVAL;
	}

	if (handler->data_size < 0) {
		ESP_LOGE(TAG, "\'%s\' can only be called for fixed size messages", __func__);
		return -EINVAL;
	}

	message.data_type = data_type_str;
	message.data_buf  = (const uint8_t *)data_buf;
	message.data_size = handler->data_size;

	return urp_data_send(config, &message);
}

int urp_send_variable(struct urp_config *config, const char *data_type_str, const void *data_buf, size_t size)
{
	struct urp_send_message message;
	const struct urp_message_handler *handler;

	handler = handler_from_str(config, data_type_str);
	if (!handler) {
		return -EINVAL;
	}

	if (handler->data_size != URP_SIZE_VARIABLE) {
		ESP_LOGE(TAG, "\'%s\' can only be called for non-fixed size messages", __func__);
		return -EINVAL;
	}

	message.data_type = data_type_str;
	message.data_buf  = (const uint8_t *)data_buf;
	message.data_size = size;

	return urp_data_send(config, &message);
}

int urp_init(struct urp_config *config)
{
	/* Configure UART parameters */
	ESP_ERROR_CHECK(uart_driver_install(config->uart_num, URP_RECV_BUF_SIZE, 0, 20, &config->task_queue, 0));
	ESP_ERROR_CHECK(uart_param_config(config->uart_num, &config->uart_config));
	ESP_ERROR_CHECK(uart_set_pin(config->uart_num, config->tx_io_num, config->rx_io_num,
	                             UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

	/* Create recv task */
	xTaskCreate(urp_recv_task, "urp_task", 4048, config, URP_RECV_THREAD_PRIO, NULL);

	return 0;
}
