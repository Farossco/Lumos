#if defined(LUMOS_ESP8266)

#include "ClientList.h"
#include "Logger.h"
#include "Utils.h"

clientListItem & ClientList::getItem (WiFiServer &server)
{
	return getItem (server, index);
}

clientListItem & ClientList::getItem (WiFiServer &server, int in)
{
	clientListItem * item;

	if (in >= n)
	{
		if (n == 0)
		{
			first         = new clientListItem;
			first->client = server.available();

			item = first;
		}
		else
		{
			item = first;
			for (int i = 0; i < n - 1; i++)
				item = item->next;

			item         = item->next = new clientListItem;
			item->client = server.available();
		}

		index = 0;
		n++;
	}
	else
	{
		item = first;

		for (int i = 0; i < in; i++)
			item = item->next;

		index = in + 1;
	}

	return *item;
} // ClientList::getItem

void ClientList::removeItem (clientListItem & itemToDelete)
{
	if (&itemToDelete == NULL)
	{
		Log.error ("Client is null" dendl);
		return;
	}
	if (first == NULL)
	{
		Log.error ("List is empty" dendl);
		return;
	}

	clientListItem * item, * temp;

	if (first == &itemToDelete)
	{
		temp  = first;
		first = temp->next;
		delete temp;
	}
	else
	{
		for (item = first; item != NULL && item->next != &itemToDelete; item = item->next)
			if (item->next == NULL)
			{
				Log.error ("ClientList : Reached end of list, item not found" dendl);
				return;
			}

		temp       = item->next;
		item->next = temp->next;
		delete temp;
	}

	n--;
} // ClientList::removeClient

String ClientList::getDisplay ()
{
	clientListItem * item = first;
	String str;

	if (n == 0)
		str = "Empty";

	for (int i = 0; i < n; i++)
	{
		str += "| ";
		str += i;
		str += ": ";
		str += utils.ltos ((uint32_t) item, HEX);
		str += " |";

		item = item->next;
	}

	return str;
}

int ClientList::getLength ()
{
	return n;
}

#endif // if defined(LUMOS_ESP8266)