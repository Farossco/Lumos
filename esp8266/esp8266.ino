#include <ESP8266WiFi.h>
#include <Wire.h>

#define DEBUG false

//Some IDs used for serial reception decrypt
#define TYPE_ERROR -1
#define TYPE_RGB 1
#define TYPE_ON 2
#define TYPE_POW 3
#define TYPE_MOD 4

WiFiServer server(80);
WiFiClient client;

int infoType;
String request;

// Variables for web request reading
byte buf[20];
long value;
boolean error;

void setup()
{
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);

  Serial.begin(250000);
  delay(10);

  // Connect to WiFi network
  if (DEBUG)
  {
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println("Patatou");
  }

  WiFi.begin("Patatou", "FD00000000");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    if (DEBUG)
      Serial.print(".");
  }
  if (DEBUG)
  {
    Serial.println("");
    Serial.println("WiFi connected");
  }

  // Start the server
  server.begin();

  if (DEBUG)
  {
    Serial.println("Server started");

    // Print the IP address
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
  }
}

void loop()
{
  // Check if a client has connected
  client = server.available();

  if (!client) return;

  // Wait until the client sends some data
  while (!client.available()) delay(1);

  // Read the first line of the request
  request = client.readStringUntil('\r');

  if (DEBUG)
  {
    Serial.println();
    Serial.println(request);
  }

  client.flush();

  // Match the request
  if (request.indexOf("/RGB=") != -1)
  {
    getRgb();
    infoType = TYPE_RGB;
  }
  else if (request.indexOf("/ON=") != -1)
  {
    getOn();
    infoType = TYPE_ON;
  }
  else if (request.indexOf("/POW=") != -1)
  {
    getPow();
    infoType = TYPE_POW;
  }
  else if (request.indexOf("/MOD=") != -1)
  {
    getMode();
    infoType = TYPE_MOD;
  }
  else
  {
    value = -1;
    infoType = TYPE_ERROR;
  }

  if (DEBUG)
  {
    Serial.println("\n");
    if (value == -1)
      Serial.println("CODING ERROR !");
  }

  if (value != -1)
  {
    Serial.print(infoType == TYPE_RGB ? "RGB" : infoType == TYPE_ON ? "ON" : infoType == TYPE_POW ? "POW" : infoType == TYPE_MOD ? "MOD" : "");
    Serial.print(value, infoType == 1 ? HEX : DEC);
  }

  // Return the response
  client.print("HTTP/1.1 ");
  client.println(value == -1 ? "400 FORMAT ERROR" : "200 OK");
  client.println("Content-Type: text/html");
  client.println(""); // Do not forget this one
  if (value == -1)
    client.println("Format error !");
  else
    client.println(value, infoType == TYPE_RGB ? HEX : DEC);
}

void getRgb()
{
  value = 0;
  error = false;

  request.getBytes(buf, 16);

  if (DEBUG)
  {
    for (int i = 9; i < 15; i++)
    {
      Serial.print(buf[i], HEX);
      Serial.print(" / ");
    }
    Serial.println("\n ----------- ");
  }

  for (int i = 9; i < 15; i++)
  {
    if (buf[i] >= '0' && buf[i] <= '9')
      buf[i] = buf[i] - '0';
    else if (buf[i] >= 'A' && buf[i] <= 'F')
      buf[i] = buf[i] - 'A' + 10;
    else
    {
      error = true;
      break;
    }
    if (DEBUG)
    {
      Serial.print(buf[i], HEX);
      Serial.print(" / ");
    }
  }

  for (int i = 9; i < 15; i++)
  {
    value += buf[i] * pow(16, 5 - (i - 9));
  }

  if (DEBUG)
  {
    Serial.println();
    Serial.print("RGB = ");
    Serial.println(value, HEX);
  }

  if (error)
    value = -1;
}

void getOn()
{
  request.getBytes(buf, 10);

  value = buf[8] - '0';

  if (DEBUG)
  {
    Serial.println();
    Serial.print("ON = ");
    Serial.println(value);
  }

  if (value != 0 && value != 1)
    value = -1;
}

void getPow()
{
  value = 0;
  error = false;

  request.getBytes(buf, 13);

  if (DEBUG)
  {
    for (int i = 9; i < 12; i++)
    {
      Serial.print(buf[i], HEX);
      Serial.print(" / ");
    }
    Serial.println("\n ----------- ");
  }

  for (int i = 9; i < 12; i++)
  {
    if (buf[i] >= '0' && buf[i] <= '9')
      buf[i] = buf[i] - '0';
    else
    {
      error = true;
      break;
    }
    if (DEBUG)
    {
      Serial.print(buf[i], DEC);
      Serial.print(" / ");
    }
  }

  for (int i = 9; i < 12; i++)
  {
    value += buf[i] * pow(10, 2 - (i - 9));
  }

  if (DEBUG)
  {
    Serial.println();
    Serial.print("POW = ");
    Serial.println(value);
  }

  if (value < 0 || value > 100 || error)
    value = -1;
}

void getMode()
{
  request.getBytes(buf, 11);

  value = buf[9] - '0';

  if (DEBUG)
  {
    Serial.println();
    Serial.print("Mode = ");
    Serial.println(value);
    Serial.print("Mode (Text) = ");
    Serial.println(value == 1 ? "FLASH" : value == 2 ? "STROBE" : value == 3 ? "FADE" : value == 4 ? "SMOOTH" : "UNKNOWN");
  }

  if (value != 1 && value != 2 && value != 3 && value != 4)
    value =  -1;
}
