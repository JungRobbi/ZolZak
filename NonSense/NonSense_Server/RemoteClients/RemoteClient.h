#pragma once
#include "../../ImaysNet/ImaysNet.h"

#include <thread>
#include <memory>
#include <mutex>

#include "../Input.h"

#include "../Player.h"

using namespace std;

class RemoteClient {
public:
	shared_ptr<thread> thread; // Ŭ���̾�Ʈ ������
	Socket tcpConnection;		// accept�� TCP ����

	unsigned long long m_id;

	char name[NAME_SIZE];

	Input m_KeyInput;
	shared_ptr<Player> m_pPlayer;

	RemoteClient() : thread(), tcpConnection(SocketType::Tcp), m_KeyInput(), m_pPlayer() {}
	RemoteClient(SocketType socketType) :tcpConnection(socketType), m_KeyInput(), m_pPlayer() {}
};