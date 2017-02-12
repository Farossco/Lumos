#include <IRremote.h>
#include <TimeLib.h>

#define BAUD_RATE 250000 // Serial baud rate

#define DEBUG_ENABLED true // DEBUG Mode
#define WAIT_FOR_TIME true // If we have to wait for time sync (if true, program will not start until time is synced)

// Time wake up
#define WAKEUP_HOUR 17
#define WAKEUP_MINUTE 07

#define LED_RED 9 // Red LED OUT pin
#define LED_GREEN 10 // Green LED OUT pin
#define LED_BLUE 11 // Blue LED OUT pin
#define LED_INFRARED 12 // Infrared IN pin

#define MIN_POWER 5 // Minimum power value
#define MAX_POWER 100 // Maximum power value
#define DEFAULT_POWER 30 // Default power value
#define POWER_SPEED 5 // Power increasion or dicreasion speed
#define FADE_SPEED 1 // Fade increasion or dicreasion speed
#define WAKEUP_SPEED 0.01 // Power increasion speed for Wakeup mode
#define N 16 // Number of different colors
#define MIN_STROBE 2 // Minimum strobe speed
#define MAX_STROBE 50 // Maximum strobe speed
#define STROBE_SPEED 1 // Strobe increasion or dicreasion speed
#define DEFAULT_STROBE_SPEED 25 // Default strobe speed value
#define MIN_FLASH 2 // Minimum strobe speed
#define MAX_FLASH 50 // Maximum flash speed
#define FLASH_SPEED 1 // Flash increasion or dicreasion speed
#define DEFAULT_FLASH_SPEED 25 // Default flash speed value

//Some IDs used for serial reception decrypt
#define TYPE_UNKNOWN -1
#define TYPE_TIME 0
#define TYPE_RGB 1
#define TYPE_ON 2
#define TYPE_POW 3
#define TYPE_MOD 4

// Some IDs used for Mode changing
#define MODE_DEFAULT 0
#define MODE_FLASH 1
#define MODE_STROBE 2
#define MODE_FADE 3
#define MODE_SMOOTH 4
#define MODE_WAKEUP 5

// Infrared reception objects declaration
IRrecv irrecv (LED_INFRARED);
decode_results results;

// Infrared codes and corresponding RGB code array
unsigned long color[][3] =
{
//        RGB Code   Code 1     Code 2
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
		{ 0xFF7B92, 0xFFF00F, 0x35A9425F }  // B5
};

//******* Useful *******//
int i, n; // Counting variables (used for "for" statements)

//******* Global *******//
boolean on; // If the leds are ON or OFF (True: ON / False: OFF)
unsigned long rgb; // Currently displayed RGB value (From 0x000000 to 0xFFFFFF)
int red; // Currentlty red value including lightning power (From 0 to 255)
int green; // Currentlty green value including lightning power (From 0 to 255)
int blue; // Currentlty blue value including lightning power (From 0 to 255)
int mode; // Current lighting mode (0: Constant lightning / 1: Flash / 2: Strobe / 3: Fade / 4: Smooth / 5: Wakeup)
float power; // Current lightning power (from MINPOWER to MAXPOWER)

//******* testWakeUpTime *******//
boolean wokeUp; // Used to set Wakeup mode only once
boolean timeAsked; // Used to ask time only once every peak time

//******* Modes *******//
int state; // Current state used by 1 to 4 modes
int count; // Delay counting
/******* action *******/
int lastMode; // Mode in previous loop - Allows mode initializations
/******* modeFlash ********/
int flashSpeed;
/******* modeStrobe *******/
int strobeSpeed; // Current Strobe speed (From MINSTROBE to MAXSTROBE)
/******* modeSmooth *******/
int unpoweredRed; // Currentlty red value without lightning power (From 0 to 255)
int unpoweredGreen; // Currentlty green value without lightning power (From 0 to 255)
int unpoweredBlue; // Currentlty blue value without lightning power (From 0 to 255)

//******* readInfrared *******//
unsigned long lastIRCode; // IR code in previous loop - Allows continious power / strobe speed increasion / dicreasion
unsigned long IRCode; // Current IR code

//******* readSerial *******//
int infoType; // Information type ("Time", "ON", "RGB", "Power" or "Mode")
int messageLength; // Message length
char charRgb[7], charPow[4], charTime[11]; // Char arrays for message decrypting with strtol function
char messageChar[20]; // Received message
String message; // Received message converted to String

void setup ()
{
	Serial.begin (BAUD_RATE); // Initialize serial communication

	if (DEBUG_ENABLED)
		Serial.println ("Starting program\n");

	// pins initialization
	pinMode (LED_RED, OUTPUT);
	pinMode (LED_GREEN, OUTPUT);
	pinMode (LED_BLUE, OUTPUT);
	pinMode (LED_INFRARED, INPUT);
	pinMode (LED_BUILTIN, OUTPUT);
	digitalWrite (LED_BUILTIN, LOW);

	on = false; // LEDs are off on startup
	irrecv.enableIRIn (); // Initialize IR communication
	rgb = 0xFFFFFF; // Initialize color to white
	mode = MODE_DEFAULT; // Initialize mode to constant lightning
	lastMode = MODE_DEFAULT; // Initialiazing last mode as well
	power = DEFAULT_POWER; // Initializing power its default value
	strobeSpeed = DEFAULT_STROBE_SPEED; // Initializing strobe speed its default value
	flashSpeed = DEFAULT_FLASH_SPEED; // Initializing flash speed its default value

	lastIRCode = 0;
	wokeUp = false;
	timeAsked = false;

	// Gently ask for time
	i = 5000;
	while (timeStatus () == timeNotSet && WAIT_FOR_TIME) // Doesn't start if time isn't set (only if WAIT_FOR_TIME = 1)
	{
		if (i >= 5000)
		{
			askForTime ();
			i = 0;
		}

		readSerial ();
		i++;
		delay (1);
	}

	if (DEBUG_ENABLED)
		Serial.println ("Program started\n");
}

void loop ()
{
	testWakeUpTime (); // Test wakeup time and peak hours for resynchronization

	readInfrared (); // Read the in-comming IR signal if present

	readSerial (); // Receive datas from ESP8266 for Wi-Wi control

	action (); // Do something according to current mode

	light (); // Finaly display the RGB value
}

// Test wakeup time and peak hours for resynchronization
void testWakeUpTime ()
{
	// If actual time coorespond with wakeup time
	if (WAKEUP_HOUR == hour () && WAKEUP_MINUTE == minute ())
	{
		if (!wokeUp) // Double if so else statement is not called
		{
			mode = MODE_WAKEUP;
			on = true;
			wokeUp = true;
			if (DEBUG_ENABLED)
				Serial.println ("Wake up !\n");
		}
	}
	else
	{
		wokeUp = false;
	}

	// If we are on a peak time, we ask for time to make sure it's right
	if (minute () == 0)
	{
		if (!timeAsked) // Double if so else statement is not called
		{
			askForTime ();
			timeAsked = true;
		}
	}
	else
	{
		timeAsked = false;
	}
}

// Asking for time to the ESP8266 (via internet)
void askForTime ()
{
	if (DEBUG_ENABLED)
		Serial.print ("Gently asking for time (");

	Serial.print ("TIMEPLEASE");

	if (DEBUG_ENABLED)
		Serial.println (")\n");
}

// Display the RGB value
void light ()
{
	rgb2color (); // Convert RGB value to Red, Green and Blue values

	analogWrite (LED_RED, on ? red : 0);
	analogWrite (LED_GREEN, on ? green : 0);
	analogWrite (LED_BLUE, on ? blue : 0);
}

// Convert RGB value to Red, Green and Blue values
void rgb2color ()
{
	// Lower is the power, lower is the color value - It allows you to control LEDs light power
	red = ((rgb & 0xFF0000) >> 16) * (power / MAX_POWER);
	green = ((rgb & 0x00FF00) >> 8) * (power / MAX_POWER);
	blue = (rgb & 0x0000FF) * (power / MAX_POWER);

	// Color values without the power
	unpoweredRed = ((rgb & 0xFF0000) >> 16);
	unpoweredGreen = ((rgb & 0x00FF00) >> 8);
	unpoweredBlue = (rgb & 0x0000FF);
}

// Read the in-comming IR signal if present
void readInfrared ()
{
	// If something is comming from IR reciever
	if (irrecv.decode (&results))
	{
		// We save the IR word in IRCode
		IRCode = results.value;

		// REPEAT (When button is press continiously, sent value is 0xFFFFFFFF, so we change it with the latest code that we recieved
		if (IRCode == 0xFFFFFFFF)
			IRCode = lastIRCode;

		// ON
		if (IRCode == 0xFFB04F || IRCode == 0xF0C41643)
		{
			on = true;
			lastIRCode = 0; // We don't save value in lastIRCode because don't care if we keep on button pressed
		}

		// [DEBUG] Print the incomming IR value
		if (DEBUG_ENABLED)
		{
			Serial.print ("Incomming IR: ");
			Serial.println (IRCode, HEX);
		}

		// If the system is off, ignore incomming infrared (Except ON of course, he is just above)
		if (!on)
		{
			irrecv.resume ();
			return;
		}

		// Test incomming value
		switch (IRCode)
		{
			//OFF
			case 0xFFF807:
			case 0xE721C0DB:
				on = false;
				lastIRCode = 0;
				break;
				break;

				//DOWN
			case 0xFFB847:
			case 0xA23C94BF:
				if (mode == MODE_STROBE) // If we are in STROBE mode, decrease speed instead of power
				{
					if (strobeSpeed - STROBE_SPEED >= MIN_STROBE)
						strobeSpeed -= STROBE_SPEED;
					else
						strobeSpeed = MIN_STROBE;
				}
				else
				{
					if (power - POWER_SPEED >= MIN_POWER)
						power -= POWER_SPEED;
					else
						power = MIN_POWER;
				}

				rgb2color ();
				lastIRCode = IRCode;

				// [DEBUG] Print current color and RED, GREEN, BLUE values
				if (DEBUG_ENABLED)
				{
					Serial.print ("Power: ");
					Serial.println (power);
					Serial.print ("RED: ");
					Serial.print (red);
					Serial.print (" / GREEN: ");
					Serial.print (green);
					Serial.print (" / BLUE: ");
					Serial.println (blue);
					Serial.println ();
				}
				break;
				break;

				//UP
			case 0xFF906F:
			case 0xE5CFBD7F:
				if (mode == MODE_FLASH) // If we are in flash mode, increase flash speed instead of power
				{
					if (flashSpeed + FLASH_SPEED <= MAX_FLASH)
						flashSpeed += FLASH_SPEED;
					else
						flashSpeed = MAX_FLASH;
				}
				if (mode == MODE_STROBE) // If we are in strobe mode, increase strobe speed instead of power
				{
					if (strobeSpeed + STROBE_SPEED <= MAX_STROBE)
						strobeSpeed += STROBE_SPEED;
					else
						strobeSpeed = MAX_STROBE;
				}
				else
				{
					if (power + POWER_SPEED <= MAX_POWER)
						power += POWER_SPEED;
					else
						power = MAX_POWER;
				}

				rgb2color ();
				lastIRCode = IRCode;

				// [DEBUG] Print current color and red, green, blue values
				if (DEBUG_ENABLED)
				{
					Serial.print ("Power: ");
					Serial.println (power);
					Serial.print ("RED: ");
					Serial.print (red);
					Serial.print (" / GREEN: ");
					Serial.print (green);
					Serial.print (" / BLUE: ");
					Serial.println (blue);
					Serial.println ();
				}
				break;
				break;

				//STROBE
			case 0xFF00FF:
			case 0xFA3F159F:
				mode = MODE_STROBE;
				break;
				break;

				//FADE
			case 0xFF58A7:
			case 0xDC0197DB:
				mode = MODE_FADE;
				break;
				break;

				//SMOOTH
			case 0xFF30CF:
			case 0x9716BE3F:
				mode = MODE_SMOOTH;
				break;
				break;

				//COLORS
			default:
				lastIRCode = 0;
				for (i = 0; i < N; i++)
					if (results.value == color[i][1]
							|| results.value == color[i][2])
					{
						mode = MODE_DEFAULT;
						lastMode = MODE_DEFAULT;
						rgb = color[i][0];
					}
				break;
		}
		irrecv.resume ();
	}
}

// Receive datas from ESP8266 for Wi-Wi control
void readSerial ()
{
	if (!Serial.available ())
		return; // Waiting for incomming datas

	// Reading incomming message
	for (n = 0; Serial.available () && n < 20; n++)
	{
		messageChar[n] = Serial.read ();
		delay (1);
	}

	message = "";

	// Converting char array into String
	for (i = 0; i < n; i++)
		message += String (messageChar[i]);

	messageLength = message.length (); // Message length

	// Testing what kind of data we are receiving (Testing if the prefix is present at position 0)
	if (message.indexOf ("TIM") == 0)
		infoType = TYPE_TIME;
	else if (message.indexOf ("ON") == 0)
		infoType = TYPE_ON;
	else if (message.indexOf ("RGB") == 0)
		infoType = TYPE_RGB;
	else if (message.indexOf ("POW") == 0)
		infoType = TYPE_POW;
	else if (message.indexOf ("MOD") == 0)
		infoType = TYPE_MOD;
	else  // If the prefix correspond to nothing or there is no prefix
	{
		// If DEBUG is active, we continue with unkown type
		if (DEBUG_ENABLED)
			infoType = TYPE_UNKNOWN;

		// If not, we stop
		else
			return;
	}

	// Testing if data length is valid
	if (infoType == TYPE_TIME && messageLength > 13 && !DEBUG_ENABLED)
		return;
	if (infoType == TYPE_ON && messageLength != 3 && !DEBUG_ENABLED)
		return;
	if (infoType == TYPE_RGB && messageLength > 9 && !DEBUG_ENABLED)
		return;
	if (infoType == TYPE_POW && messageLength > 6 && !DEBUG_ENABLED)
		return;
	if (infoType == TYPE_MOD && messageLength != 4 && !DEBUG_ENABLED)
		return;

	// [DEBUG] Printing full word, world length and information type
	if (DEBUG_ENABLED)
	{
		Serial.print ("Word: ");
		Serial.println (message);
		Serial.print ("Length: ");
		Serial.println (messageLength);
		Serial.print ("Type: ");
		Serial.println (
				infoType == TYPE_TIME ? "TIME" : infoType == TYPE_ON ? "ON" :
				infoType == TYPE_RGB ? "RGB" : infoType == TYPE_POW ? "POW" :
				infoType == TYPE_MOD ? "MOD" : "UNKNOWN");
	}

	message.remove (0, infoType == TYPE_ON ? 2 : 3); // Remove 2 first characters if "ON" type and 3 first ones if "TIME", "RGB", "POW" or "MOD" type

	// [DEBUG] printing information without prefix
	if (DEBUG_ENABLED)
	{
		Serial.print (
				infoType == TYPE_TIME ? "TIME: " :
				infoType == TYPE_ON ? "ON: " : infoType == TYPE_RGB ? "RGB: " :
				infoType == TYPE_POW ? "POW: " :
				infoType == TYPE_MOD ? "MOD: " : "UNKNOWN: ");
		Serial.println (
				(infoType == TYPE_ON) ?
						((message == "1") ? "True" :
							(message == "0") ? "False" : "Error") :
				(infoType == TYPE_MOD) ?
						(message.charAt (0) == MODE_FLASH + '0' ? "FLASH (" :
							message.charAt (0) == MODE_STROBE + '0' ?
									"STROBE (" :
							message.charAt (0) == MODE_FADE + '0' ? "FADE (" :
							message.charAt (0) == MODE_SMOOTH + '0' ?
									"SMOOTH (" : "UNKNOWN (") + message + ")" :
						message);
		// This is the end of debuging for these types
		if (infoType == TYPE_ON || infoType == TYPE_MOD
				|| infoType == TYPE_UNKNOWN)
			Serial.println ();
	}

	if (infoType == TYPE_TIME)
	{
		message.toCharArray (charTime, 11);
		setTime (strtol (charTime, NULL, 10));
	}
	else if (infoType == TYPE_ON)
	{
		on = message.charAt (0) - '0';
	}
	else if (infoType == TYPE_RGB)
	{
		message.toCharArray (charRgb, 7);
		rgb = strtol (charRgb, NULL, 16);
	}
	else if (infoType == TYPE_POW)
	{
		message.toCharArray (charPow, 4);

		if (mode == MODE_FLASH) // If we are in STROBE mode, changing strobe speed instead of power
			flashSpeed = strtol (charPow, NULL, 10)
					/ (MAX_POWER / (float) MAX_FLASH);
		else if (mode == MODE_STROBE) // If we are in STROBE mode, changing strobe speed instead of power
			strobeSpeed = strtol (charPow, NULL, 10)
					/ (MAX_POWER / (float) MAX_STROBE);
		else
			power = strtol (charPow, NULL, 10);
	}
	else if (infoType == TYPE_MOD)
	{
		mode = message.charAt (0) - '0';
	}

	if (DEBUG_ENABLED)
	{
		if (infoType == TYPE_TIME)
		{
			Serial.print ("TIME (number): ");
			Serial.println (now ());
			Serial.print ("TIME (readable): ");
			digitalClockDisplay ();
			Serial.println ("\n");
		}
		else if (infoType == TYPE_RGB)
		{
			Serial.print ("Full RGB (number): ");
			Serial.println (rgb, HEX);
			Serial.println ();
			Serial.print ("RED: ");
			Serial.println (red, HEX);
			Serial.println ();
			Serial.print ("GREEN: ");
			Serial.println (green, HEX);
			Serial.println ();
			Serial.print ("BLUE: ");
			Serial.println (blue, HEX);
			Serial.println ();

		}
		else if (infoType == TYPE_POW)
		{
			Serial.print ("Full POW (number): ");
			Serial.println (
					mode == MODE_FLASH ?
							(int) (flashSpeed * (MAX_POWER / (float) MAX_FLASH)) :
					MODE_STROBE ?
							(int) (strobeSpeed
									* (MAX_POWER / (float) MAX_STROBE)) :
							(int) power);
			Serial.println ();
		}
	}
}

// Digital clock display of the time
void digitalClockDisplay ()
{
	printDigits (day ());
	Serial.print ("/");
	printDigits (month ());
	Serial.print ("/");
	Serial.print (year ());

	Serial.print (" ");

	printDigits (hour ());
	Serial.print (":");
	printDigits (minute ());
	Serial.print (":");
	printDigits (second ());
}

// Utility for digital clock display: prints preceding colon and leading 0
void printDigits (int digits)
{
	if (digits < 10)
		Serial.print ('0');
	Serial.print (digits);
}

// Perform mode action
void action ()
{
	// If lightning if off or mode is constant lightnin, don't do anything
	if (!on || mode == MODE_DEFAULT)
		return;

	// Calling modes functions
	switch (mode)
	{
		case MODE_FLASH:
			if (lastMode != MODE_FLASH) // If this is first call of the function, we call init function (lastMode will be set in init function)
				initModeFlash ();
			modeFlash ();
			break;

		case MODE_STROBE:
			if (lastMode != MODE_STROBE)
				initModeStrobe ();
			modeStrobe ();
			break;

		case MODE_FADE:
			if (lastMode != MODE_FADE)
				initModeFade ();
			modeFade ();
			break;

		case MODE_SMOOTH:
			if (lastMode != MODE_SMOOTH)
				initModeSmooth ();
			modeSmooth ();
			break;

		case MODE_WAKEUP:
			if (lastMode != MODE_WAKEUP)
				initModeWakeup ();
			modeWakeup ();
			break;
	}
}

// Flash mode initialization
void initModeFlash ()
{
	state = 0; // Set initial state to 0
	rgb = 0xFF0000; // Set color to red
	count = 0; // Reseting counter
	lastMode = MODE_FLASH; // Setting lastMode so we don't call init again
	if (DEBUG_ENABLED)
		Serial.println ("Entering Flash mode\n");
}

// Flash mode
void modeFlash ()
{
	if (count >= MIN_FLASH + MAX_FLASH - flashSpeed) // Counting up for a certain amount of time
	{
		if (state >= 2)
			state = 0;
		else
			state++; // Incrementing state

		count = 0; // Reseting timer
	}
	else
	{
		count++; // incrementing counter
	}

	rgb = (state == 0 ? 0xFF0000 : state == 1 ? 0x00FF00 : 0x0000FF);
}

// Strobe mode initialization
void initModeStrobe ()
{
	state = 0; // Set initial state to 0
	rgb = 0xFFFFFF; // Set color to white
	count = 0; // Reseting counter
	lastMode = MODE_STROBE; // Setting lastMode so we don't call init again
	if (DEBUG_ENABLED)
		Serial.println ("Entering Strobe mode\n");
}

// Strobe mode
void modeStrobe ()
{
	if (count >= MIN_STROBE + MAX_STROBE - strobeSpeed) // Counting up for a certain amount of time
	{
		state = !state; // Inverting state
		count = 0; // Reseting timer
	}
	else
	{
		count++; // incrementing counter
	}

	rgb = state ? 0xFFFFFF : 0x000000; // Setting color to black then white then black then white...

}

// Fade Mode initialization
void initModeFade ()
{
	state = 1; // Setting state to Increasing state
	rgb = 0xFFFFFF; // Setting color to white
	power = 0; // Setting power to 0 (LED's shutted down)
	lastMode = MODE_FADE; // Setting lastMode so we don't call init again
	if (DEBUG_ENABLED)
		Serial.println ("Entering Fade mode\n");
}

// Fade Mode
void modeFade ()
{
	if (state)
	{
		power += FADE_SPEED; // Increasing power
	}
	else
	{
		power -= FADE_SPEED; // Decreasing power
	}

	if (power >= MAX_POWER - 1) // If power reach MAX_POWER-1, we start to decrease
	{
		state = 0; // Decreasing state
	}

	if (power <= 0) // If power reach 1, we start to increase
	{
		state = 1; // Increasing state
	}
}

// Smooth Mode Initialization
void initModeSmooth ()
{
	state = 0; // Init state to 0
	rgb = 0xFE0000; // Init color to red
	rgb2color (); // Calling rgb2color to generate color values
	lastMode = MODE_SMOOTH; // Setting lastMode so we don't call init again
	if (DEBUG_ENABLED)
		Serial.println ("Entering Smooth mode\n");
}

// Smooth Mode
void modeSmooth ()
{
	// First, RED is max

	// Increasing GREEN until max
	if (state == 0)
	{
		if (unpoweredGreen >= 254)
			state = 1;
		else
			rgb += 0x000100;
	}
	// Decreasing RED until 0
	else if (state == 1)
	{
		if (unpoweredRed <= 0)
			state = 2;
		else
			rgb -= 0x010000;
	}
	// Increasing BLUE until max
	else if (state == 2)
	{
		if (unpoweredBlue >= 254)
			state = 3;
		else
			rgb += 0x000001;
	}
	// Decreasing GREEN until 0
	else if (state == 3)
	{
		if (unpoweredGreen <= 0)
			state = 4;
		else
			rgb -= 0x000100;
	}
	// Increasing RED until max
	else if (state == 4)
	{
		if (unpoweredRed >= 254)
			state = 5;
		else
			rgb += 0x010000;
	}
	// Decreasing BLUE until 0
	else if (state == 5)
	{
		if (unpoweredBlue <= 0)
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
}

// Wakeup Mode initialization
void initModeWakeup ()
{
	rgb = 0x0000FF; // Setting color to blue
	power = 0; // Setting power to 0
	lastMode = MODE_WAKEUP; // Setting lastMode so we don't call init again
	if (DEBUG_ENABLED)
		Serial.println ("Entering Wakeup mode\n");
}

// Wakeup Mode
void modeWakeup ()
{
	power += WAKEUP_SPEED; // Slowly increase power

	if (power >= MAX_POWER)
		mode = lastMode = MODE_DEFAULT; // When max power is reached, leaving the mode
}
