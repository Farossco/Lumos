#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define BAUD_RATE 250000 // Serial baud rate

#define DEBUG_ENABLED false

//Some IDs used for serial reception decrypt
#define TYPE_ERROR -1
#define TYPE_RGB 1
#define TYPE_ON 2
#define TYPE_POW 3
#define TYPE_MOD 4

#define TIME_ZONE 1 // Central European Time

#define LOCAL_PORT 8888 // Local port to listen for UDP packets

// Wi-Fi informations
#define SSID0 "Patatou"
#define PASS0 "FD00000000"

WiFiUDP Udp;
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
IPAddress timeServer (132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov

WiFiServer server (80);
WiFiClient client;

int infoType;
String request;

// Variables for web request reading
byte buf[20];
long value;
boolean error;

void setup ()
{
	pinMode (0, OUTPUT);
	pinMode (2, OUTPUT);
	digitalWrite (0, LOW);
	digitalWrite (2, LOW);

	Serial.begin (BAUD_RATE);
	delay (10);

	// Connect to WiFi network
	if (DEBUG_ENABLED)
	{
		Serial.println ();
		Serial.println ();
		Serial.print ("Connecting to ");
		Serial.println (SSID0);
	}

	WiFi.begin (SSID0, PASS0);

	while (WiFi.status () != WL_CONNECTED)
	{
		delay (500);
		if (DEBUG_ENABLED)
			Serial.print (".");
	}
	if (DEBUG_ENABLED)
	{
		Serial.println ("");
		Serial.println ("WiFi connected");
	}

	// Start the server
	server.begin ();

	if (DEBUG_ENABLED)
	{
		Serial.println ("Server started");

		// Print the IP address
		Serial.print ("Use this URL to connect: ");
		Serial.print ("http://");
		Serial.print (WiFi.localIP ());
		Serial.println ("/");
	}

	sendTime ();
}

void loop ()
{
	readSerial ();

	// Check if a client has connected
	client = server.available ();

	if (!client)
		return;

	// Wait until the client sends some data
	while (!client.available ())
		delay (1);

	// Read the first line of the request
	request = client.readStringUntil ('\r');

	if (DEBUG_ENABLED)
	{
		Serial.println ();
		Serial.println (request);
	}

	client.flush ();

	// Match the request
	if (request.indexOf ("/RGB=") != -1)
	{
		getRgb ();
		infoType = TYPE_RGB;
	}
	else if (request.indexOf ("/ON=") != -1)
	{
		getOn ();
		infoType = TYPE_ON;
	}
	else if (request.indexOf ("/POW=") != -1)
	{
		getPow ();
		infoType = TYPE_POW;
	}
	else if (request.indexOf ("/MOD=") != -1)
	{
		getMode ();
		infoType = TYPE_MOD;
	}
	else
	{
		value = -1;
		infoType = TYPE_ERROR;
	}

	if (DEBUG_ENABLED)
	{
		if (value == -1)
			Serial.println ("CODING ERROR !");
	}

	if (value != -1)
	{
		if (DEBUG_ENABLED)
			Serial.print ("\nSending to Arduino: ");
		Serial.print (
				infoType == TYPE_RGB ? "RGB" : infoType == TYPE_ON ? "ON" :
				infoType == TYPE_POW ? "POW" :
				infoType == TYPE_MOD ? "MOD" : "");
		Serial.print (value, infoType == 1 ? HEX : DEC);
		if (DEBUG_ENABLED)
			Serial.print ("\n");
	}

	// Return the response
	client.print ("HTTP/1.1 ");
	client.println (value == -1 ? "400 FORMAT ERROR" : "200 OK");
	client.println ("Content-Type: text/html");
	client.println (""); // Do not forget this one
	if (value == -1)
		client.println ("Format error !");
	else
		client.println (value, infoType == TYPE_RGB ? HEX : DEC);
}

void readSerial ()
{
	if (!Serial.available ())
		return;

	int n;
	char messageChar[20];
	String message;

	for (n = 0; Serial.available () && n < 20; n++)
	{
		messageChar[n] = Serial.read ();
		delay (1);
	}

	for (int i = 0; i < n; i++)
		message += String (messageChar[i]);

	if (message.indexOf ("TIMEPLEASE") != -1)
	{
		if (DEBUG_ENABLED)
			Serial.println ("\nReceived time request from arduino");

		sendTime ();
	}
}

void sendTime ()
{
	if (DEBUG_ENABLED)
		Serial.println ("\nStarting UDP");

	Udp.begin (LOCAL_PORT);

	if (DEBUG_ENABLED)
	{
		Serial.print ("Local port: ");
		Serial.println (Udp.localPort ());
		Serial.println ("waiting for sync");
	}

	setSyncProvider (getNtpTime);

	if (DEBUG_ENABLED)
	{
		Serial.print ("\nTime is: ");
		digitalClockDisplay ();
	}

	if (timeStatus () != timeNotSet)
	{
		if (DEBUG_ENABLED)
		{
			Serial.print ("\nSending time to arduino: ");
			delay (1);
		}

		Serial.print ("TIM");
		Serial.print (now ());
	}
	else if (DEBUG_ENABLED)
		Serial.print ("\nTime not set, not sending time to arduino...");

	if (DEBUG_ENABLED)
		Serial.println ();
}

void getRgb ()
{
	value = 0;
	error = false;

	request.getBytes (buf, 16);

	if (DEBUG_ENABLED)
	{
		for (int i = 9; i < 15; i++)
		{
			Serial.print (buf[i], HEX);
			Serial.print (" / ");
		}
		Serial.println ("\n ----------- ");
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
		if (DEBUG_ENABLED)
		{
			Serial.print (buf[i], HEX);
			Serial.print (" / ");
		}
	}

	for (int i = 9; i < 15; i++)
	{
		value += buf[i] * pow (16, 5 - (i - 9));
	}

	if (DEBUG_ENABLED)
	{
		Serial.println ();
		Serial.print ("\nRGB = ");
		Serial.println (value, HEX);
	}

	if (error)
		value = -1;
}

void getOn ()
{
	request.getBytes (buf, 10);

	value = buf[8] - '0';

	if (DEBUG_ENABLED)
	{
		Serial.println ();
		Serial.print ("ON = ");
		Serial.println (value);
	}

	if (value != 0 && value != 1)
		value = -1;
}

void getPow ()
{
	value = 0;
	error = false;

	request.getBytes (buf, 13);

	if (DEBUG_ENABLED)
	{
		for (int i = 9; i < 12; i++)
		{
			Serial.print (buf[i], HEX);
			Serial.print (" / ");
		}
		Serial.println ("\n ----------- ");
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
		if (DEBUG_ENABLED)
		{
			Serial.print (buf[i], DEC);
			Serial.print (" / ");
		}
	}

	for (int i = 9; i < 12; i++)
	{
		value += buf[i] * pow (10, 2 - (i - 9));
	}

	if (DEBUG_ENABLED)
	{
		Serial.println ();
		Serial.print ("\nPOW = ");
		Serial.println (value);
	}

	if (value < 0 || value > 100 || error)
		value = -1;
}

void getMode ()
{
	request.getBytes (buf, 11);

	value = buf[9] - '0';

	if (DEBUG_ENABLED)
	{
		Serial.println ();
		Serial.print ("Mode = ");
		Serial.println (value);
		Serial.print ("Mode (Text) = ");
		Serial.println (
				value == 0 ? "DEFAULT" : value == 1 ? "FLASH" :
				value == 2 ? "STROBE" : value == 3 ? "FADE" :
				value == 4 ? "SMOOTH" : value == 5 ? "WAKEUP" : "UNKNOWN");
	}

	if (value < 0 || value > 5)
		value = -1;
}

void digitalClockDisplay ()
{
	// digital clock display of the time
	printDigits (day ());
	Serial.print ("/");
	printDigits (month ());
	Serial.print ("/");
	Serial.print (year ());

	Serial.print (" ");

	printDigits (hour ());
	Serial.print (":");
	printDigits (minute ());
	Serial.print (":");
	printDigits (second ());

	Serial.println ();
}

void printDigits (int digits)
{
	// utility for digital clock display: prints preceding colon and leading 0
	if (digits < 10)
		Serial.print ('0');
	Serial.print (digits);
}

/*-------- NTP code ----------*/

time_t getNtpTime ()
{
	int i = 1;

	while (i < 16)
	{
		while (Udp.parsePacket () > 0)
			; // discard any previously received packets

		if (DEBUG_ENABLED)
		{
			Serial.print ("Transmit NTP Request ");
			Serial.print ("(Attempt ");
			Serial.print (i);
			Serial.println (")");
		}

		sendNTPpacket (timeServer);
		uint32_t beginWait = millis ();
		while (millis () - beginWait < 1500)
		{
			int size = Udp.parsePacket ();
			if (size >= NTP_PACKET_SIZE)
			{
				if (DEBUG_ENABLED)
					Serial.println ("\nReceive NTP Response !");
				Udp.read (packetBuffer, NTP_PACKET_SIZE); // read packet into the buffer
				unsigned long secsSince1900;
				// convert four bytes starting at location 40 to a long integer
				secsSince1900 = (unsigned long) packetBuffer[40] << 24;
				secsSince1900 |= (unsigned long) packetBuffer[41] << 16;
				secsSince1900 |= (unsigned long) packetBuffer[42] << 8;
				secsSince1900 |= (unsigned long) packetBuffer[43];
				return secsSince1900 - 2208988800UL + TIME_ZONE * SECS_PER_HOUR;
			}
		}
		i++;
	}

	if (DEBUG_ENABLED)
		Serial.println ("\nNo NTP Response :-(");
	return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket (IPAddress &address)
{
	// set all bytes in the buffer to 0
	memset (packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12] = 49;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 49;
	packetBuffer[15] = 52;
	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:
	Udp.beginPacket (address, 123); //NTP requests are to port 123
	Udp.write (packetBuffer, NTP_PACKET_SIZE);
	Udp.endPacket ();
}
