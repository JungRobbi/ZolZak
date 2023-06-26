#pragma once
#include <iostream>
#include <memory>
#include <thread>
#include <list>
#include <mutex>
#include <string>

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

	static unsigned int id;
	static string name;

	static bool b_isNet; // ���� ���� ����
	static bool b_isLogin; // �α��� �Ϸ� ����
	static bool b_isLoginProg; // �α��� ���� ������ ����
public:
	static void start();
	static void Tick();

	static void do_connetion();
	static void do_recv();
	static void do_send(const char* buf, short buf_size);
	static void Process_Packet(char* p_Packet);
};