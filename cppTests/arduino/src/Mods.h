#ifndef MODS_H
#define MODS_H

class Mods
{
public:

	void init ();
	void action (); // Perform mod action

	void initFlash (); // Flash mod initialization
	void flash ();     // Flash mod

	void initStrobe (); // Strobe mod initialization
	void strobe ();     // Strobe mod

	void initFade (); // Fade mod initialization
	void fade ();     // Fade mod

	void initSmooth (); // Smooth mod Initialization
	void smooth ();     // Smooth mod

	void initDawn (); // Wakeup mod initialization
	void dawn ();     // Wakeup mod

private:
	int state;             // Current state used by some mods
	unsigned long count;   // Delay counting
	unsigned char lastMod; // Mod in previous loop - Allows mod initializations
};

extern Mods mods;

#endif // ifndef MODS_H
