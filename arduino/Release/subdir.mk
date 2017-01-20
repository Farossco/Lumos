################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
../BedControl.ino 

CPP_SRCS += \
../.ino.cpp 

LINK_OBJ += \
./.ino.cpp.o 

INO_DEPS += \
.\BedControl.ino.d 

CPP_DEPS += \
.\.ino.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
.ino.cpp.o: ../.ino.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"D:\Program Files\Eclipse\Cpp-Neon\eclipse\/arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.3-arduino2/bin/avr-g++" -c -g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10609 -DARDUINO_AVR_NANO -DARDUINO_ARCH_AVR   -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\arduino\hardware\avr\1.6.17\cores\arduino" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\arduino\hardware\avr\1.6.17\variants\eightanaloginputs" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\libraries\Time\1.5.0" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\libraries\IRremote\2.0.1" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\libraries\NTPClient\3.1.0" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '

BedControl.o: ../BedControl.ino
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"D:\Program Files\Eclipse\Cpp-Neon\eclipse\/arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.3-arduino2/bin/avr-g++" -c -g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10609 -DARDUINO_AVR_NANO -DARDUINO_ARCH_AVR   -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\arduino\hardware\avr\1.6.17\cores\arduino" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\packages\arduino\hardware\avr\1.6.17\variants\eightanaloginputs" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\libraries\Time\1.5.0" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\libraries\IRremote\2.0.1" -I"D:\Program Files\Eclipse\Cpp-Neon\eclipse\arduinoPlugin\libraries\NTPClient\3.1.0" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '


