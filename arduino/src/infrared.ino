#include "arduino.h"

// ******* Infrared codes and corresponding RGB code array ******* //
unsigned long color[N_COLOR][3] =
{
	// RGB Code   Code 1     Code 2
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

// ******* readInfrared ******* //
unsigned long lastIRCode; // IR code in previous loop - Allows continious power / strobe speed increasion / dicreasion
unsigned long IRCode;     // Current IR code
boolean flag;

void initInfrared ()
{
	if (!INFRARED_ENABLED)
		return;

	printlnNoPrefix(LEVEL_INFO);
	print(LEVEL_INFO, "Initializing infrared... ");

	lastIRCode = 0;

	pinMode (PIN_SOUND, INPUT); // Setting sound detector as an input
	irrecv.enableIRIn();        // Initialize IR communication

	printlnNoPrefix(LEVEL_INFO, "Done.");
}

// Read the in-comming IR signal if present
void readInfrared ()
{
	if (!INFRARED_ENABLED)
		return;

	flag = false;

	// If something is comming from IR reciever
	if (irrecv.decode (&results))
	{
		// We save the IR word in IRCode
		IRCode = results.value;

		// [DEBUG] Print the incomming IR value
		printlnNoPrefix(LEVEL_DEBUG);
		print (LEVEL_DEBUG, "Incomming IR: ");
		printNoPrefix (LEVEL_DEBUG, IRCode, HEX);

		// REPEAT (When button is pressed continiously, sent value is 0xFFFFFFFF, so we change it with the latest code that we recieved
		if (IRCode == 0xFFFFFFFF)
		{
			printNoPrefix (LEVEL_DEBUG, " (");
			printNoPrefix (LEVEL_DEBUG, lastIRCode, HEX);
			printNoPrefix (LEVEL_DEBUG, ")");

			IRCode = lastIRCode;
		}
			
		// ON
		if (IRCode == 0xFFB04F || IRCode == 0xF0C41643)
		{
			on         = true;
			lastIRCode = 0; // We don't save value in lastIRCode because don't care if we keep on button pressed

			printlnNoPrefix(LEVEL_INFO);
			println (LEVEL_INFO, "Switch ON");
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
				printlnNoPrefix(LEVEL_INFO);
				println (LEVEL_INFO, "Switch OFF");
				break;
				break;

			// DOWN
			case 0xFFB847:
			case 0xA23C94BF:
				if (power[mode] - IR_CHANGE_STEP >= MIN_IR_POWER)
					power[mode] -= IR_CHANGE_STEP;
				else
					power[mode] = MIN_IR_POWER;

				rgb2color();

				// [DEBUG] Print current color and RED, GREEN, BLUE values
				printlnNoPrefix(LEVEL_DEBUG);
				print (LEVEL_DEBUG, "Power (" + modeName (mode, CAPS_ALL) + " mode): ");
				printlnNoPrefix (LEVEL_DEBUG, power[mode], DEC);
				print (LEVEL_DEBUG, "RED: (" + modeName (mode, CAPS_FIRST) + " mode): ");
				printNoPrefix (LEVEL_DEBUG, red[mode], DEC);
				printNoPrefix (LEVEL_DEBUG, " / GREEN: ");
				printNoPrefix (LEVEL_DEBUG, green[mode], DEC);
				printNoPrefix (LEVEL_DEBUG, " / BLUE: ");
				printlnNoPrefix (LEVEL_DEBUG, blue[mode], DEC);

				lastIRCode = IRCode;

				break;
				break;

			// UP
			case 0xFF906F:
			case 0xE5CFBD7F:
				if (power[mode] + IR_CHANGE_STEP <= MAX_POWER)
					power[mode] += IR_CHANGE_STEP;
				else
					power[mode] = MAX_POWER;

				rgb2color();

				// [DEBUG] Print current color and RED, GREEN, BLUE values
				printlnNoPrefix(LEVEL_DEBUG);
				print (LEVEL_DEBUG, "Power (" + modeName (mode, CAPS_FIRST) + " mode): ");
				printlnNoPrefix (LEVEL_DEBUG, power[mode], DEC);
				print (LEVEL_DEBUG, "RED: (" + modeName (mode, CAPS_FIRST) + " mode): ");
				printNoPrefix (LEVEL_DEBUG, red[mode], DEC);
				printNoPrefix (LEVEL_DEBUG, " / GREEN: ");
				printNoPrefix (LEVEL_DEBUG, green[mode], DEC);
				printNoPrefix (LEVEL_DEBUG, " / BLUE: ");
				printlnNoPrefix (LEVEL_DEBUG, blue[mode], DEC);

				lastIRCode = IRCode;

				break;
				break;

			// FLASH
			case 0xFFB24D:
			case 0x7EC31EF7:
				mode = MODE_FLASH;
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
				for (int i = 0; i < N_COLOR; i++)
					if (results.value == color[i][1] || results.value == color[i][2])
					{
						mode              = MODE_DEFAULT;
						rgb[MODE_DEFAULT] = color[i][0];
					}
				break;
		}
		irrecv.resume();
	}
} // readInfrared