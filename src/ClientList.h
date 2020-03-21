#ifndef CLIENTLIST_H
#define CLIENTLIST_H

#if defined(LUMOS_ESP8266)

# include <ESP8266WiFi.h>

enum clientListItemState
{
	connected,
	awaitingData,
	processData
};

typedef struct clientListItem
{
	WiFiClient          client;
	clientListItem *    next = NULL;
	clientListItemState state = connected;
	time_t              timeout, count;
} clientListItem;

class ClientList
{
public:
	ClientList() : first (NULL), index (0), n (0){ }

	clientListItem & getItem (WiFiServer &server);
	clientListItem & getItem (WiFiServer &server, int in);
	void removeItem (clientListItem & client);
	String getDisplay ();
	int getLength ();

private:
	clientListItem * first;
	int index, n;
};

#endif // if defined(LUMOS_ESP8266)

#endif // ifndef CLIENTLIST_H