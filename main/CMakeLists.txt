set(com_http_src com/http/http_client.c
                 com/http/http_server.c
                 com/http/http_server_cmd.c)

set(com_src ${com_http_src}
            com/uart_com.c
            com/wifi_com.c)
         
set(peripherals_src peripherals/sd_card.c
                    peripherals/light_strip.c)

set(src ${com_src}
        ${peripherals_src}
        main.c
        json.c
        light.c
        light_mode_task.c
        memory.c
        utils.c)

idf_component_register(SRCS ${src} 
                       INCLUDE_DIRS .)

target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE")

IF(EXISTS "${CMAKE_SOURCE_DIR}/app/dist")
        spiffs_create_partition_image(storage "${CMAKE_SOURCE_DIR}/app/dist" FLASH_IN_PROJECT)
ELSE()
        message(WARNING "Web app hasn't been built, spiffs image won't be created. Please run 'npm run build' inside the app folder to build the web app")
ENDIF()