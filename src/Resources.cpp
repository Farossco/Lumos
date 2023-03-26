#include "Resources.h"

const char nameUnknown0[] STORAGE = "Unknown";

const char lightModeName0[] STORAGE = "Continuous";
const char lightModeName1[] STORAGE = "Flash";
const char lightModeName2[] STORAGE = "Strobe";
const char lightModeName3[] STORAGE = "Fade";
const char lightModeName4[] STORAGE = "Smooth";
const char lightModeName5[] STORAGE = "Wake up";
const char lightModeName6[] STORAGE = "Sunset";
const char lightModeName7[] STORAGE = "Music";

const char soundModeName0[] STORAGE = "Free choice";

const char soundCommandName0[] STORAGE = "Play Random";
const char soundCommandName1[] STORAGE = "Play One";
const char soundCommandName2[] STORAGE = "Play Next";
const char soundCommandName3[] STORAGE = "Play Previous";
const char soundCommandName4[] STORAGE = "Pause";
const char soundCommandName5[] STORAGE = "Resume";
const char soundCommandName6[] STORAGE = "Play Dawn";

const char messageTypeName1[] STORAGE = "RQT";
const char messageTypeName2[] STORAGE = "RQD";
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
const char messageTypeName13[] STORAGE = "DVO";
const char messageTypeName14[] STORAGE = "DTM";
const char messageTypeName15[] STORAGE = "DDU";
const char messageTypeName16[] STORAGE = "SDU";
const char messageTypeName17[] STORAGE = "SDT";

const char messageTypeDisplayName1[] STORAGE = "Time Tequest";
const char messageTypeDisplayName2[] STORAGE = "Data Request";
const char messageTypeDisplayName3[] STORAGE = "Sound Command";
const char messageTypeDisplayName4[] STORAGE = "Time";
const char messageTypeDisplayName5[] STORAGE = "Light On/Off";
const char messageTypeDisplayName6[] STORAGE = "Light Mode RGB";
const char messageTypeDisplayName7[] STORAGE = "Light Mode Power";
const char messageTypeDisplayName8[] STORAGE = "Light Mode";
const char messageTypeDisplayName9[] STORAGE = "Light Mode Speed";
const char messageTypeDisplayName10[] STORAGE = "Sound On/Off";
const char messageTypeDisplayName11[] STORAGE = "Sound Mode";
const char messageTypeDisplayName12[] STORAGE = "Sound Volume";
const char messageTypeDisplayName13[] STORAGE = "Dawn Volume";
const char messageTypeDisplayName14[] STORAGE = "Dawn Time";
const char messageTypeDisplayName15[] STORAGE = "Dawn Duration";
const char messageTypeDisplayName16[] STORAGE = "Sunset Duration";
const char messageTypeDisplayName17[] STORAGE = "Sunset Decrease Time";

const char errorName0[] STORAGE = "No error";
const char errorName1[] STORAGE = "Incorrect Value";
const char errorName2[] STORAGE = "Incorrect Complement";
const char errorName3[] STORAGE = "Incorrect Request Type";
const char errorName4[] STORAGE = "Empty String";

const StringType nameUnknown                            = Flash(nameUnknown0);
const StringType lightModeName[LightMode::N]            = { Flash(lightModeName0), Flash(lightModeName1), Flash(lightModeName2), Flash(lightModeName3), Flash(lightModeName4), Flash(lightModeName5), Flash(lightModeName6), Flash(lightModeName7) };
const StringType soundModeName[SoundMode::N]            = { Flash(soundModeName0) };
const StringType soundCommandName[SoundCommand::N]      = { Flash(soundCommandName0), Flash(soundCommandName1), Flash(soundCommandName2), Flash(soundCommandName3), Flash(soundCommandName4), Flash(soundCommandName5), Flash(soundCommandName6) };
const StringType messageTypeName[RequestType::N]        = { Flash(nameUnknown), Flash(messageTypeName1), Flash(messageTypeName2), Flash(messageTypeName3), Flash(messageTypeName4), Flash(messageTypeName5), Flash(messageTypeName6), Flash(messageTypeName7), Flash(messageTypeName8), Flash(messageTypeName9), Flash(messageTypeName10), Flash(messageTypeName11), Flash(messageTypeName12), Flash(messageTypeName13), Flash(messageTypeName14), Flash(messageTypeName15), Flash(messageTypeName16), Flash(messageTypeName17) };
const StringType messageTypeDisplayName[RequestType::N] = { Flash(nameUnknown), Flash(messageTypeDisplayName1), Flash(messageTypeDisplayName2), Flash(messageTypeDisplayName3), Flash(messageTypeDisplayName4), Flash(messageTypeDisplayName5), Flash(messageTypeDisplayName6), Flash(messageTypeDisplayName7), Flash(messageTypeDisplayName8), Flash(messageTypeDisplayName9), Flash(messageTypeDisplayName10), Flash(messageTypeDisplayName11), Flash(messageTypeDisplayName12), Flash(messageTypeDisplayName13), Flash(messageTypeDisplayName14), Flash(messageTypeDisplayName15), Flash(messageTypeDisplayName16), Flash(messageTypeDisplayName17) };
const StringType errorName[RequestError::N]             = { Flash(errorName0), Flash(errorName1), Flash(errorName2), Flash(errorName3), Flash(errorName4) };

const uint32_t WebcolorList[WEB_COLOR_HEIGHT][WEB_COLOR_WIDTH] STORAGE =
{
	{ FULL_LINE, 0xFFFFFF },
	{ 0xFF0000,  0xFF5500, 0xFFAA00, 0xFFFF00, 0xAAFF00, 0x55FF00},
	{ 0x00FF00,  0x00FF55, 0x00FFAA, 0x00FFFF, 0x00AAFF, 0x0055FF},
	{ 0x0000FF,  0x5500FF, 0xAA00FF, 0xFF00FF, 0xFF00AA, 0xFF0055}
};

const uint32_t IrColorList[IR_COLOR_N][3] STORAGE =
{
	/* RGB Code  Code 1     Code 2  */
	{ 0xFFFFFF, 0xFFA857, 0xA3C8EDDB }, /* WHITE */

	{ 0xFF0000, 0xFF9867, 0x97483BFB }, /* R1 */
	{ 0xFF5300, 0xFFE817, 0x5BE75E7F }, /* R2 */
	{ 0xFF3C00, 0xFF02FD, 0xD7E84B1B }, /* R3 */
	{ 0xFFC400, 0xFF50AF, 0x2A89195F }, /* R4 */
	{ 0xFFFF00, 0xFF38C7, 0x488F3CBB }, /* R5 */

	{ 0x00FF00, 0xFFD827, 0x86B0E697 }, /* G1 */
	{ 0x20FF5D, 0xFF48B7, 0xF377C5B7 }, /* G2 */
	{ 0x34FFF6, 0xFF32CD, 0xEE4ECCFB }, /* G3 */
	{ 0x21E7FF, 0xFF7887, 0xF63C8657 }, /* G4 */
	{ 0x00BDFF, 0xFF28D7, 0x13549BDF }, /* G5 */

	{ 0x0000FF, 0xFF8877, 0x9EF4941F }, /* B1 */
	{ 0x0068FF, 0xFF6897, 0xC101E57B }, /* B2 */
	{ 0x8068FF, 0xFF20DF, 0x51E43D1B }, /* B3 */
	{ 0xDB89FF, 0xFF708F, 0x44C407DB }, /* B4 */
	{ 0xFF7B92, 0xFFF00F, 0x35A9425F }, /* B5 */
};
