#include <IRremote.h>
#define LEDRED 8 // Red LED OUT pin
#define LEDGREEN 9 // Green LED OUT pin
#define LEDBLUE 10 // Blue LED OUT pin
#define INFRARED 11 // Infrared IN pin

#define DEBUG false // DEBUG Mode

#define MINPOWER 5 // Minimum power value
#define MAXPOWER 100 // Maximum power value
#define POWERSPEED 5 // Power increasion or dicreasion speed
#define FADESPEED 1 // Fade increasion or dicreasion speed
#define N 16 // Number of different colors
#define MINSTROBE 2 // Minimum Strobe speed
#define MAXSTROBE 50 // Maximum Strobe speed
#define STROBESPEED 1 // Strobe increasion or dicreasion speed

#define TYPERGB 3
#define TYPEON 2

// Infrared reception objects declaration
IRrecv irrecv(INFRARED);
decode_results results;

// Infrared codes and corresponding RGB code array
long color[][3] =
  //RGB Code //Code 1  //Code 2
{ {0xFFFFFF, 0xFFA857, 0xA3C8EDDB}, //WHITE

  {0xFF0000, 0xFF9867, 0x97483BFB}, //R1
  {0xFF5300, 0xFFE817, 0x5BE75E7F}, //R2
  {0xFF3C00, 0xFF02FD, 0xD7E84B1B}, //R3
  {0xFFC400, 0xFF50AF, 0x2A89195F}, //R4
  {0xFFFF00, 0xFF38C7, 0x488F3CBB}, //R5

  {0x00FF00, 0xFFD827, 0x86B0E697}, //G1
  {0x20FF5D, 0xFF48B7, 0xF377C5B7}, //G2
  {0x34FFF6, 0xFF32CD, 0xEE4ECCFB}, //G3
  {0x21E7FF, 0xFF7887, 0xF63C8657}, //G4
  {0x00BDFF, 0xFF28D7, 0x13549BDF}, //G5

  {0x0000FF, 0xFF8877, 0x9EF4941F}, //B1
  {0x0068FF, 0xFF6897, 0xC101E57B}, //B2
  {0x8068FF, 0xFF20DF, 0x51E43D1B}, //B3
  {0xDB89FF, 0xFF708F, 0x44C407DB}, //B4
  {0xFF7B92, 0xFFF00F, 0x35A9425F}  //B5
};

boolean on; // If the leds are ON or OFF (True: ON / False: OFF)
int lastMode; // Mode in previous loop - Allows mode initializations
int mode; // Current lighting mode (0: Constant lightning / 1: Flash / 2: Strobe / 3: Fade / 4: Smooth)
int count; // Strobe delay counting
int strobeSpeed; // Current Strobe speed (From MINSTROBE to MAXSTROBE)
unsigned long RGB; // Currently displayed RGB value (From 0x000000 to 0xFFFFFF)
int RED; // Currentlty red value including lightning power (From 0 to 255)
int unpoweredRED; // Currentlty red value without lightning power (From 0 to 255)
int GREEN; // Currentlty green value including lightning power (From 0 to 255)
int unpoweredGREEN; // Currentlty green value without lightning power (From 0 to 255)
int BLUE; // Currentlty blue value including lightning power (From 0 to 255)
int unpoweredBLUE; // Currentlty blue value without lightning power (From 0 to 255)
int state; // Current state used by 1 to 4 modes
int i; // Counting variable (used for "for" statements)
float power; // Current lightning power (from MINPOWER to MAXPOWER)
long lastIRCode; // IR code in previous loop - Allows continious power / strobe speed increasion / dicreasion
long IRCode; // Current IR code
byte buff[20];
int wordLength;

void setup()
{
  // pins initialization
  pinMode(LEDRED, OUTPUT);
  pinMode(LEDGREEN, OUTPUT);
  pinMode(LEDBLUE, OUTPUT);
  pinMode(INFRARED, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200); // Initialize serial communication
  on = false; // LEDs are off on startup
  irrecv.enableIRIn(); // Initialize IR communication
  RGB = 0xFFFFFF; // Initialize color to white
  mode = 0; // Initialize mode to constant lightning
  power = 30; // Initializing power to 10
  // Initializing all other variables to 0
  lastMode = 0;
  state = 0;
  lastIRCode = 0;
  IRCode = 0;
  count = 0;
  strobeSpeed = 25;
}

void loop()
{
  readInfrared(); // Read the in-comming IR signal if present

  readSerial();

  action(); // Do something according to current mode

  light(); // Finaly display the RGB value
}

// Display the RGB value
void light()
{
  rgb2color();

  if (on) // If it's on
  {
    // Envoi un signal carré avec un rapport cyclique de RED / 255
    for (i = 0; i < RED * 1; i++) digitalWrite(LEDRED, LOW);
    for (i = 0; i < (255 - RED) * 1; i++) digitalWrite(LEDRED, HIGH);

    // Envoi un signal carré avec un rapport cyclique de GREEN / 255
    for (i = 0; i < GREEN * 1; i++) digitalWrite(LEDGREEN, LOW);
    for (i = 0; i < (255 - GREEN) * 1; i++) digitalWrite(LEDGREEN, HIGH);

    // Envoi un signal carré avec un rapport cyclique de BLUE / 255
    for (i = 0; i < BLUE * 1; i++) digitalWrite(LEDBLUE, LOW);
    for (i = 0; i < (255 - BLUE) * 1; i++) digitalWrite(LEDBLUE, HIGH);
  }
  else // If it's off
  {
    // We shut down all the leds (HIGH = off / LOW = on)
    digitalWrite(LEDRED, HIGH);
    digitalWrite(LEDGREEN, HIGH);
    digitalWrite(LEDBLUE, HIGH);
  }
}

// Convert RGB value to Red, Green and Blue values
void rgb2color()
{
  // Lower is the power, lower is the color value - It allows you to control LEDs light power
  RED   = ((RGB & 0xFF0000) >> 16) * (power / MAXPOWER);
  GREEN = ((RGB & 0x00FF00) >> 8)  * (power / MAXPOWER);
  BLUE  =  (RGB & 0x0000FF)        * (power / MAXPOWER);

  // Color values without the power
  unpoweredRED   = ((RGB & 0xFF0000) >> 16);
  unpoweredGREEN = ((RGB & 0x00FF00) >> 8);
  unpoweredBLUE  =  (RGB & 0x0000FF);
}

// Read the in-comming IR signal if present
void readInfrared()
{
  if (irrecv.decode(&results))
  {
    IRCode = results.value;

    // REPEAT
    if (IRCode == 0xFFFFFFFF)
      IRCode = lastIRCode;

    if (IRCode == 0xFFB04F || IRCode == 0xF0C41643)
    {
      on = true;
      lastIRCode = 0;
    }

    if (on)
    {
      switch (IRCode)
      {
        //OFF
        case 0xFFF807:
        case 0xE721C0DB:
          on = false;
          lastIRCode = 0;
          break;
          break;

        //DOWN
        case 0xFFB847:
        case 0xA23C94BF:
          if (mode == 2)
          {
            if (strobeSpeed - STROBESPEED >= MINSTROBE) strobeSpeed -= STROBESPEED;
            else strobeSpeed = MINSTROBE;
          }
          else
          {
            if (power - POWERSPEED >= MINPOWER) power -= POWERSPEED;
            else power = MINPOWER;
          }

          rgb2color();
          lastIRCode = IRCode;
          if (DEBUG)
          {
            Serial.print("Power: ");
            Serial.println(power);
            Serial.print("RED: ");
            Serial.print(RED);
            Serial.print(" / GREEN: ");
            Serial.print(GREEN);
            Serial.print(" / BLUE: ");
            Serial.println(BLUE);
          }
          break;
          break;

        //UP
        case 0xFF906F:
        case 0xE5CFBD7F:
          if (mode == 2)
          {
            if (strobeSpeed + STROBESPEED <= MAXSTROBE) strobeSpeed += STROBESPEED;
            else strobeSpeed = MAXSTROBE;
          }
          else
          {
            if (power + POWERSPEED <= MAXPOWER) power += POWERSPEED;
            else power = MAXPOWER;
          }
          rgb2color();
          lastIRCode = IRCode;
          if (DEBUG)
          {
            Serial.print("Power: ");
            Serial.println(power);
            Serial.print("RED: ");
            Serial.print(RED);
            Serial.print(" / GREEN: ");
            Serial.print(GREEN);
            Serial.print(" / BLUE: ");
            Serial.println(BLUE);
          }

          break;
          break;

        //STROBE
        case 0xFF00FF:
        case 0xFA3F159F:
          mode = 2;
          break;
          break;

        //FADE
        case 0xFF58A7:
        case 0xDC0197DB:
          mode = 3;
          break;
          break;


        //SMOOTH
        case 0xFF30CF:
        case 0x9716BE3F:
          mode = 4;
          break;
          break;

        //COLORS
        default:
          lastIRCode = 0;
          for (i = 0; i < N; i++)
            if (results.value == color[i][1] || results.value == color[i][2])
            {
              mode = 0;
              lastMode = 0;
              RGB = color[i][0];
            }
          break;
      }
    }
    if (DEBUG)
    {
      Serial.print("Incomming IR: ");
      Serial.println(IRCode, HEX);
    }

    irrecv.resume();
  }
}

void readSerial()
{
  if (!Serial.available()) return;

  boolean flag;
  int infoType, i, n;
  char charArray[20];
  char charRgb[7];
  String str;

  for (n = 0; Serial.available() && n < 20; n++)
  {
    charArray[n] = Serial.read();
    //delay(1);
  }

  for (i = 0; i < n; i++)
    str += String(charArray[i]);
  /*
    flag = false;

      for (i = 0; i < str.length(); i++)
        if (str.charAt(i) < '0' || str.charAt(i) > '9' && str.charAt(i) < 'A' || str.charAt(i) > 'Z' && str.charAt(i) < 'a' || str.charAt(i) > 'z')
          return;

      for (i = 0; i < str.length(); i++)
        if (str.charAt(i) >= '0' && str.charAt(i) <= '9' || str.charAt(i) >= 'A' && str.charAt(i) <= 'Z')
          flag = true;

      if (!flag) return;
  */

  if (str.indexOf("ON") != -1)
    infoType = TYPEON;
  else if (str.indexOf("RGB") != -1)
    infoType = TYPERGB;
  else
    return;

  if (infoType == TYPEON && str.length() != 3)
    return;
  if (infoType == TYPERGB && (str.length() < 3 || str.length() > 9))
    return;

  if (DEBUG)
  {
    Serial.print("Word: ");
    Serial.println(str);
    Serial.print("Length: ");
    Serial.println(str.length());
    Serial.print("Type: ");
    Serial.println(infoType == TYPEON ? "ON" : "RGB");
  }

  str.remove(0, infoType); // Remove 2 first characters if "ON" type and 3 first ones if "RGB" type

  if (DEBUG)
  {
    Serial.print(infoType == TYPEON ? "ON: " : "RGB: ");
    Serial.println(infoType == TYPERGB ? str : str == "1" ? "True" : str == "0" ? "False" : "Error");
    if (infoType == TYPEON)
      Serial.println();
  }

  if (infoType == TYPERGB)
    while (str.length() < 6)
      str = "0" + str;

  if (DEBUG)
  {
    if (infoType == TYPERGB)
    {
      Serial.print("Full RGB: ");
      Serial.println(str);
    }
  }

  if  (infoType == TYPEON)
  {
    on = str == "1" ? true : false;
  }
  else
  {
    str.toCharArray(charRgb, 7);
    RGB = strtol(charRgb, NULL, 16);
  }

  if (DEBUG)
  {
    if (infoType == TYPERGB)
    {
      Serial.print("Full RGB (number): ");
      Serial.println(RGB, HEX);
      Serial.println();
    }
  }
}

void action()
{
  if (!on || mode == 0)
  {
    return;
  }

  switch (mode)
  {
    case 2:
      if (lastMode != 2)
        initStrobe();
      STROBE();
      break;

    case 3:
      if (lastMode != 3)
        initFade();
      FADE();
      break;

    case 4:
      if (lastMode != 4)
        initSmooth();
      SMOOTH();
      break;
  }
}

void initStrobe()
{
  state = 0;
  RGB = 0xFFFFFF;
  rgb2color();
  lastMode = 2;
  count = 0;
}

void STROBE()
{
  if (count >= MAXSTROBE - strobeSpeed + MINSTROBE)
  {
    state = !state;
    count = 0;
  }
  else
  {
    count ++;
  }


  if (state)
    RGB = 0xFFFFFF;
  else
    RGB = 0x000000;
}


void initFade()
{
  state = 1;
  RGB = 0xFFFFFF;
  rgb2color();
  power = 0;
  lastMode = 3;
}


void FADE()
{
  if (state)
  {
    power += FADESPEED;
  }
  else
  {
    power -= FADESPEED;
  }

  if (power >= MAXPOWER - 1)
  {
    state = 0;
  }

  if (power <= 1)
  {
    state = 1;
  }
}

void initSmooth()
{
  state = 0;
  RGB = 0xFE0000;
  rgb2color();
  lastMode = 4;
}

void SMOOTH()
{
  if (state == 0)
  {
    if (unpoweredGREEN >= 254)
      state = 1;
    else
      RGB += 0x000100;
  }
  else if (state == 1)
  {
    if (unpoweredRED <= 0)
      state = 2;
    else
      RGB -= 0x010000;
  }
  else if (state == 2)
  {
    if (unpoweredBLUE >= 254)
      state = 3;
    else
      RGB += 0x000001;
  }
  else if (state == 3)
  {
    if (unpoweredGREEN <= 0)
      state = 4;
    else
      RGB -= 0x000100;
  }
  else if (state == 4)
  {
    if (unpoweredRED >= 254)
      state = 5;
    else
      RGB += 0x010000;
  }
  else if (state == 5)
  {
    if (unpoweredBLUE <= 0)
      state = 0;
    else
      RGB -= 0x000001;
  }
  else
  {
    state = 0;
  }
}
