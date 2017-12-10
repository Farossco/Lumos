#ifndef VARIABLE_CHANGE_H
#define VARIABLE_CHANGE_H

#include <Arduino.h>
#include "Utils.h"

class VariableChange
{
public:
	VariableChange();
	void begin ();
	void check ();
	void sendInfo ();

private:
	char buf[(N_MODE * 8) + 26];
	boolean changeOn;
	unsigned long changeRgb;
	int changePower[N_MODE];
	int changeSpeed[N_MODE];
	unsigned char changeMode;
};

extern VariableChange variableChange;

#endif // ifndef VARIABLE_CHANGE_H
