#include <stdlib.h>
#include <Time.h>
#include "Global.h"
#include "Logger.h"
#include "Memory.h"

void Global::init ()
{
	if (memory.read()) // Returns True if EEPROM is not correctly initialized (This may be the first launch)
	{
		Log.info ("This is first launch, variables will be initialized to their default values" dendl);

		for (int i = MODE_MIN; i < N_MODE; i++)
		{
			rgb[i]   = DEFAULT_RGB[i];   // Initialize colors to their default value
			power[i] = DEFAULT_POWER[i]; // Initializing powers their default value
			speed[i] = DEFAULT_SPEED[i]; // Initializing speeds their default value
		}

		memory.write();
	}
	else
	{
		for (int i = MODE_FLASH; i < N_MODE; i++)
		{
			rgb[i] = DEFAULT_RGB[i]; // Initialize colors to their default values (Starting from flash mode)
		}
	}
}

// Display the RGB value
void Global::light ()
{
	rgb2color(); // Convert RGB value to Red, Green and Blue values

	analogWrite (PIN_LED_RED, on ? red[mode] : 0);
	analogWrite (PIN_LED_GREEN, on ? green[mode] : 0);
	analogWrite (PIN_LED_BLUE, on ? blue[mode] : 0);
}

// Convert RGB value to Red, Green and Blue values
void Global::rgb2color ()
{
	for (int i = MODE_MIN; i < N_MODE; i++)
	{
		// Lower is the power, lower is the color value - It allows you to control LEDs light power
		red[i]   = ((rgb[i] & 0xFF0000) >> 16) * ((float) power[i] / MAX_POWER);
		green[i] = ((rgb[i] & 0x00FF00) >> 8) * ((float) power[i] / MAX_POWER);
		blue[i]  = (rgb[i] & 0x0000FF) * ((float) power[i] / MAX_POWER);
	}
}

Global global = Global();
