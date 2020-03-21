#if defined(LUMOS_ARDUINO_MEGA)
#include "Infrared.h"
#include "Light.h"
#include "Logger.h"

Infrared::Infrared() : irrecv (0)
{
	enabled    = false;
	lastIRCode = 0;
	IRCode     = 0;
}

void Infrared::init (int pin)
{
	pinMode (pin, INPUT);

	irrecv = IRrecv (pin);

	Log.info ("Initializing infrared... ");

	irrecv.enableIRIn(); // Initialize IR communication

	Log.infonp ("Done." dendl);

	enabled = true;
}

// Read the in-comming IR signal if present
void Infrared::read ()
{
	if (!enabled)
		return;

	// If something is comming from IR reciever
	if (irrecv.decode (&results))
	{
		// We save the IR word in IRCode
		IRCode = results.value;

		// [DEBUG] Print the incomming IR value
		Log.trace ("Incomming IR: %X", IRCode);

		// REPEAT (When button is pressed continiously, sent value is 0xFFFFFFFF, so we change it with the latest code that we recieved
		if (IRCode == 0xFFFFFFFF)
		{
			Log.tracenp (" (%X)", lastIRCode);

			IRCode = lastIRCode;
		}

		Log.tracenp (dendl);

		// ON
		if (IRCode == 0xFFB04F || IRCode == 0xF0C41643)
		{
			light.switchOn();
			lastIRCode = 0; // We don't save value in lastIRCode because we don't care if we keep on button pressed

			Log.trace ("Switch ON" dendl);
		}

		// If the system is off, ignore incomming infrared (Except ON of course, it is just above)
		if (light.isOff())
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
				light.switchOff();
				lastIRCode = 0;
				Log.trace ("Switch OFF" dendl);
				break;
				break;

			// DOWN
			case 0xFFB847:
			case 0xA23C94BF:
				light.subtractPower (IR_CHANGE_STEP);

				// [DEBUG] Prints current color and RED, GREEN, BLUE values
				Log.trace ("Power (%s mod): %d" endl, utils.lightModName (light.getMod(), CAPS_ALL), light.getPower());
				Log.trace ("RED: %d / GREEN : %d / BLUE: %d" dendl, light.getRed(), light.getGreen(), light.getBlue());

				lastIRCode = IRCode;

				break;
				break;
			// UP
			case 0xFF906F:
			case 0xE5CFBD7F:
				light.addPower (IR_CHANGE_STEP);

				// [DEBUG] Prints current color and RED, GREEN, BLUE values
				Log.trace ("Power (%s mod): %d" endl, utils.lightModName (light.getMod(), CAPS_ALL), light.getPower());
				Log.trace ("RED: %d / GREEN : %d / BLUE: %d" dendl, light.getRed(), light.getGreen(), light.getBlue());

				lastIRCode = IRCode;

				break;
				break;

			// FLASH
			case 0xFFB24D:
			case 0x7EC31EF7:
				light.setMod (LIGHT_MOD_FLASH);
				break;
				break;

			// STROBE
			case 0xFF00FF:
			case 0xFA3F159F:
				light.setMod (LIGHT_MOD_STROBE);
				break;
				break;

			// FADE
			case 0xFF58A7:
			case 0xDC0197DB:
				light.setMod (LIGHT_MOD_FADE);
				break;
				break;

			// SMOOTH
			case 0xFF30CF:
			case 0x9716BE3F:
				light.setMod (LIGHT_MOD_SMOOTH);
				break;
				break;

			// COLORS
			default:
				lastIRCode = 0;
				for (int i = 0; i < N_COLOR; i++)
					if (results.value == color[i][1] || results.value == color[i][2])
					{
						light.setMod (LIGHT_MOD_CONTINUOUS);
						light.setRgb (color[i][0], LIGHT_MOD_CONTINUOUS);
					}
				break;
		}
		irrecv.resume();
	}
} // readInfrared

Infrared infrared = Infrared();

#endif // if defined(LUMOS_ARDUINO_MEGA)