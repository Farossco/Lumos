################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
../BedControl_ESP8266.ino 

CPP_SRCS += \
../.ino.cpp 

LINK_OBJ += \
./.ino.cpp.o 

INO_DEPS += \
.\BedControl_ESP8266.ino.d 

CPP_DEPS += \
.\.ino.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
.ino.cpp.o: ../.ino.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"D:\Program Files\Eclipse\Cpp-Neon\eclipse\/arduinoPlugin/packages/esp8266/tools/xtensa-lx106-elf-gcc/1.20.0-26-gb404fb9-2/bin/xtensa-lx106-elf-g++" -D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ "-ID:\Program Files\Eclipse\Cpp-Neon\eclipse\/arduinoPlugin/packages/esp8266/hardware/esp8266/2.3.0/tools/sdk/include" "-ID:\Program Files\Eclipse\Cpp-Neon\eclipse\/arduinoPlugin/packages/esp8266/hardware/esp8266/2.3.0/tools/sdk/lwip/include" "-ID:/Fichiers/Documents/GitHub/BedControl/esp8266/Release/core" -c -Wall -Wextra -Os -g -mlongcalls -mtext-section-literals -fno-exceptions -fno-rtti -falign-functions=4 -std=c++11 -ffunction-sections -fdata-sections -DF_CPU=80000000L -DLWIP_OPEN_SRC  -DARDUINO=10609 -DARDUINO_ESP8266_ESP01 -DARDUINO_ARCH_ESP8266 "-DARDUINO_BOARD=\"ESP8266_ESP01\"" -DESP8266  -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\cores\esp8266" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\variants\generic" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\ESP8266WiFi" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\ESP8266WiFi\src" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\ArduinoOTA" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\ESP8266mDNS" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\TimeESP" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '

BedControl_ESP8266.o: ../BedControl_ESP8266.ino
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"D:\Program Files\Eclipse\Cpp-Neon\eclipse\/arduinoPlugin/packages/esp8266/tools/xtensa-lx106-elf-gcc/1.20.0-26-gb404fb9-2/bin/xtensa-lx106-elf-g++" -D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ "-ID:\Program Files\Eclipse\Cpp-Neon\eclipse\/arduinoPlugin/packages/esp8266/hardware/esp8266/2.3.0/tools/sdk/include" "-ID:\Program Files\Eclipse\Cpp-Neon\eclipse\/arduinoPlugin/packages/esp8266/hardware/esp8266/2.3.0/tools/sdk/lwip/include" "-ID:/Fichiers/Documents/GitHub/BedControl/esp8266/Release/core" -c -Wall -Wextra -Os -g -mlongcalls -mtext-section-literals -fno-exceptions -fno-rtti -falign-functions=4 -std=c++11 -ffunction-sections -fdata-sections -DF_CPU=80000000L -DLWIP_OPEN_SRC  -DARDUINO=10609 -DARDUINO_ESP8266_ESP01 -DARDUINO_ARCH_ESP8266 "-DARDUINO_BOARD=\"ESP8266_ESP01\"" -DESP8266  -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\cores\esp8266" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\variants\generic" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\ESP8266WiFi" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\ESP8266WiFi\src" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\ArduinoOTA" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\ESP8266mDNS" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\TimeESP" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '


