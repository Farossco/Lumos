idf_component_register(
	SRCS alarm.c alarm_item_dynamic.c
    INCLUDE_DIRS include
	PRIV_INCLUDE_DIRS priv_include
	PRIV_REQUIRES nvs_flash freertos log
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE")