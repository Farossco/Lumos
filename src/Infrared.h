#ifndef INFRARED_H
#define INFRARED_H

#if defined(LUMOS_ARDUINO_MEGA)

# include <IRremote.h>
# include "Resources.h"

const int IR_CHANGE_STEP = 10; // increasion or decreasion step value for infrared
const int MIN_IR_POWER   = 5;  // Minimum value in wich the power can go with infrared
const int IR_DEFAULT_PIN = 8;  // Default infrared pin

class Infrared
{
public:
	Infrared();
	void init (int pin = IR_DEFAULT_PIN);
	void read ();

private:
	IRrecv irrecv;
	decode_results results;
	boolean enabled;
	unsigned long lastIRCode; // IR code in previous loop - Allows continious power / strobe speed increasion / dicreasion
	unsigned long IRCode;     // Current IR code
};

extern Infrared infrared;

#endif // if defined(LUMOS_ARDUINO_MEGA)

#endif // ifndef INFRARED_H