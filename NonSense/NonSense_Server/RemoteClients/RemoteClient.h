#pragma once
#include "../../ImaysNet/ImaysNet.h"

#include <thread>
#include <memory>
#include <mutex>

#include "../Input.h"

using namespace std;

class RemoteClient {
public:
	shared_ptr<thread> thread; // Ŭ���̾�Ʈ ������
	Socket tcpConnection;		// accept�� TCP ����

	unsigned long long m_id;

	char name[NAME_SIZE];

	Input m_KeyInput;

	RemoteClient() : thread(), tcpConnection(SocketType::Tcp), m_KeyInput() {}
	RemoteClient(SocketType socketType) :tcpConnection(socketType), m_KeyInput() {}
};