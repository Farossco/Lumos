void readSerial ()
{
	// If nothing is comming, we stop here
	if (!Serial.available())
		return;

	printlnNoPrefix();
	decodeRequest (Serial.readStringUntil ('z'));

	if (infoType == TYPE_RTM)
		sendTime();  // We send the time to the Arduino
	else if (infoType == TYPE_RIF)
		println ("Info request needs to by sent by web client");
} // readSerial
