#ifndef ARDUINO_SERIAL_H
#define ARDUINO_SERIAL_H

#include <Arduino.h>

void serial_com_init(uint32_t debugBaudRate, uint32_t comBaudRate);
void serial_com_receive_and_decode(void);

#endif // ifndef ARDUINO_SERIAL_H
