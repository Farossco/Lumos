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
