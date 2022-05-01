#ifndef VARIABLE_CHANGE_H
#define VARIABLE_CHANGE_H

#include <Arduino.h>
#include "Light.h"

class VariableChange {
public:
	VariableChange();
	void init();
	void check();
	void sendData();

private:
	LightOnOff changeLightOn;
	LightPowerArray changeLightPowers;
	LightSpeedArray changeLightSpeeds;
	LightRgbArray changeRgbs;
	LightMode changeLightMode;
	SoundMode changeSoundMode;
	SoundVolume changeSoundVolume;
	SoundOnOff changeSoundOn;
	Time changeDawnTime;
	Timing changeDawnDuration;
	Timing changeSunsetDuration;
	Timing changeSunsetDecreaseTime;
	SoundVolume changeDawnVolume;

	bool initialized;
};

extern VariableChange variableChange;

#endif // ifndef VARIABLE_CHANGE_H
