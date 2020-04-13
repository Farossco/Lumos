#ifndef SDCARD_H
#define SDCARD_H

#if defined(LUMOS_ARDUINO_MEGA)

# include <SD.h>

const float FILE_FLUSH_TIME = 5;

const uint8_t PIN_SD_CS        = 10; // Chip Select
const uint8_t PIN_SD_CD        = 9;  // Card Detect
const uint8_t PIN_SD_LED_RED   = 3;  // RGB Red pin
const uint8_t PIN_SD_LED_GREEN = 4;  // RGB Green pin
const uint8_t PIN_SD_LED_BLUE  = 5;  // RGB Blue pin
const uint8_t SD_LED_POWER     = 255;

class SdCard
{
public:
	SdCard();
	void init ();
	void action ();

private:
	File file;
	boolean cardPresent;
	uint32_t flushTimer;
	bool enabled;

	void openCard();
	bool detectCard ();
	void lightError ();
	void lightConnected ();
	void lightIdle ();
	void lightNoCard ();
	void lightOff ();
	void autoDetect ();
	void autoFlush ();

	bool createLogFile ();
};

extern SdCard sd;

#endif // if defined(LUMOS_ARDUINO_MEGA)

#endif // ifndef SDCARD_H