#pragma once
#include "../../ImaysNet/ImaysNet.h"
#include "../../AnimationType.h"

#include <thread>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <concurrent_unordered_map.h>
#include <atomic>

#include "../Input.h"

#include "../Player.h"

using namespace std;
using namespace concurrency;

class RemoteClient {
public:
	static concurrent_unordered_map<RemoteClient*, shared_ptr<RemoteClient>> remoteClients;
public:
	shared_ptr<thread> thread;	// 클라이언트 스레드
	Socket tcpConnection;		// accept한 TCP 연결

	unsigned long long m_id;

	char name[NAME_SIZE];

	Input m_KeyInput;
	shared_ptr<Player> m_pPlayer;
	bool m_clear_stage = 0;

	std::atomic<bool> b_Enable;
	std::atomic<bool> b_Login = false;

	RemoteClient() : thread(), tcpConnection(SocketType::Tcp), m_KeyInput(), m_pPlayer(), b_Enable(true) {}
	RemoteClient(SocketType socketType) :tcpConnection(socketType), m_KeyInput(), m_pPlayer(), b_Enable(true) {}
};