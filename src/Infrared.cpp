#if defined(LUMOS_ARDUINO_MEGA)
#include "Infrared.h"
#include "Light.h"
#include "ArduinoLogger.h"

Infrared::Infrared() : irrecv (0)
{
	enabled    = false;
	lastIRCode = 0;
	IRCode     = 0;
}

void Infrared::init (int pin)
{
	inf << "Initializing infrared... ";

	pinMode (pin, INPUT);

	irrecv = IRrecv (pin);

	irrecv.enableIRIn(); // Initialize IR communication

	enabled = true;

	inf << "Done." << dendl;
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
		trace << "Incomming IR: 0x" << utils.ltos (IRCode, HEX);
		// REPEAT (When button is pressed continiously, sent value is 0xFFFFFFFF, so we change it with the latest code that we recieved
		if (IRCode == 0xFFFFFFFF)
		{
			trace << " (0x" << utils.ltos (lastIRCode, HEX) << ")";
			IRCode = lastIRCode;
		}
		trace << dendl;

		// ON
		if (IRCode == 0xFFB04F || IRCode == 0xF0C41643)
		{
			light.switchOn();
			lastIRCode = 0; // We don't save value in lastIRCode because we don't care if we keep on button pressed

			trace << "Switch ON" << dendl;
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
				trace << "Switch OFF" << dendl;
				break;
				break;

			// DOWN
			case 0xFFB847:
			case 0xA23C94BF:
				light.subtractPower (IR_CHANGE_STEP);

				// [DEBUG] Prints current color and RED, GREEN, BLUE values
				trace << "Power (" << utils.getLightModeName (light.getMode()) << " mode): " << light.getPowerRaw() << " (" << light.getPowerPercent() << "%)" << endl;
				trace << "RED: " << light.getRed() << " / GREEN : " << light.getGreen() << " / BLUE: " << light.getBlue() << dendl;

				lastIRCode = IRCode;

				break;
				break;
			// UP
			case 0xFF906F:
			case 0xE5CFBD7F:
				light.addPower (IR_CHANGE_STEP);

				// [DEBUG] Prints current color and RED, GREEN, BLUE values
				trace << "Power (" << utils.getLightModeName (light.getMode()) << " mode): " << light.getPowerRaw() << " (" << light.getPowerPercent() << "%)" << endl;
				trace << "RED: " << light.getRed() << " / GREEN : " << light.getGreen() << " / BLUE: " << light.getBlue() << dendl;

				lastIRCode = IRCode;

				break;
				break;

			// FLASH
			case 0xFFB24D:
			case 0x7EC31EF7:
				light.setMode (LightMode::flash);
				break;
				break;

			// STROBE
			case 0xFF00FF:
			case 0xFA3F159F:
				light.setMode (LightMode::strobe);
				break;
				break;

			// FADE
			case 0xFF58A7:
			case 0xDC0197DB:
				light.setMode (LightMode::fade);
				break;
				break;

			// SMOOTH
			case 0xFF30CF:
			case 0x9716BE3F:
				light.setMode (LightMode::smooth);
				break;
				break;

			// COLORS
			default:
				lastIRCode = 0;
				for (int i = 0; i < N_COLOR; i++)
					if (results.value == color[i][1] || results.value == color[i][2])
					{
						light.setMode (LightMode::continuous);
						light.setRgb (color[i][0]);
					}
				break;
		}
		irrecv.resume();
	}
} // readInfrared

Infrared infrared = Infrared();

#endif // if defined(LUMOS_ARDUINO_MEGA)