#include <IRremote.h>
#include <TimeLib.h>

#define DEBUG_BAUD_RATE  250000 // Debug baud rate
#define ESP_BAUD_RATE    250000 // ESP8266 communication baud rate

#define DEBUG_ENABLED    true  // DEBUG Mode
#define WAIT_FOR_TIME    true  // If we have to wait for time sync (if true, program will not start until time is synced)
#define INFRARED_ENABLED false // If we allow infrared communication

// ******* Wake up ******* //
#define WAKEUP_HOUR   6
#define WAKEUP_MINUTE 17

// ******* Pins ******* //
#define PIN_LED_INFRARED 3  // Infrared IN pin
#define PIN_SOUND        2  // Sound detector IN pin
#define PIN_LED_RED      9  // Red LED OUT pin
#define PIN_LED_GREEN    10 // Green LED OUT pin
#define PIN_LED_BLUE     11 // Blue LED OUT pin

// ******* Power ******* //
#define MIN_POWER     0   // Minimum power value
#define MAX_POWER     100 // Maximum power value
#define DEFAULT_POWER 50  // Default power value

// ******* Infrared ******* //
#define POWER_CHANGE_SPEED 5 // Power increasion/decreasion speed

// ******* Modes ******* //
/******** Flash *******/
#define MIN_FLASH              -100 // Minimum flash speed
#define MAX_FLASH              50   // Maximum flash speed
#define IR_FLASH_CHANGE_SPEED  1    // Flash increasion or dicreasion speed
#define DEFAULT_FLASH_SPEED    0    // Default flash speed value
/******** Strobe *******/
#define MIN_STROBE             -100 // Minimum strobe speed
#define MAX_STROBE             50   // Maximum strobe speed
#define IR_STROBE_CHANGE_SPEED 1    // Strobe increasion or dicreasion speed
#define DEFAULT_STROBE_SPEED   0    // Default strobe speed value
/******** Fade *******/
#define MIN_FADE               70  // Minimum fade speed
#define MAX_FADE               100 // Maximum fade speed
#define IR_FADE_CHANGE_SPEED   1   // Fade increasion or dicreasion speed
#define DEFAULT_FADE_SPEED     75  // Default fade speed value
/******** Smooth *******/
#define MIN_SMOOTH             76  // Minimum smooth speed
#define MAX_SMOOTH             100 // Maximum smooth speed
#define IR_SMOOTH_CHANGE_SPEED 1   // Smooth increasion or dicreasion speed
#define DEFAULT_SMOOTH_SPEED   80  // Default smooth speed value
/******** Wake up *******/
#define WAKE_UP_SLOWNESS       1000 // Wake up slowness

// ******** IDs ******** //
/******** Serial reception types ********/
#define TYPE_UNKNOWN -1
#define TYPE_TIME    0
#define TYPE_RGB     1
#define TYPE_ON      2
#define TYPE_POW     3
#define TYPE_MOD     4
#define TYPE_PRT     5
/******** Modes ********/
#define MODE_DEFAULT 0
#define MODE_FLASH   1
#define MODE_STROBE  2
#define MODE_FADE    3
#define MODE_SMOOTH  4
#define MODE_WAKEUP  5

// ******** Sound ******** //
#define MIN_CLAP_DURATION          30  // Amount of time to ignore after clap started (To avoid sound bounce)
#define MAX_CLAP_DURATION          100 // Maximum clap duration in ms

#define MIN_TIME_BEETWIN_TWO_CLAPS 400 // Minimum time before starting second clap
#define MAX_TIME_BEETWIN_TWO_CLAPS 600 // Maximum time to start second clap

#define TIME_AFTER_START_OVER      1000 // Time to wait after double clap to start over

// Infrared reception objects declaration
IRrecv irrecv (PIN_LED_INFRARED);
decode_results results;

#define N_COLOR  16 // Number of different colors
#define N_PRAYER 6  // Number of different prayer (including sunrise)

// Infrared codes and corresponding RGB code array
unsigned long color[N_COLOR][3] =
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
	{ 0xFF7B92, 0xFFF00F, 0x35A9425F } // B5
};


// ******* Useful ******* //
int i, j, n; // Counting variables (used for "for" statements)

// ******* Global ******* //
boolean on;          // If the leds are ON or OFF (True: ON / False: OFF)
unsigned long rgb;   // Currently displayed RGB value (From 0x000000 to 0xFFFFFF)
float power;         // Current lightning power (from MINPOWER to MAXPOWER)
unsigned char red;   // Currentlty red value including lightning power (From 0 to 255)
unsigned char green; // Currentlty green value including lightning power (From 0 to 255)
unsigned char blue;  // Currentlty blue value including lightning power (From 0 to 255)

// ******* testWakeUpTime ******* //
boolean wokeUp;    // Used to set Wakeup mode only once
boolean timeAsked; // Used to ask time only once every peak time

// ******* Modes ******* //
unsigned char mode; // Current lighting mode (0: Constant lightning / 1: Flash / 2: Strobe / 3: Fade / 4: Smooth / 5: Wakeup)
int state;          // Current state used by some modes
int count;          // Delay counting
/******* action *******/
int lastMode;          // Mode in previous loop - Allows mode initializations
unsigned long lastRgb; // Saved RGB value before we leave default mode
float lastPower;       // Saved power value before we leave default mode
/******* modeFlash ********/
int flashSpeed; // Current flash speed (From MINFLASH to MAXFLASH)
/******* modeStrobe *******/
int strobeSpeed; // Current strobe speed (From MINSTROBE to MAXSTROBE)
/******* modeFade *******/
int fadeSpeed; // Current fade speed (From MINFADE to MAXFADE)
/******* modeSmooth *******/
int smoothSpeed; // Current smooth speed (From MINSMOOTH to MAXSMOOTH)

// ******* readInfrared ******* //
unsigned long lastIRCode; // IR code in previous loop - Allows continious power / strobe speed increasion / dicreasion
unsigned long IRCode;     // Current IR code

// ******* readSerial ******* //
int infoType;                              // Information type ("Time", "ON", "RGB", "Power" or "Mode")
int messageLength;                         // Message length
char charRgb[7], charPow[4], charTime[11]; // Char arrays for message decrypting with strtol function
char messageChar[20];                      // Received message
String message;                            // Received message converted to String
String prayersName[N_PRAYER] = { "Fajr", "Sunrise", "Dhuhr", "Asr", "Maghrib", "Isha" };
int prayerTime[N_PRAYER][3];

// ******* readClaps ******* //
int clapState;              // Same as "state" but for claps
unsigned long endStateTime; // Time position at the end of a state (Allow time counting)

// ******* onPrayerTime ******* //
boolean flagEnter, flagLeave;
boolean faded, unfaded;

// ******** Functions prototypes ******** //
void setup ();
void loop ();
void peakTime ();
void readClaps ();
void onPrayerTime ();
void testWakeUpTime ();
void askForTime ();
void light ();
void rgb2color ();
void readInfrared ();
void readSerial ();
void digitalClockDisplay ();
void printDigits (int digits);
void action ();
void initModeFlash ();
void modeFlash ();
void initModeStrobe ();
void modeStrobe ();
void initModeFade ();
void modeFade ();
void initModeSmooth ();
void modeSmooth ();
void initModeWakeup ();
void modeWakeup ();

void setup ()
{
	Serial.begin (DEBUG_BAUD_RATE); // Initialize debug communication
	Serial1.begin (ESP_BAUD_RATE);  // Initialize ESP8266 communication

	pinMode (PIN_SOUND, INPUT); // Setting sound detector as an input

	if (DEBUG_ENABLED)
		Serial.println ("Starting program\n");

	on = false; // LEDs are off on startup

	if (INFRARED_ENABLED)
		irrecv.enableIRIn();  // Initialize IR communication

	rgb = 0xFFFFFF; // Initialize color to white

	// Initialzing to default values
	mode        = MODE_DEFAULT;         // Initialize mode to constant lightning
	lastMode    = MODE_DEFAULT;         // Initialiazing last mode as well
	power       = DEFAULT_POWER;        // Initializing power its default value
	flashSpeed  = DEFAULT_FLASH_SPEED;  // Initializing flash speed to its default value
	strobeSpeed = DEFAULT_STROBE_SPEED; // Initializing strobe speed to its default value
	fadeSpeed   = DEFAULT_FADE_SPEED;   // Initializing fade speed to its default value
	smoothSpeed = DEFAULT_SMOOTH_SPEED; // Initializing smooth speed to its default value

	clapState    = 0;
	endStateTime = 0;
	lastIRCode   = 0;
	wokeUp       = false;
	timeAsked    = false;

	// Gently ask for time
	i = 5000;
	while (timeStatus() == timeNotSet && WAIT_FOR_TIME) // Doesn't start if time isn't set (only if WAIT_FOR_TIME = 1)
	{
		if (i >= 5000)
		{
			askForTime();
			i = 0;
		}

		readSerial();
		i++;
		delay (1);
	}

	if (DEBUG_ENABLED)
		Serial.println ("Program started\n");
} // setup

void loop ()
{
	peakTime(); // If we are on a peak time, we ask for time to make sure it's right

	readClaps(); // Lighting on double claps

	onPrayerTime();

	testWakeUpTime(); // Test wakeup time and peak hours for resynchronization

	readInfrared(); // Read the in-comming IR signal if present

	readSerial(); // Receive datas from ESP8266 for Wi-Wi control

	action(); // Do something according to current mode

	light(); // Finaly display the RGB value
}

void peakTime ()
{
	// If we are on a peak time, we ask for time to make sure it's right
	if (minute() == 0)
	{
		if (!timeAsked) // Double if so else statement is not called
		{
			askForTime();
			timeAsked = true;
		}
	}
	else
	{
		timeAsked = false;
	}
}

// Lighting on double claps
void readClaps ()
{
	if (clapState == 0)
	{
		if (digitalRead (PIN_SOUND) == HIGH && millis() - endStateTime >= TIME_AFTER_START_OVER) // Start of first clap
		{
			clapState    = 1;
			endStateTime = millis();
			Serial.println ("Enter state 1 (Now waiting for the clap to end)\n");
		}
	}
	else if (clapState == 1)
	{
		if (millis() - endStateTime > MAX_CLAP_DURATION) // If clap duration reach maximum
		{
			clapState = 0;
			Serial.println ("Going back to state 0 (First clap lasted too long)\n");
		}

		if (digitalRead (PIN_SOUND) == LOW && millis() - endStateTime >= MIN_CLAP_DURATION) // End of first clap
		{
			clapState    = 2;
			endStateTime = millis();
			Serial.println ("Enter state 2 ( Now waiting a little bit...)\n");
		}
	}
	else if (clapState == 2)
	{
		if (digitalRead (PIN_SOUND) == HIGH) // Too soon second clap
		{
			clapState = 0;
			Serial.println ("Going back to state 0 ( Second clap started too soon)\n");
		}

		if (millis() - endStateTime >= MIN_TIME_BEETWIN_TWO_CLAPS)
		{
			clapState    = 3;
			endStateTime = millis();
			Serial.println ("Enter state 3 ( Now waiting for second clap to start)\n");
		}
	}
	else if (clapState == 3)
	{
		if (millis() - endStateTime >= MAX_TIME_BEETWIN_TWO_CLAPS + MIN_TIME_BEETWIN_TWO_CLAPS)
		{
			clapState = 0;
			Serial.println ("Going back to state 0 ( Waited too long for second clap)\n");
		}
		if (digitalRead (PIN_SOUND) == HIGH) // Start of second clap
		{
			clapState    = 4;
			endStateTime = millis();
			Serial.println ("Enter state 4 ( Now waiting for second clap to end)\n");
		}
	}
	else if (clapState == 4)
	{
		if (millis() - endStateTime >= MAX_CLAP_DURATION)
		{
			clapState = 0;
			Serial.println ("Going back to state 0 (Second clap lasted too long)\n");
		}
		if (digitalRead (PIN_SOUND) == LOW && millis() - endStateTime >= MIN_CLAP_DURATION) // End of second clap
		{
			on           = !on;
			clapState    = 0;
			endStateTime = millis();
			Serial.println ("Switching LED !\n");
		}
	}
} // readClaps

// Test wakeup time and peak hours for resynchronization
void onPrayerTime ()
{
	flagEnter = false;
	flagLeave = false;

	for (i = 0; i < N_PRAYER; i++)
		if ((hour() * 60 + minute()) == prayerTime[i][2])
		{
			flagEnter = true;
			break;
		}

	for (j = 0; j < N_PRAYER; j++)
		if ((hour() * 60 + minute()) == (prayerTime[j][2] + 10))
		{
			flagLeave = true;
			break;
		}


	if (flagEnter)
	{
		if (!faded) // Double if so else statement is not called
		{
			mode      = MODE_FADE;
			fadeSpeed = 97;
			on        = true;
			faded     = true;

			if (DEBUG_ENABLED)
			{
				Serial.print ("Time to pray ");
				Serial.print (prayersName[i]);
				Serial.print (" !\nIt will stop at ");
				Serial.print (prayerTime[i][2] + 10);
				Serial.print (" (");
				Serial.print (prayerTime[i][0]);
				Serial.print (":");
				Serial.print (prayerTime[i][1]);
				Serial.print (")\n");
			}
		}
	}
	else
	{
		faded = false;
	}

	if (flagLeave)
	{
		if (!unfaded) // Double if so else statement is not called
		{
			mode    = MODE_DEFAULT;
			on      = false;
			unfaded = true;

			if (DEBUG_ENABLED)
			{
				Serial.print ("Stop ");
				Serial.print (prayersName[j]);
				Serial.println (" alert\n");
			}
		}
	}
	else
	{
		unfaded = false;
	}
} // onPrayerTime

// Test wakeup time and peak hours for resynchronization
void testWakeUpTime ()
{
	// If actual time coorespond with wakeup time
	if (hour() == prayerTime[0][0] && minute() == prayerTime[0][1])
	{
		if (!wokeUp) // Double if so else statement is not called
		{
			mode   = MODE_WAKEUP;
			on     = true;
			wokeUp = true;

			if (DEBUG_ENABLED)
				Serial.println ("Wake up !\n");
		}
	}
	else
	{
		wokeUp = false;
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
	rgb2color(); // Convert RGB value to Red, Green and Blue values

	analogWrite (PIN_LED_RED, on ? red : 0);
	analogWrite (PIN_LED_GREEN, on ? green : 0);
	analogWrite (PIN_LED_BLUE, on ? blue : 0);
}

// Convert RGB value to Red, Green and Blue values
void rgb2color ()
{
	// Lower is the power, lower is the color value - It allows you to control LEDs light power
	red   = ((rgb & 0xFF0000) >> 16) * (power / MAX_POWER);
	green = ((rgb & 0x00FF00) >> 8) * (power / MAX_POWER);
	blue  = (rgb & 0x0000FF) * (power / MAX_POWER);
}

// Read the in-comming IR signal if present
void readInfrared ()
{
	if (!INFRARED_ENABLED)
		return;

	// If something is comming from IR reciever
	if (irrecv.decode (&results))
	{
		// We save the IR word in IRCode
		IRCode = results.value;

		// REPEAT (When button is pressed continiously, sent value is 0xFFFFFFFF, so we change it with the latest code that we recieved
		if (IRCode == 0xFFFFFFFF)
			IRCode = lastIRCode;

		// ON
		if (IRCode == 0xFFB04F || IRCode == 0xF0C41643)
		{
			on         = true;
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
			irrecv.resume();
			return;
		}

		// Test incomming value
		switch (IRCode)
		{
			// OFF
			case 0xFFF807:
			case 0xE721C0DB:
				on         = false;
				lastIRCode = 0;
				break;
				break;

			// DOWN
			case 0xFFB847:
			case 0xA23C94BF:
				if (mode == MODE_FLASH) // If we are in flash mode, decrease speed instead of power
				{
					if (flashSpeed - IR_FLASH_CHANGE_SPEED >= MIN_FLASH)
						flashSpeed -= IR_FLASH_CHANGE_SPEED;
					else
						flashSpeed = MIN_FLASH;
				}
				else if (mode == MODE_STROBE) // If we are in STROBE mode, decrease speed instead of power
				{
					if (strobeSpeed - IR_STROBE_CHANGE_SPEED >= MIN_STROBE)
						strobeSpeed -= IR_STROBE_CHANGE_SPEED;
					else
						strobeSpeed = MIN_STROBE;
				}
				else if (mode == MODE_FADE) // If we are in fade mode, decrease speed instead of power
				{
					if (fadeSpeed - IR_FADE_CHANGE_SPEED >= MIN_FADE)
						fadeSpeed -= IR_FADE_CHANGE_SPEED;
					else
						fadeSpeed = MIN_FADE;
				}
				else if (mode == MODE_SMOOTH) // If we are in smooth mode, decrease speed instead of power
				{
					if (smoothSpeed - IR_SMOOTH_CHANGE_SPEED >= MIN_SMOOTH)
						smoothSpeed -= IR_SMOOTH_CHANGE_SPEED;
					else
						smoothSpeed = MIN_SMOOTH;
				}
				else
				{
					if (power - POWER_CHANGE_SPEED >= MIN_POWER)
						power -= POWER_CHANGE_SPEED;
					else
						power = MIN_POWER;
				}

				rgb2color();
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
					Serial.println();
				}
				break;
				break;

			// UP
			case 0xFF906F:
			case 0xE5CFBD7F:
				if (mode == MODE_FLASH) // If we are in flash mode, increase speed instead of power
				{
					if (flashSpeed + IR_FLASH_CHANGE_SPEED <= MAX_FLASH)
						flashSpeed += IR_FLASH_CHANGE_SPEED;
					else
						flashSpeed = MAX_FLASH;
				}
				else if (mode == MODE_STROBE) // If we are in strobe mode, increase speed instead of power
				{
					if (strobeSpeed + IR_STROBE_CHANGE_SPEED <= MAX_STROBE)
						strobeSpeed += IR_STROBE_CHANGE_SPEED;
					else
						strobeSpeed = MAX_STROBE;
				}
				else if (mode == MODE_FADE) // If we are in fade mode, increase speed instead of power
				{
					if (fadeSpeed + IR_FADE_CHANGE_SPEED <= MAX_FADE)
						fadeSpeed += IR_FADE_CHANGE_SPEED;
					else
						fadeSpeed = MAX_FADE;
				}
				else if (mode == MODE_SMOOTH) // If we are in smooth mode, increase speed instead of power
				{
					if (smoothSpeed + IR_SMOOTH_CHANGE_SPEED <= MAX_SMOOTH)
						smoothSpeed += IR_SMOOTH_CHANGE_SPEED;
					else
						smoothSpeed = MAX_SMOOTH;
				}
				else
				{
					if (power + POWER_CHANGE_SPEED <= MAX_POWER)
						power += POWER_CHANGE_SPEED;
					else
						power = MAX_POWER;
				}

				rgb2color();
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
					Serial.println();
				}
				break;
				break;

			// STROBE
			case 0xFF00FF:
			case 0xFA3F159F:
				mode = MODE_STROBE;
				break;
				break;

			// FADE
			case 0xFF58A7:
			case 0xDC0197DB:
				mode = MODE_FADE;
				break;
				break;

			// SMOOTH
			case 0xFF30CF:
			case 0x9716BE3F:
				mode = MODE_SMOOTH;
				break;
				break;

			// COLORS
			default:
				lastIRCode = 0;
				for (i = 0; i < N_COLOR; i++)
					if (results.value == color[i][1] || results.value == color[i][2])
					{
						mode     = MODE_DEFAULT;
						lastMode = MODE_DEFAULT;
						rgb      = color[i][0];
					}
				break;
		}
		irrecv.resume();
	}
} // readInfrared

// Receive datas from ESP8266 for Wi-Wi control
void readSerial ()
{
	if (!Serial.available())
		return;  // Waiting for incomming datas

	// Reading incomming message
	for (n = 0; Serial.available() && n < 20; n++)
	{
		messageChar[n] = Serial.read();
		delay (1);
	}

	message = "";

	// Converting char array into String
	for (i = 0; i < n; i++)
		message += String (messageChar[i]);

	messageLength = message.length(); // Message length

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
	else if (message.indexOf ("PRT") == 0)
		infoType = TYPE_PRT;
	else // If the prefix correspond to nothing or there is no prefix
	{
		// If DEBUG is active, we continue with unkown type
		if (DEBUG_ENABLED)
			infoType = TYPE_UNKNOWN;

		// If not, we stop
		else return;
	}

	if (DEBUG_ENABLED)
	{
		// [DEBUG] Printing full word, world length and information type
		Serial.print ("Word: ");
		Serial.println (message);
		Serial.print ("Length: ");
		Serial.println (messageLength);
		Serial.print ("Type: ");
		Serial.println (
			infoType == TYPE_TIME ? "TIME" : infoType == TYPE_ON ? "ON" :
			infoType == TYPE_RGB ? "RGB" : infoType == TYPE_POW ? "POW" :
			infoType == TYPE_MOD ? "MOD" : infoType == TYPE_PRT ? "PRT" : "UNKNOWN");
	}

	// Testing if data length is valid
	if (infoType == TYPE_TIME && messageLength > 13 && !DEBUG_ENABLED)
		return;
	else if (infoType == TYPE_ON && messageLength != 3 && !DEBUG_ENABLED)
		return;
	else if (infoType == TYPE_RGB && messageLength > 9 && !DEBUG_ENABLED)
		return;
	else if (infoType == TYPE_POW && messageLength > 6 && !DEBUG_ENABLED)
		return;
	else if (infoType == TYPE_MOD && messageLength != 4 && !DEBUG_ENABLED)
		return;
	else if (infoType == TYPE_PRT && messageLength > 7 && !DEBUG_ENABLED)
		return;

	message.remove (0, infoType == TYPE_ON ? 2 : 3); // Remove 2 first characters if "ON" type and 3 first ones if "TIME", "RGB", "POW" or "MOD" type

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

		if (mode == MODE_FLASH) // If we are in flash mode, changing flash speed instead of power
			flashSpeed =
			  strtol (charPow, NULL, 10) * (MAX_FLASH - (float) MIN_FLASH) / (MAX_POWER - (float) MIN_POWER) + (MIN_FLASH - MIN_POWER);

		else if (mode == MODE_STROBE) // If we are in strobe mode, changing strobe speed instead of power
			strobeSpeed =
			  strtol (charPow, NULL, 10) * (MAX_STROBE - MIN_STROBE) / (MAX_POWER - MIN_POWER) + (MIN_STROBE - MIN_POWER);

		else if (mode == MODE_FADE) // If we are in fade mode, changing fade speed instead of power
			fadeSpeed =
			  strtol (charPow, NULL, 10) * (MAX_FADE - MIN_FADE) / (MAX_POWER - MIN_POWER) + (MIN_FADE - MIN_POWER);

		else if (mode == MODE_SMOOTH) // If we are in smooth mode, changing smooth speed instead of power
			smoothSpeed =
			  strtol (charPow, NULL, 10) * (MAX_SMOOTH - MIN_SMOOTH) / (MAX_POWER - MIN_POWER) + (MIN_SMOOTH - MIN_POWER);

		else
			power = strtol (charPow, NULL, 10);
	}
	else if (infoType == TYPE_MOD)
	{
		mode = message.charAt (0) - '0';
	}
	else if (infoType == TYPE_PRT)
	{
		for (i = 0; i < N_PRAYER && !(message.charAt (0) == prayersName[i].charAt (0)); i++)
		{ }

		message.remove (0, 1);
		prayerTime[i][2] = message.toInt();
		prayerTime[i][0] = prayerTime[i][2] / 60;
		prayerTime[i][1] = prayerTime[i][2] % 60;
	}


	if (DEBUG_ENABLED)
	{
		// [DEBUG] printing information without prefix
		Serial.print
		(
			infoType == TYPE_TIME ? "TIME: " :
			infoType == TYPE_ON ? "ON: " :
			infoType == TYPE_RGB ? "RGB: " :
			infoType == TYPE_POW ? "POW: " :
			infoType == TYPE_MOD ? "MOD: " :
			infoType == TYPE_PRT ? "PRT: " : "UNKNOWN: "
		);

		Serial.println
		(
			infoType == TYPE_ON ?
			(
				message == "1" ? "True" :
				message == "0" ? "False" : "Error"
			) :
			infoType == TYPE_MOD ?
			(
				message.charAt (0) == MODE_DEFAULT + '0' ? "DEFAULT (" :
				message.charAt (0) == MODE_FLASH + '0' ? "FLASH (" :
				message.charAt (0) == MODE_STROBE + '0' ? "STROBE (" :
				message.charAt (0) == MODE_FADE + '0' ? "FADE (" :
				message.charAt (0) == MODE_SMOOTH + '0' ? "SMOOTH (" :
				message.charAt (0) == MODE_WAKEUP + '0' ? "WAKE UP (" : "UNKNOWN ("
			) + message + ") " :
			message
		);


		// This is the end of debuging for these types, so we print \n
		if (infoType == TYPE_ON || infoType == TYPE_MOD || infoType == TYPE_UNKNOWN)
			Serial.println();
		else if (infoType == TYPE_TIME)
		{
			Serial.print ("TIME (number): ");
			Serial.println (now());
			Serial.print ("TIME (readable): ");
			digitalClockDisplay();
			Serial.println ("\n");
		}
		else if (infoType == TYPE_RGB)
		{
			rgb2color();
			Serial.print ("Full RGB (number): ");
			Serial.println (rgb, HEX);
			Serial.print ("RED: ");
			Serial.println (red);
			Serial.print ("GREEN: ");
			Serial.println (green);
			Serial.print ("BLUE: ");
			Serial.println (blue);
			Serial.println();
		}
		else if (infoType == TYPE_POW)
		{
			Serial.print ("Full POW (number): ");
			Serial.println (
				mode == MODE_FLASH ? (int) flashSpeed :
				mode == MODE_STROBE ? (int) strobeSpeed :
				mode == MODE_FADE ? (int) fadeSpeed :
				mode == MODE_SMOOTH ? (int) smoothSpeed : (int) power);
			Serial.println();
		}
		else if (infoType == TYPE_PRT)
		{
			Serial.print ("Prayer: ");
			Serial.println (prayersName[i]);
			Serial.print ("Prayer time: ");
			Serial.println (prayerTime[i][2]);
			Serial.print ("Prayer time (Readable): ");
			Serial.print (prayerTime[i][0]);
			Serial.print (":");
			Serial.println (prayerTime[i][1]);
			Serial.println();
		}
	}
} // readSerial

// Digital clock display of the time
void digitalClockDisplay ()
{
	printDigits (day());
	Serial.print ("/");
	printDigits (month());
	Serial.print ("/");
	Serial.print (year());

	Serial.print (" ");

	printDigits (hour());
	Serial.print (":");
	printDigits (minute());
	Serial.print (":");
	printDigits (second());
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
	if (mode == MODE_DEFAULT && lastMode != MODE_DEFAULT)
	{
		lastMode = MODE_DEFAULT;
		rgb      = lastRgb;
		power    = lastPower;
	}

	// If lightning if off or mode is constant lightnin, don't do anything
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
	if (DEBUG_ENABLED)
		Serial.println ("Entering Flash mode\n");
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
	if (DEBUG_ENABLED)
		Serial.println ("Entering Strobe mode\n");
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
	if (DEBUG_ENABLED)
		Serial.println ("Entering Fade mode\n");
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
	if (DEBUG_ENABLED)
		Serial.println ("Entering Smooth mode\n");
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
	if (DEBUG_ENABLED)
		Serial.println ("Entering Wakeup mode\n");
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
		Serial.println ("Leaving Wakeup mode\n");
	}
}
