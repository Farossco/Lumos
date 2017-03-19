int state; // Current state used by some modes
int count; // Delay counting

/******* action *******/
int lastMode;          // Mode in previous loop - Allows mode initializations
unsigned long lastRgb; // Saved RGB value before we leave default mode
float lastPower;       // Saved power value before we leave default mode

void initModes ()
{
	lastMode    = MODE_DEFAULT;         // Initialiazing last mode as well
	flashSpeed  = DEFAULT_FLASH_SPEED;  // Initializing flash speed to its default value
	strobeSpeed = DEFAULT_STROBE_SPEED; // Initializing strobe speed to its default value
	fadeSpeed   = DEFAULT_FADE_SPEED;   // Initializing fade speed to its default value
	smoothSpeed = DEFAULT_SMOOTH_SPEED; // Initializing smooth speed to its default value
}

// Perform mode action
void action ()
{
	if (mode == MODE_DEFAULT && lastMode != MODE_DEFAULT)
	{
		lastMode = MODE_DEFAULT;
		rgb      = lastRgb;
		power    = lastPower;
	}

	// If lightning if off or mode is constant lightning, don't do anything
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
	if (lastMode == MODE_DEFAULT)
	{
		lastRgb   = rgb;
		lastPower = power;
	}
	state    = 0;          // Set initial state to 0
	rgb      = 0xFF0000;   // Set color to red
	count    = 0;          // Reseting counter
	lastMode = MODE_FLASH; // Setting lastMode so we don't call init again

	println ("Entering Flash mode\n");
}

// Flash mode
void modeFlash ()
{
	delay (1);

	if (count < 100 - flashSpeed)
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

	rgb = (state == 0 ? 0xFF0000 : state == 1 ? 0x00FF00 : 0x0000FF);
}

// Strobe mode initialization
void initModeStrobe ()
{
	if (lastMode == MODE_DEFAULT)
	{
		lastRgb   = rgb;
		lastPower = power;
	}
	state    = 0;           // Set initial state to 0
	rgb      = 0xFFFFFF;    // Set color to white
	count    = 0;           // Reseting counter
	lastMode = MODE_STROBE; // Setting lastMode so we don't call init again

	println ("Entering Strobe mode\n");
}

// Strobe mode
void modeStrobe ()
{
	delay (1);

	if (count < 100 - strobeSpeed)
	{
		count++;
		return;
	}

	count = 0;

	state = !state; // Inverting state

	rgb = state ? 0xFFFFFF : 0x000000; // Setting color to black then white then black then white...
}

// Fade Mode initialization
void initModeFade ()
{
	if (lastMode == MODE_DEFAULT)
	{
		lastRgb   = rgb;
		lastPower = power;
	}
	state    = 1;        // Setting state to Increasing state
	rgb      = 0xFFFFFF; // Setting color to white
	power    = 0;        // Setting power to 0 (LED's shutted down)
	count    = 0;
	lastMode = MODE_FADE; // Setting lastMode so we don't call init again

	println ("Entering Fade mode\n");
}

// Fade Mode
void modeFade ()
{
	delay (1);

	if (count < 100 - fadeSpeed)
	{
		count++;
		return;
	}

	count = 0;

	if (state)
	{
		power++; // Increasing power
	}
	else
	{
		power--; // Decreasing power
	}

	if (power >= MAX_POWER) // If power reach MAX_POWER, we start to decrease
	{
		state = 0; // Decreasing state
	}
	else if (power <= 0) // If power reach 0, we start to increase
	{
		state = 1; // Increasing state
	}
}

// Smooth Mode Initialization
void initModeSmooth ()
{
	if (lastMode == MODE_DEFAULT)
	{
		lastRgb   = rgb;
		lastPower = power;
	}
	state = 0;        // Init state to 0
	rgb   = 0xFF0000; // Init color to red
	count = 0;
	rgb2color();            // Calling rgb2color to generate color values
	lastMode = MODE_SMOOTH; // Setting lastMode so we don't call init again

	println ("Entering Smooth mode\n");
}

// Smooth Mode
void modeSmooth ()
{
	delay (1);

	if (count < 100 - smoothSpeed)
	{
		count++;
		return;
	}

	count = 0;

	// First, RED is max

	// Increasing GREEN until max
	if (state == 0)
	{
		if (rgb == 0xFFFF00)
			state = 1;
		else
			rgb += 0x000100;
	}
	// Decreasing RED until 0
	else if (state == 1)
	{
		if (rgb == 0x00FF00)
			state = 2;
		else
			rgb -= 0x010000;
	}
	// Increasing BLUE until max
	else if (state == 2)
	{
		if (rgb == 0x00FFFF)
			state = 3;
		else
			rgb += 0x000001;
	}
	// Decreasing GREEN until 0
	else if (state == 3)
	{
		if (rgb == 0x0000FF)
			state = 4;
		else
			rgb -= 0x000100;
	}
	// Increasing RED until max
	else if (state == 4)
	{
		if (rgb == 0xFF00FF)
			state = 5;
		else
			rgb += 0x010000;
	}
	// Decreasing BLUE until 0
	else if (state == 5)
	{
		if (rgb == 0xFF0000)
			state = 0;
		else
			rgb -= 0x000001;
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
	if (lastMode == MODE_DEFAULT)
	{
		lastRgb   = rgb;
		lastPower = power;
	}
	rgb      = 0x0000FF; // Setting color to blue
	power    = 0;        // Setting power to 0
	count    = 0;
	lastMode = MODE_WAKEUP; // Setting lastMode so we don't call init again

	println ("Entering Wakeup mode\n");
}

// Wakeup Mode
void modeWakeup ()
{
	delay (1);

	if (count < WAKE_UP_SLOWNESS)
	{
		count++;
		return;
	}

	count = 0;

	power++; // Slowly increase power

	if (power >= MAX_POWER) // When max power is reached
	{
		lastRgb   = rgb;          // RGB value when we go back to default mode so don't jump change to a different color
		lastPower = power;        // Same for power
		mode      = MODE_DEFAULT; // Leaving the mode
		println ("Leaving Wakeup mode\n");
	}
}
