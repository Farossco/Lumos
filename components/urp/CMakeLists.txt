idf_component_register(
	SRCS urp.c
    INCLUDE_DIRS .
	REQUIRES driver
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE")