#include <ESP8266WiFi.h>
#include <Wire.h>

#define DEBUG true

WiFiServer server(80);

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
  WiFiClient client = server.available();

  if (!client) return;

  long result;
  int infoType;

  // Wait until the client sends some data
  while (!client.available()) delay(1);

  // Read the first line of the request
  String request = client.readStringUntil('\r');

  if (DEBUG)
  {
    Serial.println();
    Serial.println(request);
  }

  client.flush();

  // Match the request
  if (request.indexOf("/RGB=") != -1)
  {
    result = getRGB(request);
    infoType = 1;
  }
  else if (request.indexOf("/ON=") != -1)
  {
    result = getON(request);
    infoType = 2;
  }
  else if (request.indexOf("/POW=") != -1)
  {
    result = getPOW(request);
    infoType = 3;
  }
  else if (request.indexOf("/MOD=") != -1)
  {
    result = getMode(request);
    infoType = 4;
  }
  else
  {
    result = -1;
    infoType = -1;
  }

  if (DEBUG)
  {
    Serial.println("\n");
    if (result == -1)
      Serial.println("CODING ERROR !");
  }

  if (result != -1)
  {
    Serial.print(infoType == 1 ? "RGB" : infoType == 2 ? "ON" : infoType == 3 ? "POW" : infoType == 4 ? "MOD" : "");
    Serial.print(result, infoType == 1 ? HEX : DEC);
  }

  // Return the response
  if (result == -1)
    client.println("HTTP/1.1 400 FORMAT ERROR");
  else
    client.println("HTTP/1.1 200 OK");

  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  if (result == -1)
  {
    client.println("Format error !");
  }
  else
  {
    client.println(result, infoType == 1 ? HEX : DEC);
  }
}

long getRGB(String request)
{
  byte buf[16];
  int value = LOW;

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

  int error = false;

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

  long RGB = 0;

  if (error) return -1;

  for (int i = 9; i < 15; i++)
  {
    RGB += buf[i] * pow(16, 5 - (i - 9));
  }

  if (DEBUG)
  {
    Serial.println();
    Serial.print("RGB = ");
    Serial.println(RGB, HEX);
  }

  return RGB;
}

long getON(String request)
{
  byte buf[10];
  int ON;

  request.getBytes(buf, 10);

  ON = buf[8];

  if (ON != '0' && ON != '1') return -1;

  if (DEBUG)
  {
    Serial.println();
    Serial.print("ON = ");
    Serial.println(ON - '0');
  }

  return ON - '0';
}

long getPOW(String request)
{
  byte buf[13];
  int value = LOW;

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

  int error = false;

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

  if (error) return -1;

  int POW = 0;

  for (int i = 9; i < 12; i++)
  {
    POW += buf[i] * pow(10, 2 - (i - 9));
  }

  if (DEBUG)
  {
    Serial.println();
    Serial.print("POW = ");
    Serial.println(POW);
  }

  return (POW >= 0 && POW <= 100) ? POW : -1;
}

long getMode(String request)
{
  byte buf[11];
  int Mode;

  request.getBytes(buf, 11);

  Mode = buf[9];

  if (Mode != '1' && Mode != '2' && Mode != '3' && Mode != '4') return -1;

  if (DEBUG)
  {
    Serial.println();
    Serial.print("Mode = ");
    Serial.println(Mode - '0');
    Serial.print("Mode (Text) = ");
    Serial.println(Mode == '1' ? "FLASH" : Mode == '2' ? "STROBE" : Mode == '3' ? "FADE" : "SMOOTH");
  }

  return Mode - '0';
}
