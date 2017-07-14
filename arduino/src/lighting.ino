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
	for (int i = 0; i <= MODE_MAX; i++)
	{
		// Lower is the power, lower is the color value - It allows you to control LEDs light power
		red[i]   = ((rgb[i] & 0xFF0000) >> 16) * (power[i] / maxSpeed[MODE_DEFAULT]); // maxSpeed[MODE_DEFAULT] = Max Power
		green[i] = ((rgb[i] & 0x00FF00) >> 8) * (power[i] / maxSpeed[MODE_DEFAULT]);
		blue[i]  = (rgb[i] & 0x0000FF) * (power[i] / maxSpeed[MODE_DEFAULT]);
	}
}
