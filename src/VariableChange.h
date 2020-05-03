#ifndef VARIABLE_CHANGE_H
#define VARIABLE_CHANGE_H

#if defined(LUMOS_ARDUINO_MEGA)

# include <Arduino.h>
# include "Light.h"

class VariableChange
{
public:
	VariableChange();
	void init ();
	void check ();
	void sendData ();

private:
	LightOnOff changeLightOn;
	LightPowerArray changeLightPowers;
	LightSpeedArray changeLightSpeeds;
	LightRgbArray changeRgbs;
	LightMode changeLightMode;
	SoundMode changeSoundMode;
	SoundVolume changeSoundVolume;
	SoundOnOff changeSoundOn;
	uint16_t changeDawnTime;

	bool initialized;
};

extern VariableChange variableChange;

#endif // if defined(LUMOS_ARDUINO_MEGA)

#endif // ifndef VARIABLE_CHANGE_H