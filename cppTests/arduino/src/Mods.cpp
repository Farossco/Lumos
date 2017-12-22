#include "Mods.h"
#include "Global.h"
#include "Logger.h"

void Mods::init ()
{
	lastMode = MODE_DEFAULT;                  // Initialiazing last mod as well
	for (int i = MODE_FLASH; i < N_MODE; i++) // Starting from flash mod
		global.speed[i] = DEFAULT_SPEED[i];   // Initializing speed to its default value
}

// Perform mod action
void Mods::action ()
{
	if (global.mode == MODE_DEFAULT && lastMode != MODE_DEFAULT)
		lastMode = MODE_DEFAULT;

	// If lightning if off, don't do anything
	if (!global.on)
		return;

	switch (lastMode)
	{
		case MODE_FLASH:
			if (global.mode != MODE_FLASH)
			{
				Log.info ("Leaving Flash mode" dendl);
			}
			break;

		case MODE_STROBE:
			if (global.mode != MODE_STROBE)
			{
				Log.info ("Leaving Strobe mode" dendl);
			}
			break;

		case MODE_FADE:
			if (global.mode != MODE_FADE)
			{
				Log.info ("Leaving Fade mode" dendl);
			}
			break;

		case MODE_SMOOTH:
			if (global.mode != MODE_SMOOTH)
			{
				Log.info ("Leaving Smooth mode" dendl);
			}
			break;

		case MODE_DAWN:
			if (global.mode != MODE_DAWN)
			{
				Log.info ("Leaving Dawn mode" dendl);
			}
			break;
	}

	// Calling modes functions
	switch (global.mode)
	{
		case MODE_FLASH:
			if (lastMode != MODE_FLASH) // If this is first call of the function, we call init function (lastMode will be set in init function)
				initFlash();
			flash();
			break;

		case MODE_STROBE:
			if (lastMode != MODE_STROBE)
				initStrobe();
			strobe();
			break;

		case MODE_FADE:
			if (lastMode != MODE_FADE)
				initFade();
			fade();
			break;

		case MODE_SMOOTH:
			if (lastMode != MODE_SMOOTH)
				initSmooth();
			smooth();
			break;

		case MODE_DAWN:
			if (lastMode != MODE_DAWN)
				initDawn();
			dawn();
			break;
	}
} // action

// Flash mode initialization
void Mods::initFlash ()
{
	state                  = 0;          // Set initial state to 0
	global.rgb[MODE_FLASH] = 0xFF0000;   // Set color to red
	count                  = millis();   // Reseting milliseconds counter
	lastMode               = MODE_FLASH; // Setting lastMode so we don't call init again

	Log.info ("Entering Flash mode" dendl);
}

// Flash mode
void Mods::flash ()
{
	if (millis() - count < (unsigned int) (1000 / global.speed[MODE_FLASH]))
		return;

	if (state >= 2)
		state = 0;
	else
		state++;  // Incrementing state

	count = 0; // Reseting timer

	global.rgb[MODE_FLASH] = (state == 0 ? 0xFF0000 : state == 1 ? 0x00FF00 : 0x0000FF);

	count = millis();
}

// Strobe mode initialization
void Mods::initStrobe ()
{
	state                   = 0;           // Set initial state to 0
	global.rgb[MODE_STROBE] = 0xFFFFFF;    // Set color to white
	count                   = millis();    // Reseting milliseconds counter
	lastMode                = MODE_STROBE; // Setting lastMode so we don't call init again

	Log.info ("Entering Strobe mode" dendl);
}

// Strobe mode
void Mods::strobe ()
{
	if (millis() - count < (unsigned int) (1000 / global.speed[MODE_STROBE]))
		return;

	state = !state; // Inverting state

	global.rgb[MODE_STROBE] = state ? 0xFFFFFF : 0x000000; // Setting color to black then white then black then white...

	count = millis();
}

// Fade Mode initialization
void Mods::initFade ()
{
	state                 = 0;                                     // Setting state to Decreasing state
	global.rgb[MODE_FADE] = 0xFFFFFF / global.power[MODE_DEFAULT]; // Setting color to white
	count                 = millis();                              // Reseting milliseconds counter
	lastMode              = MODE_FADE;                             // Setting lastMode so we don't call init again

	Log.info ("Entering Fade mode" dendl);
}

// Fade Mode
void Mods::fade ()
{
	if (millis() - count < (unsigned int) (1000 / global.speed[MODE_FADE]))
		return;

	if (state)
		global.rgb[MODE_FADE] += 0x010101;  // Increasing all colors
	else
		global.rgb[MODE_FADE] -= 0x010101;  // Decreasing all colors

	if (global.rgb[MODE_FADE] >= 0xFFFFFF)      // If color reach white, we start to decrease
		state = 0;                              // Decreasing state
	else if (global.rgb[MODE_FADE] <= 0x000000) // If color reach black, we start to increase
		state = 1;                              // Increasing state

	count = millis();
}

// Smooth Mode Initialization
void Mods::initSmooth ()
{
	state                   = 0;        // Init state to 0
	global.rgb[MODE_SMOOTH] = 0xFF0000; // Init color to red
	count                   = millis(); // Reseting milliseconds counter
	global.rgb2color();                 // Calling rgb2color to generate color values
	lastMode = MODE_SMOOTH;             // Setting lastMode so we don't call init again

	Log.info ("Entering Smooth mode" dendl);
}

// Smooth Mode
void Mods::smooth ()
{
	if (millis() - count < (unsigned int) (1000 / global.speed[MODE_SMOOTH]))
		return;

	// First, RED is max

	// Increasing GREEN until max
	if (state == 0)
	{
		if (global.rgb[MODE_SMOOTH] == 0xFFFF00)
			state = 1;
		else
			global.rgb[MODE_SMOOTH] += 0x000100;
	}
	// Decreasing RED until 0
	else if (state == 1)
	{
		if (global.rgb[MODE_SMOOTH] == 0x00FF00)
			state = 2;
		else
			global.rgb[MODE_SMOOTH] -= 0x010000;
	}
	// Increasing BLUE until max
	else if (state == 2)
	{
		if (global.rgb[MODE_SMOOTH] == 0x00FFFF)
			state = 3;
		else
			global.rgb[MODE_SMOOTH] += 0x000001;
	}
	// Decreasing GREEN until 0
	else if (state == 3)
	{
		if (global.rgb[MODE_SMOOTH] == 0x0000FF)
			state = 4;
		else
			global.rgb[MODE_SMOOTH] -= 0x000100;
	}
	// Increasing RED until max
	else if (state == 4)
	{
		if (global.rgb[MODE_SMOOTH] == 0xFF00FF)
			state = 5;
		else
			global.rgb[MODE_SMOOTH] += 0x010000;
	}
	// Decreasing BLUE until 0
	else if (state == 5)
	{
		if (global.rgb[MODE_SMOOTH] == 0xFF0000)
			state = 0;
		else
			global.rgb[MODE_SMOOTH] -= 0x000001;
	}
	// Then, we start over

	// Error handling
	else
		state = 0;

	count = millis();
} // modeSmooth

// Wakeup Mode initialization
void Mods::initDawn ()
{
	global.rgb[MODE_DAWN]   = 0x0000FF;  // Setting color to blue
	global.power[MODE_DAWN] = MIN_POWER; // Setting power to min value
	count                   = millis();  // Reseting milliseconds counter
	lastMode                = MODE_DAWN; // Setting lastMode so we don't call init again

	Log.info ("Entering Dawn mode for %d min." dendl, global.speed[MODE_DAWN]);
}

// Wakeup Mode
void Mods::dawn ()
{
	// x60 to convert in seconds and x1000 to convert in milliseconds
	if (millis() - count < (((unsigned long) global.speed[MODE_DAWN]) * 60 * 1000) / MAX_POWER)
		return;

	global.power[MODE_DAWN] += 1;        // Slowly increase power
	global.rgb[MODE_DAWN]    = 0xFFFFFF; // Setting color to white

	if (global.power[MODE_DAWN] >= MAX_POWER) // When max blue is reached - maxSpeed[MODE_DEFAULT] = Max Power
	{
		global.rgb[MODE_DEFAULT]   = global.rgb[MODE_DAWN];   // Transfer RGB final value to default mode
		global.power[MODE_DEFAULT] = global.power[MODE_DAWN]; // Same for power
		global.mode                = MODE_DEFAULT;            // Leaving the mode

		Log.info ("Leaving Dawn mod" dendl);
	}

	count = millis();
}

Mods mods = Mods();