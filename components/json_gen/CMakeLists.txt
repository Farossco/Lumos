idf_component_register(
	SRCS json.c
    INCLUDE_DIRS include
	REQUIRES json
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE")