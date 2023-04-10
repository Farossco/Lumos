#ifndef KCONFIG_STUB
#define KCONFIG_STUB

/* Temporary until Kconfig implementation */
#define CONFIG_NETWORK_CORE_ID                1
#define CONFIG_HTTP_CLIENT_TIME_HOST          "api.timezonedb.com" /* HTPP time host */
#define CONFIG_HTTP_CLIENT_TIME_FORMAT        "json"               /* Format for receive the time */
#define CONFIG_HTTP_CLIENT_TIME_KEY           ESP_TIME_API_KEY     /* TimeZoneDB API key */
#define CONFIG_HTTP_CLIENT_TIME_BY            "zone"               /* localization method */
#define CONFIG_HTTP_CLIENT_TIME_FIELDS        "timestamp"          /* Needed fields in the answer */
#define CONFIG_LED_STRIP_SPI_USING_SK9822     1
#define CONFIG_LED_STRIP_SPI_MUTEX_TIMEOUT_MS 1
#endif /* ifndef KCONFIG_STUB */
