#pragma once
#include <list>

#include "DataMSG.h"

class MSGQueue
{
public:
	static std::list<DataMSG> SendMSGQueue;
	static std::list<DataMSG> RecvMSGQueue;

public:
	static void AddSendMSG(DataMSG input) { SendMSGQueue.push_back(input); }
	static void AddRecvMSG(DataMSG input) { RecvMSGQueue.push_back(input); }

	static DataMSG& PopFrontSendMSG()
	{
		DataMSG front_msg = SendMSGQueue.front();
		SendMSGQueue.pop_front();
		return front_msg;
	}
	static DataMSG& PopFrontRecvMSG()
	{
		DataMSG front_msg = RecvMSGQueue.front();
		RecvMSGQueue.pop_front();
		return front_msg;
	}

};

