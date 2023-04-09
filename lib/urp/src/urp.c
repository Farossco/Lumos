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
#define URP_SEND_BUF_SIZE      CONFIG_URP_SEND_BUF_SIZE


#define TAG "urp"

#undef ESP_LOGE
#undef ESP_LOGW
#undef ESP_LOGD
#undef ESP_LOGV
#undef ESP_LOGI
#undef ESP_LOG_BUFFER_HEX

#define ESP_LOGE(TAG, ...)                        printf("[%s] [ERR] ", TAG); printf(__VA_ARGS__); printf("\n")
#define ESP_LOGW(TAG, ...)                        printf("[%s] [WRN] ", TAG); printf(__VA_ARGS__); printf("\n")
#define ESP_LOGD(TAG, ...)                        printf("[%s] [DBG] ", TAG); printf(__VA_ARGS__); printf("\n")
#define ESP_LOGV(TAG, ...)                        printf("[%s] [VER] ", TAG); printf(__VA_ARGS__); printf("\n")
#define ESP_LOGI(TAG, ...)                        printf("[%s] [INF] ", TAG); printf(__VA_ARGS__); printf("\n")
#define ESP_LOG_BUFFER_HEX(tag, buffer, buff_len) esp_log_buffer_hex_internal(tag, buffer, buff_len, ESP_LOG_INFO)

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

	char         msg_type_buf[URP_VALUE_TYPE_LENGTH];
	uint16_t     checksum;
	uint8_t      *data_buf;
	size_t       data_buf_size;
	const size_t data_buf_max_size;
	bool         escaped;
};

/**
 * @brief Data structure for URP message encoding
 *
 */
struct urp_send_message {
	const uint8_t *data_buf;
	size_t        data_size;
	const char    *msg_type;
};

/**
 * @brief Calls the configured handler for the message type
 *
 * @param message The received message data
 * @param handler The handler to be called
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
	} else if (handler->data_size == URP_SIZE_STRING) {
		message->data_buf[message->data_buf_size] = '\0';

		if (handler->cb_string) {
			return handler->cb_string((const char *)message->data_buf, handler->arg);
		}
	} else if (handler->data_size < 0) { /* Any other data lower than 0 is invalid */
		char msg_type_str[URP_VALUE_TYPE_LENGTH + 1];

		memcpy(msg_type_str, handler->msg_type_buf, URP_VALUE_TYPE_LENGTH);
		msg_type_str[URP_VALUE_TYPE_LENGTH] = '\0';
		ESP_LOGE(TAG, "Incorrect data size %d for mode \"%s\"", handler->data_size, message->data_buf, errno);
		return;
	} else {
		if (message->data_buf_size != handler->data_size) {
			char msg_type_str[URP_VALUE_TYPE_LENGTH + 1];

			memcpy(msg_type_str, message->msg_type_buf, URP_VALUE_TYPE_LENGTH);
			msg_type_str[URP_VALUE_TYPE_LENGTH] = '\0';

			ESP_LOGE(TAG, "Received URP data of type \"%s\" with incorrect size (%d should be %d)",
			         msg_type_str, message->data_buf_size, handler->data_size);
			return;
		}

		if (handler->cb_fixed) {
			return handler->cb_fixed(message->data_buf, handler->arg);
		}
	}

	char msg_type_str[URP_VALUE_TYPE_LENGTH + 1];

	memcpy(msg_type_str, handler->msg_type_buf, URP_VALUE_TYPE_LENGTH);
	msg_type_str[URP_VALUE_TYPE_LENGTH] = '\0';

	ESP_LOGE(TAG, "No callback defined for %s", msg_type_str);
}

/**
 * @brief Get a handler from a message type buffer.
 *
 * The buffer needs to have at least a size of URP_VALUE_TYPE_LENGTH.
 *
 * @param config       The URP configuration object
 * @param msg_type_buf The message type buffer (not NULL-terminated)
 *
 * @return The matching handler or NULL if no match could be found.
 */
static struct urp_message_handler * handler_from_msg_type_buf(struct urp_config *config, const uint8_t *msg_type_buf)
{
	if (!config->handlers) {
		ESP_LOGE(TAG, "No handler defined");
		return NULL;
	}

	for (int i = 0; i < config->handlers_size; i++) {
		if (memcmp(msg_type_buf, config->handlers[i].msg_type_buf, URP_VALUE_TYPE_LENGTH) == 0) {
			return &config->handlers[i];
		}
	}

	char msg_type_str[URP_VALUE_TYPE_LENGTH + 1];

	memcpy(msg_type_str, msg_type_buf, URP_VALUE_TYPE_LENGTH);
	msg_type_str[URP_VALUE_TYPE_LENGTH] = '\0';

	ESP_LOGE(TAG, "Given message type \"%s\" is not registered", msg_type_str);

	return NULL;
}

/**
 * @brief Get a handler from a message type string.
 *
 * @param msg_type_str The message type in string (NULL-terminated) format.
 *
 * @return The matching handler or NULL if no match could be found.
 */
static struct urp_message_handler * handler_from_msg_type_str(struct urp_config *config, const char *msg_type_str)
{
	size_t len = strlen(msg_type_str);

	if (len != URP_VALUE_TYPE_LENGTH) {
		ESP_LOGE(TAG, "Invalid data type, length is %d, must be %d", len, URP_VALUE_TYPE_LENGTH);
		return NULL;
	}

	return handler_from_msg_type_buf(config, (const uint8_t *)msg_type_str);
}

/**
 * @brief Calculate checksum from message message type and message data
 *
 * @param msg_type_str The message type string
 * @param data_buf     The message data array
 * @param data_size    The size of the data array
 * @param checksum     The outputted checksum
 *
 * @return 0 if success, negative data otherwise
 */
static int calculate_checksum(const char *msg_type_str, const uint8_t *data_buf, size_t data_size, uint16_t *checksum)
{
	uint32_t sum = 0;

	if (!msg_type_str || !data_buf || !checksum) {
		return -EINVAL;
	}

	for (int i = 0; i < URP_VALUE_TYPE_LENGTH; i++) {
		sum += msg_type_str[i];
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
 * @brief Verify the integrity of the message with the checksum
 *
 * @param message The message containig the data to verify
 *
 * @return 0 if success,
 *         -EBADMSG if checksum verification failed
 */
static int verify_checksum(struct urp_recv_message *message)
{
	int err;
	uint16_t calculated_checksum;

	err = calculate_checksum(message->msg_type_buf, message->data_buf,
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
 * @brief Fills the decoded data structure with the content of the ring buffer
 *        while decoding it
 *
 * @param config  The URP configuration object
 * @param message The decoded URP message to be filled
 *
 * @return -EAGAIN to request more data
 *         0 if success, negative value otherwise
 */
static int urp_data_fill(struct urp_config *config, struct urp_recv_message *message)
{
	while (1) {
		uint8_t c;
		uint8_t cs_offset = message->has_checksum ? 2 : 0;
		size_t rx;

		rx = uart_read_bytes(config->uart_num, &c, 1, 0);

		if (rx == 0) {
			return -EAGAIN;
		}

		ESP_LOGD(TAG, "Character: %03d - 0x%02X - \'%c\' - cursor: %d", c, c, c, message->cursor);

		if (!message->escaped && c == URP_CHAR_END) {
			ESP_LOGD(TAG, "End of message");

			if (message->cursor <= URP_VALUE_TYPE_LENGTH + cs_offset) {
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
			message->msg_type_buf[message->cursor - 1] = c;
			ESP_LOGD(TAG, "msg_type_buf[%d] = %c (%d)", message->cursor - 1, c, c);
		} else if (message->has_checksum && message->cursor == URP_VALUE_TYPE_LENGTH + 1) {
			message->checksum = (((uint16_t)c) << 8) & 0xFF00;
			ESP_LOGD(TAG, "Checksum MSB");
		} else if (message->has_checksum && message->cursor == URP_VALUE_TYPE_LENGTH + 2) {
			message->checksum |= c;
			ESP_LOGD(TAG, "Checksum LSB - CS: %d (0x%04X)", message->checksum, message->checksum);
		} else {
			/* Leaving 1 byte of data available in the buffer to add a \0 character if needed */
			if (message->data_buf_size >= message->data_buf_max_size - 1) {
				ESP_LOGE(TAG, "Not enough space in buffer");
				return -ENOMEM;
			}

			message->data_buf[message->data_buf_size] = c;
			ESP_LOGD(TAG, "data_buf[%d] = %c (%d)", message->data_buf_size, c, c);

			message->data_buf_size++;
			ESP_LOGD(TAG, "data_buf_size = %d",     message->data_buf_size);
		}

		message->cursor++;
		message->escaped = false;
	}
}

/**
 * @brief Data receiver thread. This thread waits for new data on the input ring buffer
 *        and calls urp_data_fill to decode the data and fill the message structure
 *
 * @param arg Task parameter - Unused
 */
static void urp_recv_task(void *arg)
{
	struct urp_config *config              = (struct urp_config *)arg;
	uint8_t rx_buf[URP_RECV_DATA_SIZE_MAX] = { 0 };
	uart_event_t event;
	struct urp_recv_message message = {
		.cursor            = 0,
		.has_checksum      = false,
		.msg_type_buf      = { 0 },
		.checksum          = 0,
		.data_buf          = rx_buf,
		.data_buf_size     = 0,
		.data_buf_max_size = sizeof(rx_buf),
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
		err = urp_data_fill(config, &message);

		if (err == -EAGAIN) {
			taskYIELD();
			continue;
		} else if (err < 0) {
			message.cursor  = 0;
			message.escaped = false;
			taskYIELD();
			goto fill;
		}

		const struct urp_message_handler *handler = handler_from_msg_type_buf(config, message.msg_type_buf);

		if (handler) {
			handle_data(&message, handler);
			goto clear;
		}

		char msg_type_str[URP_VALUE_TYPE_LENGTH + 1];

		memcpy(msg_type_str, message.msg_type_buf, URP_VALUE_TYPE_LENGTH);
		msg_type_str[URP_VALUE_TYPE_LENGTH] = '\0';

		ESP_LOGW(TAG, "No match for %s", msg_type_str);

clear:
		message.cursor = 0;

		taskYIELD();
		goto fill;
	}
}

/**
 * Creates the output message from the provided data
 *
 * @param config  The URP configuration object
 * @param message The message data to encode
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

	calculate_checksum(message->msg_type, message->data_buf, message->data_size, &checksum);
	checksum_buf[0] = (uint8_t)((checksum & 0xFF00) >> 8);
	checksum_buf[1] = (uint8_t)(checksum & 0x00FF);

	uart_tx_chars(config->uart_num, &start, 1);
	uart_tx_chars(config->uart_num, message->msg_type, URP_VALUE_TYPE_LENGTH);
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

/* ==================================================================================== */
/* ================================= Public functions ================================= */
/* ==================================================================================== */

int urp_send_fixed(struct urp_config *config, const char *msg_type_str, const void *data_buf)
{
	struct urp_send_message message;
	const struct urp_message_handler *handler;

	handler = handler_from_msg_type_str(config, msg_type_str);
	if (!handler) {
		return -EINVAL;
	}

	if (handler->data_size < 0) {
		ESP_LOGE(TAG, "\'%s\' can only be called for fixed size messages", __func__);
		return -EINVAL;
	}

	message.msg_type  = msg_type_str;
	message.data_buf  = (const uint8_t *)data_buf;
	message.data_size = handler->data_size;

	return urp_data_send(config, &message);
}

int urp_send_variable(struct urp_config *config, const char *msg_type_str, const void *data, size_t data_size)
{
	struct urp_send_message message;
	const struct urp_message_handler *handler;

	handler = handler_from_msg_type_str(config, msg_type_str);
	if (!handler) {
		return -EINVAL;
	}

	if (handler->data_size != URP_SIZE_VARIABLE) {
		ESP_LOGE(TAG, "\'%s\' can only be called for non-fixed size messages", __func__);
		return -EINVAL;
	}

	message.msg_type  = msg_type_str;
	message.data_buf  = (const uint8_t *)data;
	message.data_size = data_size;

	return urp_data_send(config, &message);
}

int urp_send_int(struct urp_config *config, const char *msg_type_str, long data)
{
	struct urp_send_message message;
	const struct urp_message_handler *handler;
	char data_buf[20];
	ssize_t data_size;

	handler = handler_from_msg_type_str(config, msg_type_str);
	if (!handler) {
		return -EINVAL;
	}

	if (handler->data_size != URP_SIZE_INT) {
		ESP_LOGE(TAG, "\'%s\' can only be called for integer messages", __func__);
		return -EINVAL;
	}

	data_size = sprintf(data_buf, "%ld", data);
	if (data_size < 0) {
		return data_size;
	}

	message.msg_type  = msg_type_str;
	message.data_buf  = (const uint8_t *)data_buf;
	message.data_size = data_size;

	return urp_data_send(config, &message);
}

int urp_send_float(struct urp_config *config, const char *msg_type_str, double data)
{
	struct urp_send_message message;
	const struct urp_message_handler *handler;
	char data_buf[20];
	ssize_t data_size;

	handler = handler_from_msg_type_str(config, msg_type_str);
	if (!handler) {
		return -EINVAL;
	}

	if (handler->data_size != URP_SIZE_FLOAT) {
		ESP_LOGE(TAG, "\'%s\' can only be called for floating point messages", __func__);
		return -EINVAL;
	}

	data_size = sprintf(data_buf, "%Lf", data);
	if (data_size < 0) {
		return data_size;
	}

	message.msg_type  = msg_type_str;
	message.data_buf  = (const uint8_t *)data_buf;
	message.data_size = data_size;

	return urp_data_send(config, &message);
}

int urp_send_string(struct urp_config *config, const char *msg_type_str, const char *data)
{
	struct urp_send_message message;
	const struct urp_message_handler *handler;

	handler = handler_from_msg_type_str(config, msg_type_str);
	if (!handler) {
		return -EINVAL;
	}

	if (handler->data_size != URP_SIZE_STRING) {
		ESP_LOGE(TAG, "\'%s\' can only be called for string messages", __func__);
		return -EINVAL;
	}

	message.msg_type  = msg_type_str;
	message.data_buf  = (const uint8_t *)data;
	message.data_size = strlen(data);

	return urp_data_send(config, &message);
}

int urp_init(struct urp_config *config)
{
	/* Configure UART parameters */
	ESP_ERROR_CHECK(uart_driver_install(config->uart_num, URP_RECV_BUF_SIZE, URP_SEND_BUF_SIZE, 20, &config->task_queue, 0));
	ESP_ERROR_CHECK(uart_param_config(config->uart_num, &config->uart_config));
	ESP_ERROR_CHECK(uart_set_pin(config->uart_num, config->tx_io_num, config->rx_io_num,
	                             UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

	/* Create recv task */
	xTaskCreate(urp_recv_task, "urp_task", 4048, config, URP_RECV_THREAD_PRIO, NULL);

	return 0;
}
