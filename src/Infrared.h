#ifndef INFRARED_H
#define INFRARED_H

#if defined(LUMOS_ARDUINO_MEGA)

# include <IRremote.h>

const int N_COLOR        = 16; // Number of different colors
const int IR_CHANGE_STEP = 5;  // increasion or decreasion step value for infrared
const int MIN_IR_POWER   = 5;  // Minimum value in wich the power can go with infrared
const int IR_DEFAULT_PIN = 8;  // Default infrared pin

// ******* Infrared codes and corresponding RGB code array ******* //
const unsigned long color[N_COLOR][3] =
{
	// RGB Code   Code 1     Code 2
	{ 0xFFFFFF, 0xFFA857, 0xA3C8EDDB }, // WHITE
	{ 0xFF0000, 0xFF9867, 0x97483BFB }, // R1
	{ 0xFF5300, 0xFFE817, 0x5BE75E7F }, // R2
	{ 0xFF3C00, 0xFF02FD, 0xD7E84B1B }, // R3
	{ 0xFFC400, 0xFF50AF, 0x2A89195F }, // R4
	{ 0xFFFF00, 0xFF38C7, 0x488F3CBB }, // R5

	{ 0x00FF00, 0xFFD827, 0x86B0E697 }, // G1
	{ 0x20FF5D, 0xFF48B7, 0xF377C5B7 }, // G2
	{ 0x34FFF6, 0xFF32CD, 0xEE4ECCFB }, // G3
	{ 0x21E7FF, 0xFF7887, 0xF63C8657 }, // G4
	{ 0x00BDFF, 0xFF28D7, 0x13549BDF }, // G5

	{ 0x0000FF, 0xFF8877, 0x9EF4941F }, // B1
	{ 0x0068FF, 0xFF6897, 0xC101E57B }, // B2
	{ 0x8068FF, 0xFF20DF, 0x51E43D1B }, // B3
	{ 0xDB89FF, 0xFF708F, 0x44C407DB }, // B4
	{ 0xFF7B92, 0xFFF00F, 0x35A9425F } // B5
};


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