void sendJson (String status)
{
	JsonObject& jsonRootGlobal = jsonBuffer.createObject();

	jsonRootGlobal["STATUS"] = status;
	jsonRootGlobal["ON"]     = on;
	jsonRootGlobal["RGB"]    = rgb;
	jsonRootGlobal["POWER"]  = (int) power;
	jsonRootGlobal["MODE"]   = mode;
	jsonRootGlobal.printTo (client);
}
