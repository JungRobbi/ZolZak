#pragma once
#include <iostream>
#include <memory>
#include <thread>
#include <list>
#include <mutex>

#include "../Globals.h"

#include "../ImaysNet/Packet.h"
#include "../ImaysNet/PacketQueue.h"

using namespace std;

class Socket;

class NetworkMGR
{
	static recursive_mutex mutex;

public:
	static char* SERVERIP;
	static shared_ptr<Socket> tcpSocket;
public:
	static void start();
	static void Tick();

	static void do_recv();
	static void do_send(const char* buf, short buf_size);
	static void Process_Packet(char* p_Packet);
};