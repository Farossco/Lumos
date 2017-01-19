#include <IRremote.h>
#define LEDRED 8
#define LEDGREEN 9
#define LEDBLUE 10
#define INFRARED 11
#define SPEED 0x050505
#define MINPOWER 5
#define MAXPOWER 100
#define POWERSPEED 5

IRrecv irrecv(INFRARED);
decode_results results;

long color[][3] =
  //RGB Code //Code 1  //Code 2
{ {0xFFFFFF, 0xFFA857, 0xA3C8EDDB}, //WHITE
  {0xFF0000, 0xFF9867, 0x97483BFB}, //RED
  {0x00FF00, 0xFFD827, 0x86B0E697}, //GREEN
  {0x0000FF, 0xFF8877, 0x9EF4941F}  //BLUE
};

boolean on = false;
int mode;
long RGB;
int COUNT;
int powerRED;
int powerGREEN;
int powerBLUE;
int etat = 1;
int i;
float power;
long lastCode;
long actualCode;

void setup()
{
  pinMode(LEDRED, OUTPUT);
  pinMode(LEDGREEN, OUTPUT);
  pinMode(LEDBLUE, OUTPUT);
  pinMode(INFRARED, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(250000);
  irrecv.enableIRIn();
  RGB = 0xFFFFFF;
  mode = 0;
  COUNT = 0;
  etat = 1;
  power = 10;
  lastCode = 0;
  actualCode = 0;
}


void loop()
{
  readInfrared();

  action();

  light();
}

void light()
{
  rgb2power();

  if (on)
  {
    for (i = 0; i < powerRED * 1; i++) digitalWrite(LEDRED, LOW);
    for (i = 0; i < (255 - powerRED) * 1; i++) digitalWrite(LEDRED, HIGH);

    for (i = 0; i < powerGREEN * 1; i++) digitalWrite(LEDGREEN, LOW);
    for (i = 0; i < (255 - powerGREEN) * 1; i++) digitalWrite(LEDGREEN, HIGH);

    for (i = 0; i < powerBLUE * 1; i++) digitalWrite(LEDBLUE, LOW);
    for (i = 0; i < (255 - powerBLUE) * 1; i++) digitalWrite(LEDBLUE, HIGH);

  }
  else
  {
    digitalWrite(LEDRED, HIGH);
    digitalWrite(LEDGREEN, HIGH);
    digitalWrite(LEDBLUE, HIGH);
  }
}

void rgb2power()
{
  powerRED =   ((RGB & 0xFF0000) >> 16) * (power / MAXPOWER);
  powerGREEN = ((RGB & 0x00FF00) >> 8) * (power / MAXPOWER);
  powerBLUE =   (RGB & 0x0000FF) * (power / MAXPOWER);
}

void readInfrared()
{
  if (irrecv.decode(&results))
  {
    actualCode = results.value;

    // REPEAT
    if (actualCode == 0xFFFFFFFF)
      actualCode = lastCode;

    switch (actualCode)
    {
      // ON
      case 0xFFB04F:
      case 0xF0C41643:
        mode = 0;
        on = true;
        lastCode = 0;
        break;
        break;

      //OFF
      case 0xFFF807:
      case 0xE721C0DB:
        mode = 0;
        on = false;
        lastCode = 0;
        break;
        break;

      //DOWN
      case 0xFFB847:
      case 0xA23C94BF:
        if (power >= MINPOWER + POWERSPEED) power -= POWERSPEED;
        else power = MINPOWER;
        rgb2power();
        lastCode = actualCode;
        Serial.print("Power: ");
        Serial.println(power);
        Serial.print("RED: ");
        Serial.print(powerRED);
        Serial.print(" / GREEN: ");
        Serial.print(powerGREEN);
        Serial.print(" / BLUE: ");
        Serial.println(powerBLUE);
        break;
        break;

      //UP
      case 0xFF906F:
      case 0xE5CFBD7F:
        if (power <= MAXPOWER - POWERSPEED) power += POWERSPEED;
        else power = MAXPOWER;
        rgb2power();
        lastCode = actualCode;
        Serial.print("Power: ");
        Serial.println(power);
        Serial.print("RED: ");
        Serial.print(powerRED);
        Serial.print(" / GREEN: ");
        Serial.print(powerGREEN);
        Serial.print(" / BLUE: ");
        Serial.println(powerBLUE);
        break;
        break;

      //COLORS
      default:
        lastCode = 0;
        if (on)
          for (i = 0; i < strlen(color); i++)
            if (results.value == color[i][1] || results.value == color[i][2])
            {
              mode = 0;
              RGB = color[i][0];
            }
        break;
    }
    Serial.println(actualCode, HEX);
    irrecv.resume();
  }
}


void action()
{
  if (!on)
  {
    return;
  }
}

void FADE()
{
  if (RGB >= 0xFFFFFF - SPEED)
  {
    etat = 0;
  }

  if (RGB <= 1 + SPEED)
  {
    etat = 1;
  }

  COUNT++;

  if (COUNT >= 0)
  {
    if (etat)
    {
      RGB += SPEED;
      COUNT = 0;
    }
    else
    {
      RGB -= SPEED;
      COUNT = 0;
    }
  }
}
