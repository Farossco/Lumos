/**
 * @file urp.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-04-07
 *
 * @brief
 */

#ifndef URP_H
#define URP_H

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

#include <stdint.h>
#include <driver/uart.h>
#include <esp_err.h>

/* Temporary definitions until Kconfig can be used */
#define CONFIG_URP_CHAR_START         '$'
#define CONFIG_URP_CHAR_START_NC      '&'
#define CONFIG_URP_CHAR_END           '*'
#define CONFIG_URP_CHAR_ESCAPE        '\\'
#define CONFIG_URP_RECV_BUF_SIZE      1024
#define CONFIG_URP_VALUE_TYPE_LENGTH  3
#define CONFIG_URP_RECV_TASK_PRIO     12
#define CONFIG_URP_RECV_DATA_SIZE_MAX 256
#define CONFIG_URP_SEND_BUF_SIZE      0

#define URP_SIZE_VARIABLE             ((size_t)-1)
#define URP_SIZE_INT                  ((size_t)-2)
#define URP_SIZE_FLOAT                ((size_t)-3)
#define URP_SIZE_STRING               ((size_t)-4)

/**
 * @brief Helper to declare a message handler with fixed size.
 */
#define URP_MESSAGE_FIXED_HANDLER(_type, _size, _cb, _arg) \
		{                                                      \
			.msg_type_str = (_type),                           \
			.data_size    = (_size),                           \
			.cb_fixed     = (_cb),                             \
			.arg          = _arg                               \
		}

/**
 * @brief Helper to declare a message handler with variable size.
 */
#define URP_MESSAGE_VARIABLE_HANDLER(_type, _cb, _arg) \
		{                                                  \
			.msg_type_str = (_type),                       \
			.data_size    = URP_SIZE_VARIABLE,             \
			.cb_variable  = (_cb),                         \
			.arg          = _arg                           \
		}

/**
 * @brief Helper to declare a message handler with integer type.
 */
#define URP_MESSAGE_INT_HANDLER(_type, _cb, _arg) \
		{                                             \
			.msg_type_str = (_type),                  \
			.data_size    = URP_SIZE_INT,             \
			.cb_int       = (_cb),                    \
			.arg          = _arg                      \
		}

/**
 * @brief Helper to declare a message handler with float type.
 */
#define URP_MESSAGE_FLOAT_HANDLER(_type, _cb, _arg) \
		{                                               \
			.msg_type_str = (_type),                    \
			.data_size    = URP_SIZE_FLOAT,             \
			.cb_float     = (_cb),                      \
			.arg          = _arg                        \
		}

/**
 * @brief Helper to declare a message handler with string type.
 */
#define URP_MESSAGE_STRING_HANDLER(_type, _cb, _arg) \
		{                                                \
			.msg_type_str = (_type),                     \
			.data_size    = URP_SIZE_STRING,             \
			.cb_string    = (_cb),                       \
			.arg          = _arg                         \
		}

struct urp_message_handler {
	/**
	 * @brief The string containing the data type.
	 *
	 * The size of this buffer must be of at least
	 * CONFIG_URP_VALUE_TYPE_LENGTH and remain valid for the entire time
	 * that URP data are received or sent.
	 */
	const char *msg_type_str;

	/**
	 * @brief The size of the messages for this type.
	 *
	 * Use urp_send to send messages with fixed size.
	 *
	 * For non-fixed sized messages, use URP_SIZE_NON_FIXED as the size and
	 * the urp_send_non_fixed() function.
	 */
	ssize_t data_size;

	/**
	 * @brief Extra argument to add to the callback
	 *
	 */
	void *arg;

	union {
		/**
		 * @brief Callback function.
		 *	  Called on URP data received.
		 *
		 * This callback gets called whenever data whose type is
		 * declared as fixed sized is received.
		 *
		 * @param data The message data.
		 */
		void (*cb_fixed)(const void *data, void *arg);

		/**
		 * @brief Callback function.
		 *	  Called on URP data received.
		 *
		 * This callback gets called whenever a data whose type is
		 * declared as variable sized is received.
		 *
		 * @param data The message data.
		 * @param size The size of the data.
		 */
		void (*cb_variable)(const void *data, size_t size, void *arg);

		void (*cb_int)(long data, void *arg);

		void (*cb_float)(double data, void *arg);

		void (*cb_string)(const char *data, void *arg);
	};
};

struct urp_config {
	struct urp_message_handler *handlers;
	size_t                     handlers_size;
	uart_port_t                uart_num;
	uart_config_t              uart_config;
	uint8_t                    tx_io_num;
	uint8_t                    rx_io_num;

	/* Private */
	QueueHandle_t              task_queue;
};

/**
 * Register receiver handlers
 * @param handlers The array of handlers
 * @param size The size of the array
 * @param uart_dev_name The UART device name
 * @return 0 if success, negative data otherwise
 */
esp_err_t urp_init(struct urp_config *config);

/**
 * @brief Send an URP message from the provided data type and data.
 *
 * Only messages types declared with fixed size can be sent via this function.
 *
 * @param data_type_str  The data type
 * @param data           The fixed size data
 *
 * @return 0 if success, negative data otherwise
 */
esp_err_t urp_send_fixed(struct urp_config *config, const char *data_type_str, const void *data);

/**
 * @brief Send an URP message from the provided data type and data.
 *
 * Only messages types declared with non-fixed size can be sent via this
 * function.
 *
 * @param data_type_str  The data type
 * @param data_buf       The message data buffer
 * @param data_size      The size of the data
 *
 * @return 0 if success, negative data otherwise
 */
esp_err_t urp_send_variable(struct urp_config *config, const char *data_type_str, const void *data, size_t data_size);

esp_err_t urp_send_int(struct urp_config *config, const char *data_type_str, long data);

esp_err_t urp_send_float(struct urp_config *config, const char *data_type_str, double data);

esp_err_t urp_send_string(struct urp_config *config, const char *data_type_str, const char *data);

#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* URP_H */
