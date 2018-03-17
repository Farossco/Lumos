#include "arduino.h"

// Display the RGB value
void light ()
{
	rgb2color(); // Convert RGB value to Red, Green and Blue values

	analogWrite (PIN_LED_RED, on ? red[mode] : 0);
	analogWrite (PIN_LED_GREEN, on ? green[mode] : 0);
	analogWrite (PIN_LED_BLUE, on ? blue[mode] : 0);
}

// Convert RGB value to Red, Green and Blue values
void rgb2color ()
{
	for (int i = MODE_MIN; i < N_MODE; i++)
	{
		// Lower is the power, lower is the color value - It allows you to control LEDs light power
		red[i]   = ((rgb[i] & 0xFF0000) >> 16) * ((float) power[i] / MAX_POWER);
		green[i] = ((rgb[i] & 0x00FF00) >> 8) * ((float) power[i] / MAX_POWER);
		blue[i]  = (rgb[i] & 0x0000FF) * ((float) power[i] / MAX_POWER);
	}
}
