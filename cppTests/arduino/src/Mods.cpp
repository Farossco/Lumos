#include "Mods.h"
#include "Global.h"
#include "Logger.h"

void Mods::init ()
{
	lastMod = MOD_DEFAULT;                   // Initialiazing last mod as well
	for (int i = MOD_FLASH; i < N_MOD; i++)  // Starting from flash mod
		global.speed[i] = DEFAULT_SPEED[i];  // Initializing speed to its default value
}

// Perform mod action
void Mods::action ()
{
	if (global.mod == MOD_DEFAULT && lastMod != MOD_DEFAULT)
		lastMod = MOD_DEFAULT;

	// If lightning if off, don't do anything
	if (!global.on)
		return;

	switch (lastMod)
	{
		case MOD_FLASH:
			if (global.mod != MOD_FLASH)
			{
				Log.info ("Leaving Flash mod" dendl);
			}
			break;

		case MOD_STROBE:
			if (global.mod != MOD_STROBE)
			{
				Log.info ("Leaving Strobe mod" dendl);
			}
			break;

		case MOD_FADE:
			if (global.mod != MOD_FADE)
			{
				Log.info ("Leaving Fade mod" dendl);
			}
			break;

		case MOD_SMOOTH:
			if (global.mod != MOD_SMOOTH)
			{
				Log.info ("Leaving Smooth mod" dendl);
			}
			break;

		case MOD_DAWN:
			if (global.mod != MOD_DAWN)
			{
				Log.info ("Leaving Dawn mod" dendl);
			}
			break;
	}

	// Calling mods functions
	switch (global.mod)
	{
		case MOD_FLASH:
			if (lastMod != MOD_FLASH) // If this is first call of the function, we call init function (lastMod will be set in init function)
				initFlash();
			flash();
			break;

		case MOD_STROBE:
			if (lastMod != MOD_STROBE)
				initStrobe();
			strobe();
			break;

		case MOD_FADE:
			if (lastMod != MOD_FADE)
				initFade();
			fade();
			break;

		case MOD_SMOOTH:
			if (lastMod != MOD_SMOOTH)
				initSmooth();
			smooth();
			break;

		case MOD_DAWN:
			if (lastMod != MOD_DAWN)
				initDawn();
			dawn();
			break;
	}
} // action

// Flash mod initialization
void Mods::initFlash ()
{
	state                 = 0;         // Set initial state to 0
	global.rgb[MOD_FLASH] = 0xFF0000;  // Set color to red
	count                 = millis();  // Reseting milliseconds counter
	lastMod               = MOD_FLASH; // Setting lastMod so we don't call init again

	Log.info ("Entering Flash mod" dendl);
}

// Flash mod
void Mods::flash ()
{
	if (millis() - count < (unsigned int) (1000 / global.speed[MOD_FLASH]))
		return;

	if (state >= 2)
		state = 0;
	else
		state++;  // Incrementing state

	count = 0; // Reseting timer

	global.rgb[MOD_FLASH] = (state == 0 ? 0xFF0000 : state == 1 ? 0x00FF00 : 0x0000FF);

	count = millis();
}

// Strobe mod initialization
void Mods::initStrobe ()
{
	state                  = 0;          // Set initial state to 0
	global.rgb[MOD_STROBE] = 0xFFFFFF;   // Set color to white
	count                  = millis();   // Reseting milliseconds counter
	lastMod                = MOD_STROBE; // Setting lastMod so we don't call init again

	Log.info ("Entering Strobe mod" dendl);
}

// Strobe mod
void Mods::strobe ()
{
	if (millis() - count < (unsigned int) (1000 / global.speed[MOD_STROBE]))
		return;

	state = !state; // Inverting state

	global.rgb[MOD_STROBE] = state ? 0xFFFFFF : 0x000000; // Setting color to black then white then black then white...

	count = millis();
}

// Fade Mod initialization
void Mods::initFade ()
{
	state                = 0;                                    // Setting state to Decreasing state
	global.rgb[MOD_FADE] = 0xFFFFFF / global.power[MOD_DEFAULT]; // Setting color to white
	count                = millis();                             // Reseting milliseconds counter
	lastMod              = MOD_FADE;                             // Setting lastMod so we don't call init again

	Log.info ("Entering Fade mod" dendl);
}

// Fade Mod
void Mods::fade ()
{
	if (millis() - count < (unsigned int) (1000 / global.speed[MOD_FADE]))
		return;

	if (state)
		global.rgb[MOD_FADE] += 0x010101;  // Increasing all colors
	else
		global.rgb[MOD_FADE] -= 0x010101;  // Decreasing all colors

	if (global.rgb[MOD_FADE] >= 0xFFFFFF)      // If color reach white, we start to decrease
		state = 0;                             // Decreasing state
	else if (global.rgb[MOD_FADE] <= 0x000000) // If color reach black, we start to increase
		state = 1;                             // Increasing state

	count = millis();
}

// Smooth Mod Initialization
void Mods::initSmooth ()
{
	state                  = 0;        // Init state to 0
	global.rgb[MOD_SMOOTH] = 0xFF0000; // Init color to red
	count                  = millis(); // Reseting milliseconds counter
	global.rgb2color();                // Calling rgb2color to generate color values
	lastMod = MOD_SMOOTH;              // Setting lastMod so we don't call init again

	Log.info ("Entering Smooth mod" dendl);
}

// Smooth Mod
void Mods::smooth ()
{
	if (millis() - count < (unsigned int) (1000 / global.speed[MOD_SMOOTH]))
		return;

	// First, RED is max

	// Increasing GREEN until max
	if (state == 0)
	{
		if (global.rgb[MOD_SMOOTH] == 0xFFFF00)
			state = 1;
		else
			global.rgb[MOD_SMOOTH] += 0x000100;
	}
	// Decreasing RED until 0
	else if (state == 1)
	{
		if (global.rgb[MOD_SMOOTH] == 0x00FF00)
			state = 2;
		else
			global.rgb[MOD_SMOOTH] -= 0x010000;
	}
	// Increasing BLUE until max
	else if (state == 2)
	{
		if (global.rgb[MOD_SMOOTH] == 0x00FFFF)
			state = 3;
		else
			global.rgb[MOD_SMOOTH] += 0x000001;
	}
	// Decreasing GREEN until 0
	else if (state == 3)
	{
		if (global.rgb[MOD_SMOOTH] == 0x0000FF)
			state = 4;
		else
			global.rgb[MOD_SMOOTH] -= 0x000100;
	}
	// Increasing RED until max
	else if (state == 4)
	{
		if (global.rgb[MOD_SMOOTH] == 0xFF00FF)
			state = 5;
		else
			global.rgb[MOD_SMOOTH] += 0x010000;
	}
	// Decreasing BLUE until 0
	else if (state == 5)
	{
		if (global.rgb[MOD_SMOOTH] == 0xFF0000)
			state = 0;
		else
			global.rgb[MOD_SMOOTH] -= 0x000001;
	}
	// Then, we start over

	// Error handling
	else
		state = 0;

	count = millis();
} // Mods::smooth

// Dawn Mod initialization
void Mods::initDawn ()
{
	global.rgb[MOD_DAWN]   = 0x0000FF;  // Setting color to blue
	global.power[MOD_DAWN] = MIN_POWER; // Setting power to min value
	count                  = millis();  // Reseting milliseconds counter
	lastMod                = MOD_DAWN;  // Setting lastMod so we don't call init again

	Log.info ("Entering Dawn mod for %d min." dendl, global.speed[MOD_DAWN]);
}

// Dawn Mod
void Mods::dawn ()
{
	// x60 to convert in seconds and x1000 to convert in milliseconds
	if (millis() - count < (((unsigned long) global.speed[MOD_DAWN]) * 60 * 1000) / MAX_POWER)
		return;

	global.power[MOD_DAWN] += 1;        // Slowly increase power
	global.rgb[MOD_DAWN]    = 0xFFFFFF; // Setting color to white

	if (global.power[MOD_DAWN] >= MAX_POWER) // When max blue is reached - maxSpeed[MOD_DEFAULT] = Max Power
	{
		global.rgb[MOD_DEFAULT]   = global.rgb[MOD_DAWN];   // Transfer RGB final value to default mod
		global.power[MOD_DEFAULT] = global.power[MOD_DAWN]; // Same for power
		global.mod                = MOD_DEFAULT;            // Leaving the mod

		Log.info ("Leaving Dawn mod" dendl);
	}

	count = millis();
}

Mods mods = Mods();
