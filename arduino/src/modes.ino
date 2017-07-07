int state; // Current state used by some modes
int count; // Delay counting

/******* action *******/
int lastMode; // Mode in previous loop - Allows mode initializations

void initModes ()
{
	lastMode = MODE_DEFAULT; // Initialiazing last mode as well
	for (int i = 1; i <= MODE_MAX; i++)
		speed[i] = defaultSpeed[i];  // Initializing speed to its default value
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
	state = 0;                  // Set initial state to 0
	rgb[MODE_FLASH] = 0xFF0000; // Set color to red
	count    = 0;               // Reseting counter
	lastMode = MODE_FLASH;      // Setting lastMode so we don't call init again

	println ("Entering Flash mode");
}

// Flash mode
void modeFlash ()
{
	delay (1);

	if (count < 100 - speed[MODE_FLASH])
	{
		count++;
		return;
	}

	count = 0;

	if (state >= 2)
		state = 0;
	else
		state++;  // Incrementing state

	count = 0; // Reseting timer

	rgb[MODE_FLASH] = (state == 0 ? 0xFF0000 : state == 1 ? 0x00FF00 : 0x0000FF);
}

// Strobe mode initialization
void initModeStrobe ()
{
	state = 0;                   // Set initial state to 0
	rgb[MODE_STROBE] = 0xFFFFFF; // Set color to white
	count    = 0;                // Reseting counter
	lastMode = MODE_STROBE;      // Setting lastMode so we don't call init again

	println ("Entering Strobe mode");
}

// Strobe mode
void modeStrobe ()
{
	delay (1);

	if (count < 100 - speed[MODE_STROBE])
	{
		count++;
		return;
	}

	count = 0;

	state = !state; // Inverting state

	rgb[MODE_STROBE] = state ? 0xFFFFFF : 0x000000; // Setting color to black then white then black then white...
}

// Fade Mode initialization
void initModeFade ()
{
	state = 1;                 // Setting state to Increasing state
	rgb[MODE_FADE] = 0x000000; // Setting color to white
	count    = 0;
	lastMode = MODE_FADE; // Setting lastMode so we don't call init again

	println ("Entering Fade mode");
}

// Fade Mode
void modeFade ()
{
	delay (1);

	if (count < 100 - speed[MODE_FADE])
	{
		count++;
		return;
	}

	count = 0;

	if (state)
		rgb[MODE_FADE] += 0x010101;  // Increasing all colors
	else
		rgb[MODE_FADE] -= 0x010101;  // Decreasing all colors

	if (rgb[MODE_FADE] >= 0xFFFFFF) // If color reach white, we start to decrease
		state = 0;                  // Decreasing state
	else if (rgb[MODE_FADE] <= 0)   // If color reach black, we start to increase
		state = 1;                  // Increasing state
}

// Smooth Mode Initialization
void initModeSmooth ()
{
	state = 0;                   // Init state to 0
	rgb[MODE_SMOOTH] = 0xFF0000; // Init color to red
	count = 0;
	rgb2color();            // Calling rgb2color to generate color values
	lastMode = MODE_SMOOTH; // Setting lastMode so we don't call init again

	println ("Entering Smooth mode");
}

// Smooth Mode
void modeSmooth ()
{
	delay (1);

	if (count < 100 - speed[MODE_SMOOTH])
	{
		count++;
		return;
	}

	count = 0;

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
	{
		state = 0;
	}
} // modeSmooth

// Wakeup Mode initialization
void initModeWakeup ()
{
	rgb[MODE_WAKEUP] = 0x000000; // Setting color to black
	count    = 0;
	lastMode = MODE_WAKEUP; // Setting lastMode so we don't call init again

	println ("Entering Wakeup mode");
}

// Wakeup Mode
void modeWakeup ()
{
	delay (1);

	if (count < speed[MODE_WAKEUP])
	{
		count++;
		return;
	}

	count = 0;

	rgb[MODE_WAKEUP] += 0x000001; // Slowly increase blue

	if (rgb[MODE_WAKEUP] >= 0x0000FF) // When max blue is reached
	{
		rgb[MODE_DEFAULT]   = rgb[MODE_WAKEUP];   // Transfer RGB final value to default mode
		power[MODE_DEFAULT] = power[MODE_WAKEUP]; // Same for power
		mode = MODE_DEFAULT;                      // Leaving the mode
		println ("Leaving Wakeup mode");
	}
}
