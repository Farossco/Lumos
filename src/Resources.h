#ifndef RESOURCES_H
#define RESOURCES_H

#include <Arduino.h>
#include "Types.h"

#define STORE_IN_PROGMEM false

#if (STORE_IN_PROGMEM)
# define StringType __FlashStringHelper *
# define Flash(x) reinterpret_cast<const StringType>(x)
# define STORAGE    PROGMEM
#else /* if (STORE_IN_PROGMEM) */
# define StringType char *
# define Flash(x) x
# define STORAGE
#endif /* if (STORE_IN_PROGMEM) */

#define IR_COLOR_N 16 /* Number of different colors */

extern const StringType nameUnknown;
extern const StringType soundModeName[SoundMode::N];
extern const StringType soundCommandName[7];
extern const StringType messageTypeName[RequestType::N];
extern const StringType messageTypeDisplayName[RequestType::N];
extern const StringType errorName[RequestError::N];

extern const uint32_t IrColorList[IR_COLOR_N][3];

#endif // ifndef RESOURCES_H
