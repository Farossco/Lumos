function(generate_settings
		input_keys
		input_groups
		input_defs
		output_header
		output_header_internal
		config_key_length_max
		config_group_length_max
		)

	idf_build_get_property(python PYTHON)

	message("Generating Settings lib header files")

	execute_process(
			COMMAND
			${python}
			${CMAKE_CURRENT_SOURCE_DIR}/gen_settings.py
			--input_keys ${PROJECT_DIR}/${input_keys}
			--input_groups ${PROJECT_DIR}/${input_groups}
			--input_defs ${PROJECT_DIR}/${input_defs}
			--header ${output_header}
			--header_internal ${output_header_internal}
			--config_key_length_max ${config_key_length_max}
			--config_group_length_max ${config_group_length_max}
			RESULT_VARIABLE generate_settings_result
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)

	if (${generate_settings_result})
		message(FATAL_ERROR "Exception happened while generating settings definitions")
	endif()
endfunction()

set(srcs settings.c)

if(CONFIG_SETTINGS_IMPL_NVS)
	list(APPEND srcs impl/settings_impl_nvs.c)
endif()

idf_component_register(
	SRCS ${srcs}
    INCLUDE_DIRS include
	PRIV_INCLUDE_DIRS priv_include
	PRIV_REQUIRES nvs_flash freertos log
)

generate_settings(
		${CONFIG_SETTINGS_PATH_KEYS}
		${CONFIG_SETTINGS_PATH_GROUPS}
		${CONFIG_SETTINGS_PATH_DEFS}
		${CMAKE_BINARY_DIR}/config/settings_def.h
		${CMAKE_BINARY_DIR}/config/settings_def_internal.h
		${CONFIG_SETTINGS_KEY_NAME_LENGTH_MAX}
		${CONFIG_SETTINGS_GROUP_NAME_LENGTH_MAX})

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE")
