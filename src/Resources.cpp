#include "Resources.h"

const char nameUnknown0[] STORAGE = "Unknown";

const char lightModeName0[] STORAGE = "Continuous";
const char lightModeName1[] STORAGE = "Flash";
const char lightModeName2[] STORAGE = "Strobe";
const char lightModeName3[] STORAGE = "Fade";
const char lightModeName4[] STORAGE = "Smooth";
const char lightModeName5[] STORAGE = "Wake up";
const char lightModeName6[] STORAGE = "Sunset";
const char lightModeName7[] STORAGE = "Start animation";
const char lightModeName8[] STORAGE = "Music";

const char soundModeName0[] STORAGE = "Free choice";

const char soundCommandName0[] STORAGE = "Play random";
const char soundCommandName1[] STORAGE = "Play one";
const char soundCommandName2[] STORAGE = "Play next";
const char soundCommandName3[] STORAGE = "Play previous";
const char soundCommandName4[] STORAGE = "Pause";
const char soundCommandName5[] STORAGE = "Resume";
const char soundCommandName6[] STORAGE = "Play dawn";

const char messageTypeName1[] STORAGE = "TIME";
const char messageTypeName2[] STORAGE = "INFO";
const char messageTypeName3[] STORAGE = "SCO";
const char messageTypeName4[] STORAGE = "TIM";
const char messageTypeName5[] STORAGE = "LON";
const char messageTypeName6[] STORAGE = "RGB";
const char messageTypeName7[] STORAGE = "POW";
const char messageTypeName8[] STORAGE = "LMO";
const char messageTypeName9[] STORAGE = "SPE";
const char messageTypeName10[] STORAGE = "SON";
const char messageTypeName11[] STORAGE = "SMO";
const char messageTypeName12[] STORAGE = "VOL";
const char messageTypeName13[] STORAGE = "DTM";

const char messageTypeDisplayName1[] STORAGE = "Time request";
const char messageTypeDisplayName2[] STORAGE = "Info request";
const char messageTypeDisplayName3[] STORAGE = "Sound command";
const char messageTypeDisplayName4[] STORAGE = "Time";
const char messageTypeDisplayName5[] STORAGE = "Light On/Off";
const char messageTypeDisplayName6[] STORAGE = "RGB";
const char messageTypeDisplayName7[] STORAGE = "Light power";
const char messageTypeDisplayName8[] STORAGE = "Light mode";
const char messageTypeDisplayName9[] STORAGE = "Light mode speed";
const char messageTypeDisplayName10[] STORAGE = "Sound On/Off";
const char messageTypeDisplayName11[] STORAGE = "Sound mode";
const char messageTypeDisplayName12[] STORAGE = "Sound volume";
const char messageTypeDisplayName13[] STORAGE = "Dawn time";

const char errorName0[] STORAGE = "No error";
const char errorName1[] STORAGE = "Out of bounds";
const char errorName2[] STORAGE = "Unknowm complement";
const char errorName3[] STORAGE = "Unknown request type";
const char errorName4[] STORAGE = "Empty String";

const FlashString nameUnknown                            = Flash (nameUnknown0);
const FlashString lightModeName[LightMode::N]            = { Flash (lightModeName0), Flash (lightModeName1), Flash (lightModeName2), Flash (lightModeName3), Flash (lightModeName4), Flash (lightModeName5), Flash (lightModeName6), Flash (lightModeName7), Flash (lightModeName8) };
const FlashString soundModeName[SoundMode::N]            = { Flash (soundModeName0) };
const FlashString soundCommandName[SoundCommand::N]      = { Flash (soundCommandName0), Flash (soundCommandName1), Flash (soundCommandName2), Flash (soundCommandName3), Flash (soundCommandName4), Flash (soundCommandName5), Flash (soundCommandName6) };
const FlashString messageTypeName[RequestType::N]        = { Flash (nameUnknown), Flash (messageTypeName1), Flash (messageTypeName2), Flash (messageTypeName3), Flash (messageTypeName4), Flash (messageTypeName5), Flash (messageTypeName6), Flash (messageTypeName7), Flash (messageTypeName8), Flash (messageTypeName9), Flash (messageTypeName10), Flash (messageTypeName11), Flash (messageTypeName12), Flash (messageTypeName13) };
const FlashString messageTypeDisplayName[RequestType::N] = { Flash (nameUnknown), Flash (messageTypeDisplayName1), Flash (messageTypeDisplayName2), Flash (messageTypeDisplayName3), Flash (messageTypeDisplayName4), Flash (messageTypeDisplayName5), Flash (messageTypeDisplayName6), Flash (messageTypeDisplayName7), Flash (messageTypeDisplayName8), Flash (messageTypeDisplayName9), Flash (messageTypeDisplayName10), Flash (messageTypeDisplayName11), Flash (messageTypeDisplayName12), Flash (messageTypeDisplayName13) };
const FlashString errorName[RequestError::N]             = { Flash (errorName0), Flash (errorName1), Flash (errorName2), Flash (errorName3), Flash (errorName4) };

const uint32_t WebcolorList[WEB_COLOR_HEIGHT][WEB_COLOR_WIDTH] STORAGE =
{
	{ FULL_LINE, 0xFFFFFF },
	{ 0xFF0000,  0xFF5500, 0xFFAA00, 0xFFFF00, 0xAAFF00, 0x55FF00},
	{ 0x00FF00,  0x00FF55, 0x00FFAA, 0x00FFFF, 0x00AAFF, 0x0055FF},
	{ 0x0000FF,  0x5500FF, 0xAA00FF, 0xFF00FF, 0xFF00AA, 0xFF0055}
};

const uint32_t IrColorList[IR_COLOR_N][3] STORAGE =
{
	// RGB Code Code 1 Code 2
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