#ifndef REQUEST_H
#define REQUEST_H

#include <Arduino.h>

class Request
{
public:
	void decode (char * request, long & result, int & infoMod, int & infotype, int & errorType);
};

extern Request request;

#endif // ifndef REQUEST_H
