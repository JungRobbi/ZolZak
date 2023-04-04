#pragma once
#include <list>

#include "DataMSG.h"

class PacketQueue
{
public:
	static std::list<DataMSG> SendPacketQueue;
	static std::list<DataMSG> RecvPacketQueue;

public:
	static void AddSendMSG(DataMSG input) { SendPacketQueue.push_back(input); }
	static void AddRecvMSG(DataMSG input) { RecvPacketQueue.push_back(input); }

	static DataMSG& PopFrontSendMSG()
	{
		DataMSG front_msg = SendPacketQueue.front();
		SendPacketQueue.pop_front();
		return front_msg;
	}
	static DataMSG& PopFrontRecvMSG()
	{
		DataMSG front_msg = RecvPacketQueue.front();
		RecvPacketQueue.pop_front();
		return front_msg;
	}

};

