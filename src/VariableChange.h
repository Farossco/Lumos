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
	void sendInfo ();

private:
	bool changeLightOn;
	LightSetting changeLightPower;
	LightSetting changeLightSpeed;
	LightSetting changeRed;
	LightSetting changeGreen;
	LightSetting changeBlue;
	LightMode changeLightMode;
	SoundMode changeSoundMode;
	SoundSetting changeSoundVolume;
	bool changeSoundOn;
	uint16_t changeDawnTime;
	bool initialized;
};

extern VariableChange variableChange;

#endif // if defined(LUMOS_ARDUINO_MEGA)

#endif // ifndef VARIABLE_CHANGE_H