#include "DataMSG.h"

DataMSG::DataMSG() : msg{ E_MSG_NONE }, data{ NULL }
{
}

DataMSG::DataMSG(char* buf)
{
	memcpy(&msg, buf, sizeof(int));
	memcpy(&data, &buf[4], sizeof(data));
}

DataMSG::~DataMSG()
{
}
