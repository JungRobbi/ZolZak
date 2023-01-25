#pragma once
#include <string.h>

#include "../Globals.h"

enum E_MSG 
{
	E_MSG_NONE = 0, E_MSG_POSITIONING, E_MSG_MOVE
};

class DataMSG
{
public:
	int msg;
	char data[BUFSIZE - 4];

public:
	DataMSG();
	DataMSG(char* buf);
	~DataMSG();
};

