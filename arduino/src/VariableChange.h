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
	int changePower[N_MOD];
	int changeSpeed[N_MOD];
	unsigned long changeRgb;
	unsigned char changeMod;
};

extern VariableChange variableChange;

#endif // ifndef VARIABLE_CHANGE_H
