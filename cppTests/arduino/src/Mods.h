#ifndef MODS_H
#define MODS_H

class Mods
{
public:

	void init ();
	void action (); // Perform mode action

	void initFlash (); // Flash mode initialization
	void flash ();     // Flash mode

	void initStrobe (); // Strobe mode initialization
	void strobe ();     // Strobe mode

	void initFade (); // Fade Mode initialization
	void fade ();     // Fade Mode

	void initSmooth (); // Smooth Mode Initialization
	void smooth ();     // Smooth Mode

	void initDawn (); // Wakeup Mode initialization
	void dawn ();     // Wakeup Mode

private:
	int state;              // Current state used by some modes
	unsigned long count;    // Delay counting
	unsigned char lastMode; // Mode in previous loop - Allows mode initializations
};

extern Mods mods;

#endif // ifndef MODS_H
