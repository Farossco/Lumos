#ifndef RESOURCES_H
#define RESOURCES_H

#include <Arduino.h>
#include "Types.h"

#define STORE_IN_PROGMEM true

#if (STORE_IN_PROGMEM)
# define StringType __FlashStringHelper *
# define Flash(x) reinterpret_cast<const StringType>(x)
# define STORAGE PROGMEM
#else
# define StringType char *
# define Flash(x) x
# define STORAGE
#endif

#define WEB_COLOR_WIDTH  6 // The width of the web interface's color panel
#define WEB_COLOR_HEIGHT 4 // The height of the web interface's color panel

#define IR_COLOR_N       16 // Number of different colors

#define FULL_LINE        0x000000 // TODO : get rid of that

extern const StringType nameUnknown;
extern const StringType lightModeName[LightMode::N];
extern const StringType soundModeName[SoundMode::N];
extern const StringType soundCommandName[SoundCommand::N];
extern const StringType messageTypeName[RequestType::N];
extern const StringType messageTypeDisplayName[RequestType::N];
extern const StringType errorName[RequestError::N];

extern const uint32_t WebcolorList[WEB_COLOR_HEIGHT][WEB_COLOR_WIDTH];

extern const uint32_t IrColorList[IR_COLOR_N][3];

#endif // ifndef RESOURCES_H