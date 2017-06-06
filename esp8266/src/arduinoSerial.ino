void readSerial ()
{
	// If nothing is comming, we stop here
	if (!Serial.available())
		return;

	message = "";

	for (n = 0; n < 20 && Serial.available(); n++)
	{
		messageChar[n] = Serial.read();
		delay (1);
	}

	// Converting char array into String
	for (int i = 0; i < n; i++)
		message += String (messageChar[i]);

	// If the Arduino is asking for time
	if (message.indexOf ("TIMEPLEASE") != -1)
	{
		println ("\nReceived time request from arduino");

		sendTime(); // We send the time to the Arduino
	}
}
