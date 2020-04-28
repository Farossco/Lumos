#ifndef RESOURCES_H
#define RESOURCES_H

#include <Arduino.h>
#include "Types.h"

#define STORE_IN_PROGMEM true

#if (STORE_IN_PROGMEM)
# define FlashString __FlashStringHelper *
# define Flash(x) reinterpret_cast<const FlashString>(x)
# define STORAGE     PROGMEM
#else
# define FlashString char *
# define Flash(x) x
# define STORAGE
#endif

#define WEB_COLOR_WIDTH  6 // The width of the web interface's color panel
#define WEB_COLOR_HEIGHT 4 // The height of the web interface's color panel

#define IR_COLOR_N       16 // Number of different colors

#define FULL_LINE        0x000000 // TODO : get rid of that

extern const FlashString nameUnknown;
extern const FlashString lightModeName[LightMode::N];
extern const FlashString soundModeName[SoundMode::N];
extern const FlashString soundCommandName[SoundCommand::N];
extern const FlashString messageTypeName[RequestType::N];
extern const FlashString messageTypeDisplayName[RequestType::N];
extern const FlashString errorName[RequestError::N];

extern const uint32_t WebcolorList[WEB_COLOR_HEIGHT][WEB_COLOR_WIDTH];

extern const uint32_t IrColorList[IR_COLOR_N][3];

#endif // ifndef RESOURCES_H