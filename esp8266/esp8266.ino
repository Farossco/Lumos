#include <ESP8266WiFi.h>
#include <Wire.h>

const char* ssid = "Patatou";
const char* password = "FD00000000";

WiFiServer server(80);

void setup()
{
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi network
  //Serial.println();
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println("");
  //Serial.println("WiFi connected");

  // Start the server
  server.begin();
  //Serial.println("Server started");

  // Print the IP address
  //Serial.print("Use this URL to connect: ");
  //Serial.print("http://");
  //Serial.print(WiFi.localIP());
  //Serial.println("/");

}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();

  if (!client) return;

  long result;
  int infoType;

  // Wait until the client sends some data
  while (!client.available()) delay(1);

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  //Serial.println(request);
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
  else
  {
    result = -1;
    infoType = -1;
  }
  /*
    //Serial.println("\n");
    if (result == -1)
    {
      //Serial.println("CODING ERROR !");
    }
    else*/
  Serial.print(infoType == 1 ? "RGB" : infoType == 2 ? "ON" : "");
  Serial.print(result, HEX);



  // Return the response
  if (result == -1)
    client.println("HTTP/1.1 400 OK");
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
    client.println(result, HEX);
  }
}

long getRGB(String request)
{
  byte buf[15];
  int value = LOW;

  request.getBytes(buf, 16);
  for (int i = 9; i < 15; i++)
  {
    //Serial.print(buf[i], HEX);
    //Serial.print(" / ");
  }

  //Serial.println("\n ----------- ");

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

    //Serial.print(buf[i], HEX);
    //Serial.print(" / ");
  }

  long RGB = 0;

  if (error) return -1;

  for (int i = 9; i < 15; i++)
  {
    RGB += buf[i] * pow(16, 5 - (i - 9));
  }

  return RGB;
}

long getON(String request)
{
  byte buf[9];
  int ON;

  request.getBytes(buf, 10);

  ON = buf[8];

  if (ON != '0' && ON != '1') return -1;

  return ON - '0';
}

