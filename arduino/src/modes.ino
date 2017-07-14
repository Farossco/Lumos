#include "arduino.h"

unsigned long dick;

void initModes ()
{
	lastMode = MODE_DEFAULT;                  // Initialiazing last mode as well
	for (int i = MODE_FLASH; i < N_MODE; i++) // Starting from flash mode
		speed[i] = DEFAULT_SPEED[i];          // Initializing speed to its default value
}

// Perform mode action
void action ()
{
	if (mode == MODE_DEFAULT && lastMode != MODE_DEFAULT)
		lastMode = MODE_DEFAULT;

	// If lightning if off, don't do anything
	if (!on)
		return;

	// Calling modes functions
	switch (mode)
	{
		case MODE_FLASH:
			if (lastMode != MODE_FLASH) // If this is first call of the function, we call init function (lastMode will be set in init function)
				initModeFlash();
			modeFlash();
			break;

		case MODE_STROBE:
			if (lastMode != MODE_STROBE)
				initModeStrobe();
			modeStrobe();
			break;

		case MODE_FADE:
			if (lastMode != MODE_FADE)
				initModeFade();
			modeFade();
			break;

		case MODE_SMOOTH:
			if (lastMode != MODE_SMOOTH)
				initModeSmooth();
			modeSmooth();
			break;

		case MODE_WAKEUP:
			if (lastMode != MODE_WAKEUP)
				initModeWakeup();
			modeWakeup();
			break;
	}
} // action

// Flash mode initialization
void initModeFlash ()
{
	state           = 0;          // Set initial state to 0
	rgb[MODE_FLASH] = 0xFF0000;   // Set color to red
	count           = millis();   // Reseting milliseconds counter
	lastMode        = MODE_FLASH; // Setting lastMode so we don't call init again

	printlnNoPrefix();
	println ("Entering Flash mode");
}

// Flash mode
void modeFlash ()
{
	if (millis() - count < (unsigned int) (1000 / speed[MODE_FLASH]))
		return;

	if (state >= 2)
		state = 0;
	else
		state++;  // Incrementing state

	count = 0; // Reseting timer

	rgb[MODE_FLASH] = (state == 0 ? 0xFF0000 : state == 1 ? 0x00FF00 : 0x0000FF);

	count = millis();
}

// Strobe mode initialization
void initModeStrobe ()
{
	state            = 0;           // Set initial state to 0
	rgb[MODE_STROBE] = 0xFFFFFF;    // Set color to white
	count            = millis();    // Reseting milliseconds counter
	lastMode         = MODE_STROBE; // Setting lastMode so we don't call init again

	printlnNoPrefix();
	println ("Entering Strobe mode");
}

// Strobe mode
void modeStrobe ()
{
	if (millis() - count < (unsigned int) (1000 / speed[MODE_STROBE]))
		return;

	state = !state; // Inverting state

	rgb[MODE_STROBE] = state ? 0xFFFFFF : 0x000000; // Setting color to black then white then black then white...

	count = millis();
}

// Fade Mode initialization
void initModeFade ()
{
	state          = 1;         // Setting state to Increasing state
	rgb[MODE_FADE] = 0x000000;  // Setting color to white
	count          = millis();  // Reseting milliseconds counter
	lastMode       = MODE_FADE; // Setting lastMode so we don't call init again

	printlnNoPrefix();
	println ("Entering Fade mode");
}

// Fade Mode
void modeFade ()
{
	if (millis() - count < (unsigned int) (1000 / speed[MODE_FADE]))
		return;

	if (state)
		rgb[MODE_FADE] += 0x010101;  // Increasing all colors
	else
		rgb[MODE_FADE] -= 0x010101;  // Decreasing all colors

	if (rgb[MODE_FADE] >= 0xFFFFFF) // If color reach white, we start to decrease
		state = 0;                  // Decreasing state
	else if (rgb[MODE_FADE] <= 0)   // If color reach black, we start to increase
		state = 1;                  // Increasing state

	count = millis();
}

// Smooth Mode Initialization
void initModeSmooth ()
{
	state            = 0;        // Init state to 0
	rgb[MODE_SMOOTH] = 0xFF0000; // Init color to red
	count            = millis(); // Reseting milliseconds counter
	rgb2color();                 // Calling rgb2color to generate color values
	lastMode = MODE_SMOOTH;      // Setting lastMode so we don't call init again

	printlnNoPrefix();
	println ("Entering Smooth mode");
}

// Smooth Mode
void modeSmooth ()
{
	if (millis() - count < (unsigned int) (1000 / speed[MODE_SMOOTH]))
		return;

	// First, RED is max

	// Increasing GREEN until max
	if (state == 0)
	{
		if (rgb[MODE_SMOOTH] == 0xFFFF00)
			state = 1;
		else
			rgb[MODE_SMOOTH] += 0x000100;
	}
	// Decreasing RED until 0
	else if (state == 1)
	{
		if (rgb[MODE_SMOOTH] == 0x00FF00)
			state = 2;
		else
			rgb[MODE_SMOOTH] -= 0x010000;
	}
	// Increasing BLUE until max
	else if (state == 2)
	{
		if (rgb[MODE_SMOOTH] == 0x00FFFF)
			state = 3;
		else
			rgb[MODE_SMOOTH] += 0x000001;
	}
	// Decreasing GREEN until 0
	else if (state == 3)
	{
		if (rgb[MODE_SMOOTH] == 0x0000FF)
			state = 4;
		else
			rgb[MODE_SMOOTH] -= 0x000100;
	}
	// Increasing RED until max
	else if (state == 4)
	{
		if (rgb[MODE_SMOOTH] == 0xFF00FF)
			state = 5;
		else
			rgb[MODE_SMOOTH] += 0x010000;
	}
	// Decreasing BLUE until 0
	else if (state == 5)
	{
		if (rgb[MODE_SMOOTH] == 0xFF0000)
			state = 0;
		else
			rgb[MODE_SMOOTH] -= 0x000001;
	}
	// Then, we start over

	// Error handling
	else
		state = 0;

	count = millis();
} // modeSmooth

// Wakeup Mode initialization
void initModeWakeup ()
{
	rgb[MODE_WAKEUP]   = 0x0000FF;    // Setting color to blue
	power[MODE_WAKEUP] = MIN_POWER;   // Setting power to min value
	count              = millis();    // Reseting milliseconds counter
	lastMode           = MODE_WAKEUP; // Setting lastMode so we don't call init again

	printlnNoPrefix();
	println ("Entering Wakeup mode");
}

// Wakeup Mode
void modeWakeup ()
{
	if (millis() - count < (unsigned int) (10000 / speed[MODE_WAKEUP]))
		return;

	power[MODE_WAKEUP] += 1;        // Slowly increase power
	rgb[MODE_WAKEUP]    = 0x0000FF; // Setting color to blue

	if (power[MODE_WAKEUP] >= MAX_POWER) // When max blue is reached - maxSpeed[MODE_DEFAULT] = Max Power
	{
		rgb[MODE_DEFAULT]   = rgb[MODE_WAKEUP];   // Transfer RGB final value to default mode
		power[MODE_DEFAULT] = power[MODE_WAKEUP]; // Same for power
		mode                = MODE_DEFAULT;       // Leaving the mode
		println ("Leaving Wakeup mode");
	}

	count = millis();
}
