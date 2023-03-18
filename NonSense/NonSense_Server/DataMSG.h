#pragma once
#include <string.h>

#include "../Globals.h"

enum E_MSG 
{
	E_MSG_NONE = 0, E_MSG_CONNECT, E_MSG_REQUEST_PLAYERS, E_MSG_POSITIONING, E_MSG_MOVE
};

class DataMSG
{
public:
	double id;
	unsigned int msg;
	char data[BUFSIZE - 4 - 8];

public:
	DataMSG();
	DataMSG(char* buf);
	~DataMSG();
};

