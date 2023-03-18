#include "DataMSG.h"

DataMSG::DataMSG() : id{}, msg{ E_MSG_NONE }
{
	memset(data, 0, sizeof(data));
}

DataMSG::DataMSG(char* buf)
{
	memcpy(&id, buf, sizeof(double));
	memcpy(&msg, &buf[8], sizeof(int));
	memcpy(&data, &buf[12], sizeof(data));
}

DataMSG::~DataMSG()
{
}
