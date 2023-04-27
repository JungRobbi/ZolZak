#pragma once
#include "../../ImaysNet/ImaysNet.h"

#include <thread>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <concurrent_unordered_map.h>

#include "../Input.h"

#include "../Player.h"

using namespace std;
using namespace concurrency;

class RemoteClient {
public:
	static concurrent_unordered_map<RemoteClient*, shared_ptr<RemoteClient>> remoteClients;
public:
	shared_ptr<thread> thread;	// Ŭ���̾�Ʈ ������
	Socket tcpConnection;		// accept�� TCP ����

	unsigned long long m_id;

	char name[NAME_SIZE];

	Input m_KeyInput;
	shared_ptr<Player> m_pPlayer;

	bool b_Enable = true;

	RemoteClient() : thread(), tcpConnection(SocketType::Tcp), m_KeyInput(), m_pPlayer() {}
	RemoteClient(SocketType socketType) :tcpConnection(socketType), m_KeyInput(), m_pPlayer() {}
};