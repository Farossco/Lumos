#ifndef VARIABLE_CHANGE_H
#define VARIABLE_CHANGE_H

#include <Arduino.h>
#include "Utils.h"

class VariableChange
{
public:
	VariableChange();
	void init ();
	void check ();
	void sendInfo ();

private:
	boolean changeOn;
	unsigned long changeRgb;
	int changePower[N_MODE];
	int changeSpeed[N_MODE];
	unsigned char changeMode;
};

extern VariableChange variableChange;

#endif // ifndef VARIABLE_CHANGE_H
