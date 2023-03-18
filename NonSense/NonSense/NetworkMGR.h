#pragma once
#include <iostream>
#include <memory>
#include <thread>
#include <list>
#include <mutex>

#include "../Globals.h"

#include "DataMSG.h"
#include "MSGQueue.h"

using namespace std;

class Socket;

class NetworkMGR
{
	static list<shared_ptr<thread>> threads;

public:
	static recursive_mutex mx_out;

	static char* SERVERIP;
	static shared_ptr<Socket> tcpSocket;
	
	static double ID;

public:
	static void start();
	static void Tick();

	static int getCount_Thread()
	{
		return threads.size();
	}

	static void ClientThread(); // 서버와의 연결
};