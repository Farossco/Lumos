void readWeb ()
{
	client = server.available(); // Check if a client has connected

	if (!client) return;  // If nobody connected, we stop here

	while (!client.available()) // Wait until the client sends some data
		delay (1);

	decodeRequest (decodeWeb());

	if (errorType != ERR_NOE)
	{
		sendJsonToClient ("ERROR", ErrorTypeName (errorType, true));
		client.flush();
		client.stop();
		println ("Stopping here");
		return;
	}

	print ("Sending to arduino: ");

	Serial.print (infoTypeName (infoType, true));

	if (infoType == TYPE_RGB || infoType == TYPE_POW || infoType == TYPE_SPE)
		Serial.print (infoMode, DEC);

	Serial.print (result, infoType == TYPE_RGB ? HEX : DEC);
	Serial.print ('z'); // End character

	printlnNoPrefix();

	sendJsonToClient ("OK", "");

	client.flush();
	client.stop();
} // readWeb

String decodeWeb ()
{
	String request;

	request = client.readStringUntil ('\r'); // Read the first line of the request

	// [DEBUG] Printing client IP
	printlnNoPrefix();
	print ("Received request from ");
	printlnNoPrefix (client.remoteIP());

	// [DEBUG] Printing the incomming request
	println ("Request: " + request);

	// Remove unwanted characteres
	request.remove (0, 5);
	request.remove (request.indexOf (" "), request.length() - request.indexOf (" "));

	return request;
}
