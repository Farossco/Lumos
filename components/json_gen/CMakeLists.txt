idf_component_register(
	SRCS json_gen.c
    INCLUDE_DIRS include
	PRIV_REQUIRES log
	REQUIRES json
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE")