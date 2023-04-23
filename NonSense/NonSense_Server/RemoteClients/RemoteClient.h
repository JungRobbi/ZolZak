#pragma once
#include "../../ImaysNet/ImaysNet.h"

#include <thread>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "../Input.h"

#include "../Player.h"

using namespace std;

class RemoteClient {
public:
	static unordered_map<RemoteClient*, shared_ptr<RemoteClient>> remoteClients;
	static recursive_mutex mx_rc;
public:
	shared_ptr<thread> thread; // 클라이언트 스레드
	Socket tcpConnection;		// accept한 TCP 연결

	unsigned long long m_id;

	char name[NAME_SIZE];

	Input m_KeyInput;
	shared_ptr<Player> m_pPlayer;

	RemoteClient() : thread(), tcpConnection(SocketType::Tcp), m_KeyInput(), m_pPlayer() {}
	RemoteClient(SocketType socketType) :tcpConnection(socketType), m_KeyInput(), m_pPlayer() {}
};