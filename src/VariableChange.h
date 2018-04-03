#ifndef VARIABLE_CHANGE_H
#define VARIABLE_CHANGE_H

#include <Arduino.h>
#include "Light.h"

class VariableChange
{
public:
	VariableChange();
	void init ();
	void check ();
	void sendInfo ();

private:
	boolean changeOn;
	unsigned char changePower[LIGHT_N_MOD];
	unsigned int changeSpeed[LIGHT_N_MOD];
	unsigned long changeRgb;
	unsigned char changeLightMod;
	unsigned char changeSoundMod;
};

extern VariableChange variableChange;

#endif // ifndef VARIABLE_CHANGE_H
