#pragma once
#include "../Globals.h"

#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <unordered_map>

#include <signal.h>
#include <thread>
#include <memory>
#include <mutex>

#include "Input.h"
#include "Timer.h"

#include "RemoteClients/RemoteClient.h"
#include "Scene.h"

using namespace std;

volatile bool stopWorking = false;

const int N_THREAD{ 1 };
static unsigned long long N_CLIENT_ID{ 0 };

shared_ptr<Scene> scene;

vector<shared_ptr<thread>> worker_threads;

enum class IO_TYPE;

void ProcessSignalAction(int sig_number)
{
	if (sig_number == SIGINT)
		stopWorking = true;
}

unordered_map<RemoteClient*, shared_ptr<RemoteClient>> remoteClients;
vector<RemoteClient*> remoteClients_ptr_v; // 클라이언트들은 0부터 시작하는 양수의 id를 갖는다. id는 클라이언트 포인터를 담는 벡터의 인덱스를 의미한다.
recursive_mutex mx_rc;

list<shared_ptr<RemoteClient>> deleteClinets;

void ProcessClientLeave(shared_ptr<RemoteClient> remoteClient)
{
	// 에러 혹은 소켓 종료이다.
	// 해당 소켓은 제거해버리자. 
	remoteClient->tcpConnection.Close();
	{
		lock_guard<recursive_mutex> lock_rc(mx_rc);
		remoteClients.erase(remoteClient.get());

		cout << "Client left. There are " << remoteClients.size() << " connections.\n";
	}
}

// IOCP를 준비한다.
Iocp iocp(N_THREAD); // 5개의 스레드 사용을 API에 힌트로 준다.

recursive_mutex mx_accept;

shared_ptr<Socket> p_listenSocket;
shared_ptr<RemoteClient> remoteClientCandidate;

void CloseServer();
void ProcessAccept();

void Process_Packet(shared_ptr<RemoteClient>& p_Client, char* p_Packet);

void Worker_Thread()
{
	try
	{
		while (!stopWorking) {
			// I/O 완료 이벤트가 있을 때까지 기다립니다.
			IocpEvents readEvents;
			iocp.Wait(readEvents, 100);

			// 받은 이벤트 각각을 처리합니다.
			for (int i = 0; i < readEvents.m_eventCount; ++i)
			{
				auto& readEvent = readEvents.m_events[i];
				auto p_readOverlapped = (EXP_OVER*)readEvent.lpOverlapped;

				if (IO_TYPE::IO_SEND == p_readOverlapped->m_ioType) {
					cout << " Send! - size : " << (int)p_readOverlapped->_buf[0] << endl;
					cout << " Send! - type : " << (int)p_readOverlapped->_buf[1] << endl;
					p_readOverlapped->m_isReadOverlapped = false;
					continue;
				}

				if (readEvent.lpCompletionKey == 0) // 리슨소켓이면
				{
					ProcessAccept(); // 클라이언트 연결 작업
				}
				else  // TCP 연결 소켓이면
				{
					// 처리할 클라이언트 받아오기
					shared_ptr<RemoteClient> remoteClient;
					{
						lock_guard<recursive_mutex> lock_rc(mx_rc);
						remoteClient = remoteClients[(RemoteClient*)readEvent.lpCompletionKey];
					}

					//
					if (remoteClient)
					{
						// 이미 수신된 상태이다. 수신 완료된 것을 그냥 꺼내 쓰자.
						remoteClient->tcpConnection.m_isReadOverlapped = false;
						int ec = readEvent.dwNumberOfBytesTransferred;

						if (ec <= 0)
						{
							// 읽은 결과가 0 즉 TCP 연결이 끝났다...
							// 혹은 음수 즉 뭔가 에러가 난 상태이다...
							ProcessClientLeave(remoteClient);
						}
						else
						{
							// 이미 수신된 상태이다. 수신 완료된 것을 그냥 꺼내 쓰자.
							char* recv_buf = remoteClient->tcpConnection.m_recvOverlapped._buf;
							int recv_buf_Length = ec;

							cout << " recv! - recv_buf_Length : " << recv_buf_Length << endl;

							{ // 패킷 처리
								int remain_data = recv_buf_Length + remoteClient->tcpConnection.m_prev_remain;
								while (remain_data > 0) {
									unsigned char packet_size = recv_buf[0];
									// 남은 데이터가 현재 처리할 패킷 크기보다 적으면 잘린 것이다. (혹은 딱 맞게 떨어진 것이다.)
									if (packet_size > remain_data)
										break;

									//패킷 처리
									Process_Packet(remoteClient, recv_buf);

									//다음 패킷 이동, 남은 데이터 갱신
									recv_buf += packet_size;
									remain_data -= packet_size;

								}
								//남은 데이터 저장
								remoteClient->tcpConnection.m_prev_remain = remain_data;

								//남은 데이터가 0이 아닌 값을 가지면 recv_buf의 맨 앞으로 복사한다.
								if (remain_data > 0) {
									memcpy(remoteClient->tcpConnection.m_recvOverlapped._buf, recv_buf, remain_data);
								}
							}

							// 수신 받을 준비를 한다.
							if (remoteClient->tcpConnection.ReceiveOverlapped() != 0
								&& WSAGetLastError() != ERROR_IO_PENDING)
							{
								ProcessClientLeave(remoteClient);
							}
							else
							{
								// I/O를 걸었다. 완료를 대기하는 중 상태로 바꾸자.
								remoteClient->tcpConnection.m_isReadOverlapped = true;
							}
						}
					}
				}
			}
		}
	}
	catch (Exception& e)
	{
		cout << "Exception! " << e.what() << endl;
	}
}

int main(int argc, char* argv[])
{
	// 사용자가 ctl-c를 누르면 메인루프를 종료하게 만듭니다.
	signal(SIGINT, ProcessSignalAction);

	scene = make_shared<Scene>();
	Timer::Initialize();
	Timer::Reset();

	p_listenSocket = make_shared<Socket>(SocketType::Tcp);
	p_listenSocket->Bind(Endpoint("0.0.0.0", SERVERPORT));
	p_listenSocket->Listen();
	// IOCP에 추가한다.
	iocp.Add(*p_listenSocket, nullptr);

	remoteClientCandidate = make_shared<RemoteClient>(SocketType::Tcp);

	string errorText;
	if (!p_listenSocket->AcceptOverlapped(remoteClientCandidate->tcpConnection, errorText)
		&& WSAGetLastError() != ERROR_IO_PENDING)
	{
		throw Exception("Overlapped AcceptEx failed.");
	}
	p_listenSocket->m_isReadOverlapped = true;

	for (int i{}; i < N_THREAD; ++i)
		worker_threads.emplace_back(make_shared<thread>(Worker_Thread));

	while (true) {
		Timer::Tick(0.0f);

		for (auto& rc : remoteClients) {
			DWORD direction = 0;
			if (rc.second->m_KeyInput.keys['W'] || rc.second->m_KeyInput.keys['w']) direction |= DIR_FORWARD;
			if (rc.second->m_KeyInput.keys['S'] || rc.second->m_KeyInput.keys['s']) direction |= DIR_BACKWARD;
			if (rc.second->m_KeyInput.keys['A'] || rc.second->m_KeyInput.keys['a']) direction |= DIR_LEFT;
			if (rc.second->m_KeyInput.keys['D'] || rc.second->m_KeyInput.keys['d']) direction |= DIR_RIGHT;

			if (direction) {
				cout << "direction = " << direction << endl;

				for (auto& rc_to : remoteClients) {
					SC_MOVE_PLAYER_PACKET send_packet;
					send_packet.size = sizeof(SC_MOVE_PLAYER_PACKET);
					send_packet.type = E_PACKET::E_PACKET_SC_MOVE_PLAYER;
					send_packet.id = rc.second->m_id;
					send_packet.direction = direction;
					rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
				}
			}
		}
	}

	for (auto& th : worker_threads) th->join();

	// 서버 종료
	CloseServer();
}

void CloseServer()
{
	lock_guard<recursive_mutex> lock_accept(mx_accept);
	// i/o 완료 체크
	p_listenSocket->Close();
	{
		lock_guard<recursive_mutex> lock_rc(mx_rc);

		for (auto i : remoteClients)
		{
			i.second->tcpConnection.Close();
		}


		// 서버를 종료하기 위한 정리중
		cout << "서버를 종료하고 있습니다...\n";
		while (remoteClients.size() > 0)
		{
			// I/O completion이 없는 상태의 RemoteClient를 제거한다.
			for (auto i = remoteClients.begin(); i != remoteClients.end(); ++i)
			{
				if (!i->second->tcpConnection.m_isReadOverlapped)
					remoteClients.erase(i);
			}

			// I/O completion이 발생하면 더 이상 Overlapped I/O를 걸지 말고 '이제 정리해도 돼...'를 플래깅한다.
			IocpEvents readEvents;
			iocp.Wait(readEvents, 100);

			// 받은 이벤트 각각을 처리합니다.
			for (int i = 0; i < readEvents.m_eventCount; i++)
			{
				auto& readEvent = readEvents.m_events[i];
				if (readEvent.lpCompletionKey == 0) // 리슨소켓이면
				{
					p_listenSocket->m_isReadOverlapped = false;
				}
				else
				{
					shared_ptr<RemoteClient> remoteClient = remoteClients[(RemoteClient*)readEvent.lpCompletionKey];
					if (remoteClient)
					{
						remoteClient->tcpConnection.m_isReadOverlapped = false;
					}
				}
			}
		}
	}
	cout << "서버 끝.\n";
}
void ProcessAccept()
{
	lock_guard<recursive_mutex> lock_accept(mx_accept);
	p_listenSocket->m_isReadOverlapped = false;
	// 이미 accept은 완료되었다. 귀찮지만, Win32 AcceptEx 사용법에 따르는 마무리 작업을 하자. 
	if (remoteClientCandidate->tcpConnection.UpdateAcceptContext(*p_listenSocket) != 0)
	{
		//리슨소켓을 닫았던지 하면 여기서 에러날거다. 그러면 리슨소켓 불능상태로 만들자.
		p_listenSocket->Close();
	}
	else // 잘 처리함
	{
		shared_ptr<RemoteClient> remoteClient = remoteClientCandidate;
		remoteClient->m_id = N_CLIENT_ID++;
		remoteClient->m_pPlayer = make_shared<Player>();
		remoteClient->m_pPlayer->remoteClient = remoteClient.get();
		remoteClients_ptr_v.emplace_back(remoteClient.get());

		// 새 TCP 소켓도 IOCP에 추가한다.
		iocp.Add(remoteClient->tcpConnection, remoteClient.get());

		// overlapped 수신을 받을 수 있어야 하므로 여기서 I/O 수신 요청을 걸어둔다.
		if (remoteClient->tcpConnection.ReceiveOverlapped() != 0
			&& WSAGetLastError() != ERROR_IO_PENDING)
		{
			// 에러. 소켓을 정리하자. 그리고 그냥 버리자.
			remoteClient->tcpConnection.Close();
		}
		else
		{
			// I/O를 걸었다. 완료를 대기하는 중 상태로 바꾸자.
			remoteClient->tcpConnection.m_isReadOverlapped = true;

			// 새 클라이언트를 목록에 추가.
			{
				lock_guard<recursive_mutex> lock_rc(mx_rc);
				remoteClients.insert({ remoteClient.get(), remoteClient });

				cout << "Client joined. There are " << remoteClients.size() << " connections.\n";
			}
		}

		// 계속해서 소켓 받기를 해야 하므로 리슨소켓도 overlapped I/O를 걸자.
		remoteClientCandidate = make_shared<RemoteClient>(SocketType::Tcp);
		string errorText;
		if (!p_listenSocket->AcceptOverlapped(remoteClientCandidate->tcpConnection, errorText)
			&& WSAGetLastError() != ERROR_IO_PENDING)
		{
			// 에러나면 리슨소켓 불능 상태로 남기자. 
			p_listenSocket->Close();
		}
		else
		{
			// 리슨소켓은 연결이 들어옴을 기다리는 상태가 되었다.
			p_listenSocket->m_isReadOverlapped = true;
		}
	}
}


void Process_Packet(shared_ptr<RemoteClient>& p_Client, char* p_Packet)
{
	switch (p_Packet[1]) // 패킷 타입
	{
	case E_PACKET::E_PACKET_CS_LOGIN: {
		CS_LOGIN_PACKET* recv_packet = reinterpret_cast<CS_LOGIN_PACKET*>(p_Packet);
		memcpy(p_Client->name,recv_packet->name,sizeof(recv_packet->name));

		//id 부여
		p_Client->m_id = N_CLIENT_ID++;

		// 접속한 클라이언트에게 모든 플레이어 정보 송신
		for (auto& rc : remoteClients) {
			if (rc.second->m_id == p_Client->m_id) 
				continue;
			SC_ADD_PLAYER_PACKET send_packet;
			send_packet.size = sizeof(SC_ADD_PLAYER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_ADD_PLAYER;
			send_packet.id = rc.second->m_id;
			memcpy(send_packet.name, rc.second->name, sizeof(rc.second->name));
			p_Client->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}

		// 다른 클라이언트들에게 접속한 클라이언트 정보 송신
		for (auto& rc : remoteClients) {
			if (rc.second->m_id == p_Client->m_id)
				continue;
			SC_ADD_PLAYER_PACKET send_packet;
			send_packet.size = sizeof(SC_ADD_PLAYER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_ADD_PLAYER;
			send_packet.id = p_Client->m_id;
			memcpy(send_packet.name, p_Client->name, sizeof(p_Client->name));
			rc.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}

		break;
	}
	case E_PACKET::E_PACKET_CS_KEYDOWN: {
		CS_KEYDOWN_PACKET* recv_packet = reinterpret_cast<CS_KEYDOWN_PACKET*>(p_Packet);
		p_Client->m_KeyInput.keys[recv_packet->key] = TRUE;
		cout << recv_packet->key << " E_PACKET_CS_KEYDOWN" << endl;
		break;
	}
	case E_PACKET::E_PACKET_CS_KEYUP: {
		CS_KEYUP_PACKET* recv_packet = reinterpret_cast<CS_KEYUP_PACKET*>(p_Packet);
		p_Client->m_KeyInput.keys[recv_packet->key] = FALSE;
		cout << recv_packet->key << " E_PACKET_CS_KEYUP" << endl;
		break;
	}
	case E_PACKET::E_PACKET_CS_MOVE: {
		
		break;
	}
	default:
		break;
	}
}