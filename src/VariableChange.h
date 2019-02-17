#ifndef VARIABLE_CHANGE_H
#define VARIABLE_CHANGE_H

#if defined(__AVR_ATmega2560__)

# include <Arduino.h>
# include "Light.h"

class VariableChange
{
public:
	VariableChange();
	void init ();
	void check ();
	void sendInfo ();

private:
	bool changeOn;
	uint8_t changePower[LIGHT_N_MOD];
	uint16_t changeSpeed[LIGHT_N_MOD];
	uint32_t changeRgb;
	uint8_t changeLightMod;
	uint8_t changeSoundMod;
	uint16_t changeDawnTime;
};

extern VariableChange variableChange;

#endif // if defined(__AVR_ATmega2560__)

#endif // ifndef VARIABLE_CHANGE_H
