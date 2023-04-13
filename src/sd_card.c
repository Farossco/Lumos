#if 0

#include "sd_card.h"
#include <SD.h>
#include "esp_log.h"
#include "temp_log_util.h"

const uint8_t PIN_SD_CS        = 10; /* Chip Select */
const uint8_t PIN_SD_CD        = 9;  /* Card Detect */
const uint8_t PIN_SD_LED_RED   = 3;  /* RGB Red pin */
const uint8_t PIN_SD_LED_GREEN = 4;  /* RGB Green pin */
const uint8_t PIN_SD_LED_BLUE  = 5;  /* RGB Blue pin */
const uint8_t SD_LED_POWER     = 255;
const uint8_t SD_ATTEMPTS      = 3; /* The number of attempts at opening the card */

static const char *TAG = "sd_card";

static bool cardDetected;
static bool cardOpened;
static uint8_t attempts;
static File file;

static void sd_card_open()
{
	attempts++;

	ESP_LOGI(TAG, "Opening card (Attempt n°%d)... ", attempts);

	if (SD.begin(PIN_SD_CS)) { /* Card successfully opened */
		attempts = 0;

		ESP_LOGI(TAG, "Done", attempts);
	} else { /* Card failed to opened */
		if (attempts < SD_ATTEMPTS && sd_card_detect()) {
			ESP_LOGI(TAG, "Reattempting...", attempts);
			sd_card_open();
		} else {
			ESP_LOGI(TAG, "SD card opening failed!");
			attempts = 0;
		}
	}
}

static void sd_card_close()
{
	ESP_LOGI(TAG, "Closing card");

	SD.end();

	cardOpened = false;

	ESP_LOGI(TAG, "Done", attempts);
}

static boolean sd_card_detect()
{
	return cardDetected = digitalRead(PIN_SD_CD);
}

void static sd_card_auto_detect()
{
	if (cardDetected) {          /* If card was present */
		if (!sd_card_detect()) { /* But has been removed */
			ESP_LOGD(TAG, "SD card removed");
			sd_card_close();
		}
	} else {                    /* If card was absent */
		if (sd_card_detect()) { /* And a card is detected */
			ESP_LOGI(TAG, "SD card detected");
			delay(1000); /* Waiting to make sure the card is properly connected */

			sd_card_open();
		}
	}
}

void sd_card_action()
{
	sd_card_auto_detect();

	if (cardOpened)
		file.flush();
}

void sd_card_init()
{
	pinMode(PIN_SD_LED_RED,   OUTPUT);
	pinMode(PIN_SD_LED_GREEN, OUTPUT);
	pinMode(PIN_SD_LED_BLUE,  OUTPUT);
	pinMode(PIN_SD_CS, INPUT);
	pinMode(PIN_SD_CD, INPUT_PULLUP);

	cardOpened   = false;
	cardDetected = false;
	attempts     = 0;

	if (sd_card_detect()) {
		sd_card_open();
	} else {
		ESP_LOGW(TAG, "SD card undetected, no SD logging");
	}
}

#endif /* if 0 */
