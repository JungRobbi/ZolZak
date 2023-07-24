#pragma once
#include "../Globals.h"

#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <concurrent_unordered_map.h>

#include <signal.h>
#include <thread>
#include <memory>
#include <mutex>
#include <atomic>

#include "Input.h"
#include "Timer.h"

#include "remoteClients/RemoteClient.h"
#include "Scene.h"
#include "Room.h"
#include "Terrain.h"
#include "DBMGR.h"

#include "Components/PlayerMovementComponent.h"
#include "Components/CloseTypeFSMComponent.h"
#include "Components/SphereCollideComponent.h"
#include "Components/BoxCollideComponent.h"

// unordered map 탐색 사용하기 위해 index 추가
// Player 간, Monster 간 시야 처리
using namespace std;
using namespace concurrency;

volatile bool stopWorking = false;

const int N_THREAD{ 1 };
static unsigned long long N_CLIENT_ID{ 10 };

shared_ptr<Room> room;

vector<shared_ptr<thread>> worker_threads;

enum class IO_TYPE;

void ProcessSignalAction(int sig_number)
{
	if (sig_number == SIGINT)
		stopWorking = true;
}

list<shared_ptr<RemoteClient>> deleteClinets;

void ProcessClientLeave(shared_ptr<RemoteClient> remoteClient, shared_ptr<DBMGR> p_DBMGR)
{
	unsigned long long leave_id = remoteClient->m_id;
	//Scene의 ObjectList에서 떠난 Player 제거
//	Scene::scene->PushDelete(remoteClient->m_pPlayer.get());

	// 에러 혹은 소켓 종료이다.
	// 해당 소켓은 제거해버리자. 
	remoteClient->tcpConnection.Close();
	bool b_expected = true;
	remoteClient->b_Enable.compare_exchange_strong(b_expected, false);
	
	{
//		lock_guard<recursive_mutex> lock_rc(RemoteClient::mx_rc);
//		RemoteClient::remoteClients.unsafe_erase(remoteClient.get());
		cout << "Client left. There are " << RemoteClient::remoteClients.size() << " connections.\n";
	}

	wchar_t* wstr = ChartoWChar(remoteClient->name);
	
	p_DBMGR->Set_UPDATE_PLAYER(
		wstr,
		remoteClient->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetPosition().x,
		remoteClient->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetPosition().z,
		remoteClient->m_pPlayer->GetRemainHP(),
		remoteClient->m_pPlayer->GetHealth(),
		remoteClient->m_clear_stage
	);
	
	delete[] wstr;
	//플레이어가 떠났다고 알림
	for (auto rc : RemoteClient::remoteClients) {
		if (!rc.second->b_Enable.load())
			continue;
		SC_REMOVE_PLAYER_PACKET send_packet;
		send_packet.size = sizeof(SC_REMOVE_PLAYER_PACKET);
		send_packet.type = E_PACKET::E_PACKET_SC_REMOVE_PLAYER;
		send_packet.id = leave_id;
		rc.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
	}

}

// IOCP를 준비한다.
Iocp iocp(N_THREAD); // N_THREAD개의 스레드 사용을 API에 힌트로 준다.

recursive_mutex mx_accept;
recursive_mutex mx_scene;

shared_ptr<Socket> p_listenSocket;
shared_ptr<RemoteClient> remoteClientCandidate;

void CloseServer();
void ProcessAccept();

void Process_Packet(shared_ptr<RemoteClient>& p_Client, char* p_Packet, shared_ptr<DBMGR> p_DBMGR);
void Process_Timer_Event(void* p_Client, IO_TYPE type);
void Process_Timer_Event_for_NPC(Character* p_NPC, IO_TYPE type);

void Worker_Thread()
{
	shared_ptr<DBMGR> db_mgr{ make_shared<DBMGR>() };
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
			/*		cout << " Send! - size : " << (int)p_readOverlapped->_buf[0] << endl;
					cout << " Send! - type : " << (int)p_readOverlapped->_buf[1] << endl;*/
					p_readOverlapped->m_isReadOverlapped = false;
					continue;
				}

				if (readEvent.lpCompletionKey == (ULONG_PTR)p_listenSocket.get()) // 리슨소켓이면
				{
					ProcessAccept(); // 클라이언트 연결 작업
				}
				else  // TCP 연결 소켓이면
				{
					if (IO_TYPE::IO_RECV != p_readOverlapped->m_ioType &&
						IO_TYPE::IO_TIMER_MONSTER_ANIMATION <= p_readOverlapped->m_ioType &&
						IO_TYPE::IO_TIMER_MONSTER_TARGET >= p_readOverlapped->m_ioType) {
						// Monster Event 처리
					//	cout << "Timer_Monster! index - " << readEvent.lpCompletionKey << endl;
						Process_Timer_Event_for_NPC((Character*)readEvent.lpCompletionKey, p_readOverlapped->m_ioType);
						continue;
					}

					// 처리할 클라이언트 받아오기
					shared_ptr<RemoteClient> remoteClient;
					remoteClient = RemoteClient::remoteClients[(RemoteClient*)readEvent.lpCompletionKey];

					// Timer Event 처리
					if (IO_TYPE::IO_RECV != p_readOverlapped->m_ioType) {
						// Player Event 처리
						if (remoteClient)
							Process_Timer_Event(remoteClient.get(), p_readOverlapped->m_ioType);
						continue;
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
							ProcessClientLeave(remoteClient, db_mgr);
						}
						else
						{
							// 이미 수신된 상태이다. 수신 완료된 것을 그냥 꺼내 쓰자.
							char* recv_buf = remoteClient->tcpConnection.m_recvOverlapped._buf;
							int recv_buf_Length = ec;

		
							{ // 패킷 처리
								int remain_data = recv_buf_Length + remoteClient->tcpConnection.m_prev_remain;
								while (remain_data > 0) {
									unsigned char packet_size = recv_buf[0];
									// 남은 데이터가 현재 처리할 패킷 크기보다 적으면 잘린 것이다. (혹은 딱 맞게 떨어진 것이다.)
									if (packet_size > remain_data)
										break;

									//패킷 처리
									Process_Packet(remoteClient, recv_buf, db_mgr);

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
								ProcessClientLeave(remoteClient, db_mgr);
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

	Scene::scene = make_shared<Scene>();
	Scene::scene->LoadSceneObb();

	std::cout << "Terrain Loding..." << std::endl;
	XMFLOAT3 xmf3Scale(1.0f, 0.38f, 1.0f);
	Scene::terrain = new HeightMapTerrain(_T("Terrain/terrain.raw"), 800, 800, xmf3Scale);
	Scene::terrain->SetPosition(-400, 0, -400);
	std::cout << "Terrain Loding Complete!" << std::endl;

	for (auto& room : Room::roomlist)
		room.second->start();

	Timer::Initialize();
	Timer::Reset();

	p_listenSocket = make_shared<Socket>(SocketType::Tcp);
	p_listenSocket->Bind(Endpoint("0.0.0.0", SERVERPORT));
	p_listenSocket->Listen();
	// IOCP에 추가한다.
	iocp.Add(*p_listenSocket, p_listenSocket.get());

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
	int MAX_MonsterTimerDelay = 10000;
	int MonsterTimerDelay = 0;
	while (true) {
		Timer::Tick(0.0f);
		for (auto& room : Room::roomlist) {
			room.second->update();
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


	for (auto i : RemoteClient::remoteClients)
	{
		i.second->tcpConnection.Close();
	}


	// 서버를 종료하기 위한 정리중
	cout << "서버를 종료하고 있습니다...\n";
	while (RemoteClient::remoteClients.size() > 0)
	{
		// I/O completion이 없는 상태의 RemoteClient를 제거한다.
		for (auto i = RemoteClient::remoteClients.begin(); i != RemoteClient::remoteClients.end(); ++i)
		{
			if (!i->second->tcpConnection.m_isReadOverlapped) {
				RemoteClient::remoteClients.unsafe_erase(i);
			}
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
				shared_ptr<RemoteClient> remoteClient = RemoteClient::remoteClients[(RemoteClient*)readEvent.lpCompletionKey];
				if (remoteClient)
				{
					remoteClient->tcpConnection.m_isReadOverlapped = false;
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

		{
			remoteClient->m_pPlayer = make_shared<Player>();
			remoteClient->m_pPlayer->start();
			remoteClient->m_pPlayer->GetComponent<PlayerMovementComponent>()->SetContext(Scene::terrain);
			remoteClient->m_pPlayer->GetComponent<PlayerMovementComponent>()->SetPosition(XMFLOAT3(-16.0f, Scene::terrain->GetHeight(-16.0f, 103.0f), 103.0f));
			remoteClient->m_pPlayer->SetPosition(XMFLOAT3(-16.0f, Scene::terrain->GetHeight(-16.0f, 103.0f), 103.0f));
			remoteClient->m_pPlayer->remoteClient = remoteClient.get();
		}
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
			RemoteClient::remoteClients.insert({ remoteClient.get(), remoteClient });
			cout << "Client joined. There are " << RemoteClient::remoteClients.size() << " connections.\n";
			
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


void Process_Packet(shared_ptr<RemoteClient>& p_Client, char* p_Packet, shared_ptr<DBMGR> p_DBMGR)
{
	switch (p_Packet[1]) // 패킷 타입
	{
	case E_PACKET::E_PACKET_CS_LOGIN: {
		CS_LOGIN_PACKET* recv_packet = reinterpret_cast<CS_LOGIN_PACKET*>(p_Packet);
		if (DBMGR::db_connection) {
			//DB에 데이터가 있는지 확인하고 아이디 생성 혹은 불러오기
			wchar_t* wname = ChartoWChar(recv_packet->name);
			if (!p_DBMGR->Get_SELECT_PLAYER(wname)) { // LOGIN_FAIL - 없는 ID
				// 새 ID 생성
				p_DBMGR->Set_INSERT_ID(wname);
				cout << "Login FAIL! (새 아이디 생성) " << endl;
				SC_LOGIN_FAIL_PACKET send_packet;
				send_packet.size = sizeof(SC_LOGIN_FAIL_PACKET);
				send_packet.type = E_PACKET::E_PACKET_SC_LOGIN_FAIL_PACKET;
				p_Client->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
				delete[] wname;
				break;
			}
			else {
				bool login = false;
				for (auto& p : RemoteClient::remoteClients) {
					if (!p.second->b_Enable.load())
						continue;
					if ((!strcmp(p.second->name, recv_packet->name)) && p.second->b_Enable) {
						login = true;
					}
				}
				if (login) { // LOGIN_FAIL - 이미 접속한 ID
					cout << "Login FAIL! (이미 접속) " << endl;
					SC_LOGIN_FAIL_PACKET send_packet;
					send_packet.size = sizeof(SC_LOGIN_FAIL_PACKET);
					send_packet.type = E_PACKET::E_PACKET_SC_LOGIN_FAIL_PACKET;
					p_Client->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
					delete[] wname;
					break;
				}
				else {	
					cout << "Login OK! (DB) " << endl;
					{
						SC_LOGIN_OK_PACKET send_packet;
						send_packet.size = sizeof(SC_LOGIN_OK_PACKET);
						send_packet.type = E_PACKET::E_PACKET_SC_LOGIN_OK_PACKET;
						p_Client->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
					}
					delete[] wname;
				}
			}
		}
		else {
			cout << "Login OK! " << endl;
			{
				SC_LOGIN_OK_PACKET send_packet;
				send_packet.size = sizeof(SC_LOGIN_OK_PACKET);
				send_packet.type = E_PACKET::E_PACKET_SC_LOGIN_OK_PACKET;
				p_Client->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
			}
		}

		memcpy(p_Client->name, recv_packet->name, sizeof(recv_packet->name));
		//id 부여
		p_Client->m_id = N_CLIENT_ID++;

		for (auto& room : Room::roomlist) { // roomList 갱신
			SC_ROOM_CREATE_PACKET send_packet;
			send_packet.size = sizeof(SC_ROOM_CREATE_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_ROOM_CREATE_PACKET;
			send_packet.roomNum = room.second->m_roomNum;
			memcpy(send_packet.hostName, room.second->hostName.c_str(), sizeof(room.second->hostName.c_str()));
			memcpy(send_packet.roomName, room.second->roomName.c_str(), sizeof(room.second->roomName.c_str()));
			send_packet.connection_playerNum = room.second->joinPlayerNum;
			cout << "Room::roomlist - " << send_packet.roomNum << endl;
			cout << "send_packet.hostName - " << send_packet.hostName << endl;
			cout << "send_packet.roomName - " << send_packet.roomName << endl;
			p_Client->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}

		break;
	}
	case E_PACKET::E_PACKET_CS_KEYDOWN: {
		CS_KEYDOWN_PACKET* recv_packet = reinterpret_cast<CS_KEYDOWN_PACKET*>(p_Packet);
		p_Client->m_KeyInput.keys[recv_packet->key] = TRUE;
	//	cout << recv_packet->key << " E_PACKET_CS_KEYDOWN" << endl;

		switch (recv_packet->key)
		{
		case VK_SPACE: {
			if (!p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetJumpAble())
				break;

			p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>()->Jump();
			for (auto& rc : Room::roomlist[p_Client->m_roomNum]->Clients) {
				if (!rc.second->b_Enable.load())
					continue;
				SC_PLAYER_ANIMATION_TYPE_PACKET send_packet;
				send_packet.size = sizeof(SC_PLAYER_ANIMATION_TYPE_PACKET);
				send_packet.type = E_PACKET::E_PACKET_SC_ANIMATION_TYPE_PLAYER;
				send_packet.id = p_Client->m_id;
				send_packet.Anitype = E_PLAYER_ANIMATION_TYPE::E_JUMP;
				rc.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
			}
		}
			break;
		case VK_RBUTTON: {
			p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>()->Dash();
			for (auto& rc : Room::roomlist[p_Client->m_roomNum]->Clients) {
				if (!rc.second->b_Enable.load())
					continue;
				SC_PLAYER_ANIMATION_TYPE_PACKET send_packet;
				send_packet.size = sizeof(SC_PLAYER_ANIMATION_TYPE_PACKET);
				send_packet.type = E_PACKET::E_PACKET_SC_ANIMATION_TYPE_PLAYER;
				send_packet.id = p_Client->m_id;
				send_packet.Anitype = E_PLAYER_ANIMATION_TYPE::E_DASH;
				rc.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
			}
		}
			break;
		case VK_LBUTTON: {
			for (auto& rc : Room::roomlist[p_Client->m_roomNum]->Clients) {
				if (!rc.second->b_Enable.load())
					continue;
				SC_PLAYER_ANIMATION_TYPE_PACKET send_packet;
				send_packet.size = sizeof(SC_PLAYER_ANIMATION_TYPE_PACKET);
				send_packet.type = E_PACKET::E_PACKET_SC_ANIMATION_TYPE_PLAYER;
				send_packet.id = p_Client->m_id;
				send_packet.Anitype = E_PLAYER_ANIMATION_TYPE::E_ATTACK0;
				rc.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
			}
			break;
		}
		default:
			break;
		}
		break;
	}
	case E_PACKET::E_PACKET_CS_KEYUP: {
		CS_KEYUP_PACKET* recv_packet = reinterpret_cast<CS_KEYUP_PACKET*>(p_Packet);
		p_Client->m_KeyInput.keys[recv_packet->key] = FALSE;
	//	cout << recv_packet->key << " E_PACKET_CS_KEYUP" << endl;
		break;
	}
	case E_PACKET::E_PACKET_CS_MOVE: {
		CS_MOVE_PACKET* recv_packet = reinterpret_cast<CS_MOVE_PACKET*>(p_Packet);
		XMFLOAT3 xmf3Dir{ XMFLOAT3(recv_packet->dirX, recv_packet->dirY, recv_packet->dirZ) };
		auto pm = p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>();
		pm->Move(xmf3Dir, true);
		break;
	}
	case E_PACKET::E_PACKET_CS_ROTATE: {
		CS_ROTATE_PACKET* recv_packet = reinterpret_cast<CS_ROTATE_PACKET*>(p_Packet);
		auto pm = p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>();
		float RotX = recv_packet->Add_Pitch;
		float RotY = recv_packet->Add_Yaw;
		float RotZ = recv_packet->Add_Roll;

		XMFLOAT3 xmf3Look = pm->GetLookVector();
		XMFLOAT3 xmf3Up = pm->GetUpVector();
		XMFLOAT3 xmf3Right = pm->GetRightVector();

		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(RotY));
		pm->SetLookVector(Vector3::TransformNormal(xmf3Look, xmmtxRotate));
		pm->SetRightVector(Vector3::TransformNormal(xmf3Right, xmmtxRotate));

		pm->is_Rotate = true;
		break;
	}
	case E_PACKET::E_PACKET_CS_TEMP_HIT_MONSTER_PACKET: {
		CS_TEMP_HIT_MONSTER_PACKET* recv_packet = reinterpret_cast<CS_TEMP_HIT_MONSTER_PACKET*>(p_Packet);
		Character* Monster;
		{
			auto p = find_if(Scene::scene->MonsterObjects.begin(),
				Scene::scene->MonsterObjects.end(),
				[&recv_packet](Object* lhs) {
					return dynamic_cast<Character*>(lhs)->num == recv_packet->monster_id;
				});

			if (p == Scene::scene->MonsterObjects.end())
				break;

			Monster = dynamic_cast<Character*>(*p);
		}

		Monster->GetHit(recv_packet->hit_damage);

		// 다른 클라이언트들에게 남은 HP 정보 통신
		for (auto& rc : Room::roomlist[p_Client->m_roomNum]->Clients) {
			if (!rc.second->b_Enable.load())
				continue;
			SC_TEMP_HIT_MONSTER_PACKET send_packet;
			send_packet.size = sizeof(SC_TEMP_HIT_MONSTER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_TEMP_HIT_MONSTER_PACKET;
			send_packet.monster_id = recv_packet->monster_id;
			send_packet.remain_hp = Monster->GetRemainHP();
			rc.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
		break;
	}
	case E_PACKET::E_PACKET_CS_TEMP_HIT_PLAYER_PACKET: {
		CS_TEMP_HIT_PLAYER_PACKET* recv_packet = reinterpret_cast<CS_TEMP_HIT_PLAYER_PACKET*>(p_Packet);
		shared_ptr<Player> player;
		for (auto& rc : Room::roomlist[p_Client->m_roomNum]->Clients) {
			if (!rc.second->b_Enable.load())
				continue;
			if (rc.second->m_id == recv_packet->player_id)
				player = rc.second->m_pPlayer;
		}

		player->GetHit(recv_packet->hit_damage);

		// 다른 클라이언트들에게 남은 HP 정보 통신
		for (auto& rc : Room::roomlist[p_Client->m_roomNum]->Clients) {
			if (!rc.second->b_Enable.load())
				continue;
			SC_TEMP_HIT_PLAYER_PACKET send_packet;
			send_packet.size = sizeof(SC_TEMP_HIT_PLAYER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_TEMP_HIT_PLAYER_PACKET;
			send_packet.player_id = recv_packet->player_id;
			send_packet.remain_hp = player->GetRemainHP();
			rc.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
		break;
	}
	case E_PACKET::E_PACKET_CS_CHAT_PACKET: {
		CS_CHAT_PACKET* recv_packet = reinterpret_cast<CS_CHAT_PACKET*>(p_Packet);
		for (auto& rc : Room::roomlist[p_Client->m_roomNum]->Clients) {
			if (!rc.second->b_Enable.load())
				continue;
			if (rc.second->m_id == p_Client->m_id)
				continue;
			SC_CHAT_PACKET send_packet;
			send_packet.size = sizeof(SC_CHAT_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_CHAT_PACKET;
			memcpy(send_packet.chat, recv_packet->chat, sizeof(recv_packet->chat));
			rc.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
		break;
	}
	case E_PACKET_CS_ROOM_CREATE_PACKET: {
		CS_ROOM_CREATE_PACKET* recv_packet = reinterpret_cast<CS_ROOM_CREATE_PACKET*>(p_Packet);
		// 임시 방 생성
		int rN = Room::g_roomNum++;
		auto p_room = make_shared<Room>();
		p_room->roomName = std::string{ recv_packet->roomName };
		p_room->hostName = std::string{ p_Client->name };
		p_room->m_roomNum = rN;
		p_room->start();
		Room::roomlist.insert({ rN, p_room });
		for (auto& rc : RemoteClient::remoteClients) {
			if (!rc.second->b_Enable.load())
				continue;
			SC_ROOM_CREATE_PACKET send_packet;
			send_packet.size = sizeof(SC_ROOM_CREATE_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_ROOM_CREATE_PACKET;
			send_packet.roomNum = rN;
			memcpy(send_packet.hostName, p_Client->name, sizeof(p_Client->name));
			memcpy(send_packet.roomName, recv_packet->roomName, sizeof(recv_packet->roomName));
			send_packet.connection_playerNum = 0;
			rc.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
		break;
	}
	case E_PACKET_CS_ROOM_JOIN_PACKET: {
		CS_ROOM_JOIN_PACKET* recv_packet = reinterpret_cast<CS_ROOM_JOIN_PACKET*>(p_Packet);

	/*	Room::roomlist[recv_packet->roomNum]->Clients.insert({ p_Client->m_id, p_Client });
		p_Client->m_pPlayer->m_roomNum = recv_packet->roomNum;*/

		Room::roomlist[recv_packet->roomNum]->Clients.insert({ p_Client->m_id, p_Client });
		p_Client->m_roomNum = recv_packet->roomNum;

		{ // Room Join Message
			SC_ROOM_JOIN_OK_PACKET send_packet;
			send_packet.size = sizeof(SC_ROOM_JOIN_OK_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_ROOM_JOIN_OK_PACKET;
			send_packet.id = p_Client->m_id;
			p_Client->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
		
		//// LOGIN_OK
		//p_Client->m_pPlayer = make_shared<Player>();
		//p_Client->m_pPlayer->start();
		//p_Client->m_pPlayer->remoteClient = p_Client.get();
		//bool expected = false;
		//p_Client->b_Login.compare_exchange_strong(expected, true);
		//p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>()->SetContext(Scene::terrain);

		//p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>()
		//	->SetPosition(XMFLOAT3{ (float)p_DBMGR->player_x, Scene::terrain->GetHeight((float)p_DBMGR->player_x, (float)p_DBMGR->player_z) ,(float)p_DBMGR->player_z });
		//p_Client->m_pPlayer->SetPosition(XMFLOAT3{ (float)p_DBMGR->player_x, Scene::terrain->GetHeight((float)p_DBMGR->player_x, (float)p_DBMGR->player_z) ,(float)p_DBMGR->player_z });

		//p_Client->m_pPlayer->SetHealth((int)p_DBMGR->player_Maxhp);
		//p_Client->m_pPlayer->SetRemainHP((int)p_DBMGR->player_hp);
		//p_Client->m_clear_stage = (int)p_DBMGR->player_clear_stage;

		//p_Client->m_pPlayer->m_roomNum = p_Client->m_roomNum;

		//{ // 접속한 클라이언트 본인 정보 송신
		//	SC_LOGIN_INFO_PACKET send_packet;
		//	send_packet.size = sizeof(SC_LOGIN_INFO_PACKET);
		//	send_packet.type = E_PACKET::E_PACKET_SC_LOGIN_INFO;
		//	send_packet.id = p_Client->m_id;
		//	send_packet.maxHp = p_Client->m_pPlayer->GetHealth();
		//	send_packet.remainHp = p_Client->m_pPlayer->GetRemainHP();
		//	send_packet.x = p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetPosition().x;
		//	send_packet.y = p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetPosition().y;
		//	send_packet.z = p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetPosition().z;
		//	send_packet.clearStage = p_Client->m_clear_stage;
		//	p_Client->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		//}

		//// 접속한 클라이언트에게 모든 플레이어 정보 송신
		//for (auto& rc : Room::roomlist[recv_packet->roomNum]->Clients) {
		//	if (!rc.second->b_Enable.load())
		//		continue;
		//	if (rc.second->m_id == p_Client->m_id)
		//		continue;
		//	SC_ADD_PLAYER_PACKET send_packet;
		//	send_packet.size = sizeof(SC_ADD_PLAYER_PACKET);
		//	send_packet.type = E_PACKET::E_PACKET_SC_ADD_PLAYER;
		//	send_packet.id = rc.second->m_id;
		//	memcpy(send_packet.name, rc.second->name, sizeof(rc.second->name));
		//	send_packet.maxHp = rc.second->m_pPlayer->GetHealth();
		//	send_packet.remainHp = rc.second->m_pPlayer->GetRemainHP();
		//	send_packet.x = rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetPosition().x;
		//	send_packet.y = rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetPosition().y;
		//	send_packet.z = rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetPosition().z;
		//	send_packet.clearStage = rc.second->m_clear_stage;
		//	send_packet.type = 0;
		//	p_Client->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		//}

		//// 다른 클라이언트들에게 접속한 클라이언트 정보 송신
		//for (auto& rc : Room::roomlist[recv_packet->roomNum]->Clients) {
		//	if (!rc.second->b_Enable.load())
		//		continue;
		//	if (rc.second->m_id == p_Client->m_id)
		//		continue;
		//	SC_ADD_PLAYER_PACKET send_packet;
		//	send_packet.size = sizeof(SC_ADD_PLAYER_PACKET);
		//	send_packet.type = E_PACKET::E_PACKET_SC_ADD_PLAYER;
		//	send_packet.id = p_Client->m_id;
		//	memcpy(send_packet.name, p_Client->name, sizeof(p_Client->name));
		//	send_packet.maxHp = p_Client->m_pPlayer->GetHealth();
		//	send_packet.remainHp = p_Client->m_pPlayer->GetRemainHP();
		//	send_packet.x = p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetPosition().x;
		//	send_packet.y = p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetPosition().y;
		//	send_packet.z = p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetPosition().z;
		//	send_packet.clearStage = p_Client->m_clear_stage;
		//	send_packet.type = 0;
		//	rc.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		//}


		break;
	}
	default:
		break;
	}
}

void Process_Timer_Event(void* p_Client, IO_TYPE type)
{
	switch (type)
	{
	case IO_TYPE::IO_TIMER_PLAYER_ANIMATION: {
		for (auto& rc_to : RemoteClient::remoteClients) {
			if (!rc_to.second->b_Enable.load())
				continue;
			SC_PLAYER_ANIMATION_TYPE_PACKET send_packet;
			send_packet.size = sizeof(SC_PLAYER_ANIMATION_TYPE_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_ANIMATION_TYPE_PLAYER;
			send_packet.id = ((RemoteClient*)p_Client)->m_id;
			send_packet.Anitype = (char)((RemoteClient*)p_Client)->m_pPlayer->PresentAniType;
			rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
	}
		break;
	case IO_TYPE::IO_TIMER_PLAYER_MOVE: {
		auto rc_pos = ((RemoteClient*)p_Client)->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetPosition();
		for (auto& rc_to : RemoteClient::remoteClients) {
			if (!rc_to.second->b_Enable.load())
				continue;
			SC_MOVE_PLAYER_PACKET send_packet;
			send_packet.size = sizeof(SC_MOVE_PLAYER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_MOVE_PLAYER;
			send_packet.id = ((RemoteClient*)p_Client)->m_id;
			send_packet.x = rc_pos.x;
			send_packet.y = rc_pos.y;
			send_packet.z = rc_pos.z;
			rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
	}
		break;
	case IO_TYPE::IO_TIMER_PLAYER_LOOK: {
		XMFLOAT3 xmf3FinalLook = ((RemoteClient*)p_Client)->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetLookVector();
		for (auto& rc_to : RemoteClient::remoteClients) {
			if (!rc_to.second->b_Enable.load())
				continue;
			SC_LOOK_PLAYER_PACKET send_packet;
			send_packet.size = sizeof(SC_LOOK_PLAYER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_LOOK_PLAYER;
			send_packet.id = ((RemoteClient*)p_Client)->m_id;
			send_packet.x = xmf3FinalLook.x;
			send_packet.y = xmf3FinalLook.y;
			send_packet.z = xmf3FinalLook.z;
			rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
	}
		break;
	case IO_TYPE::IO_TIMER_MONSTER_ANIMATION: {
		//Monster Animation
		if (((Character*)p_Client)->OldAniType.load() != ((Character*)p_Client)->PresentAniType) {
			for (auto& rc_to : RemoteClient::remoteClients) {
				if (!rc_to.second->b_Enable.load())
					continue;
				SC_MONSTER_ANIMATION_TYPE_PACKET send_packet;
				send_packet.size = sizeof(SC_MONSTER_ANIMATION_TYPE_PACKET);
				send_packet.type = E_PACKET::E_PACKET_SC_ANIMATION_TYPE_MONSTER;
				send_packet.id = ((Character*)p_Client)->num;
				send_packet.Anitype = ((Character*)p_Client)->PresentAniType;
				rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
			}
			int expected = ((Character*)p_Client)->OldAniType.load();
			((Character*)p_Client)->OldAniType.compare_exchange_strong(expected, ((Character*)p_Client)->PresentAniType.load());
		}
	}
		break;
	case IO_TYPE::IO_TIMER_MONSTER_MOVE: {
		//Monster Pos
		for (auto& rc_to : RemoteClient::remoteClients) {
			if (!rc_to.second->b_Enable.load())
				continue;
			SC_MOVE_MONSTER_PACKET send_packet;
			send_packet.size = sizeof(SC_MOVE_MONSTER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_MOVE_MONSTER_PACKET;
			send_packet.id = ((Character*)p_Client)->num;
			send_packet.x = ((Character*)p_Client)->GetPosition().x;
			send_packet.y = ((Character*)p_Client)->GetPosition().y;
			send_packet.z = ((Character*)p_Client)->GetPosition().z;
			rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
	}
		break;
	case IO_TYPE::IO_TIMER_MONSTER_WANDER: {
		//WanderPosition
		for (auto& rc_to : RemoteClient::remoteClients) {
			if (!rc_to.second->b_Enable.load())
				continue;
			SC_LOOK_MONSTER_PACKET send_packet;
			send_packet.size = sizeof(SC_LOOK_MONSTER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_LOOK_MONSTER_PACKET;
			send_packet.id = ((Character*)p_Client)->num;
			send_packet.x = ((Character*)p_Client)->GetComponent<CloseTypeFSMComponent>()->WanderPosition.x;
			send_packet.y = ((Character*)p_Client)->GetComponent<CloseTypeFSMComponent>()->WanderPosition.y;
			send_packet.z = ((Character*)p_Client)->GetComponent<CloseTypeFSMComponent>()->WanderPosition.z;
			rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
	}
		break;
	case IO_TYPE::IO_TIMER_MONSTER_TARGET: {
		//Monster Target
		if (((Character*)p_Client)->GetComponent<CloseTypeFSMComponent>()->GetTargetPlayer()) {
			for (auto& rc_to : RemoteClient::remoteClients) {
				if (!rc_to.second->b_Enable.load())
					continue;
				SC_AGGRO_PLAYER_PACKET send_packet;
				send_packet.size = sizeof(SC_AGGRO_PLAYER_PACKET);
				send_packet.type = E_PACKET::E_PACKET_SC_AGGRO_PLAYER_PACKET;
				send_packet.player_id = ((Player*)(((Character*)p_Client)
					->GetComponent<CloseTypeFSMComponent>()->GetTargetPlayer()))->remoteClient->m_id;
				send_packet.monster_id = ((Character*)p_Client)->num;
				rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
			}
		}
	}
		break;
	default:
		break;
	}
}

void Process_Timer_Event_for_NPC(Character* p_NPC, IO_TYPE type)
{
	switch (type)
	{
	case IO_TYPE::IO_TIMER_MONSTER_ANIMATION: {
		//Monster Animation
		if (p_NPC->OldAniType.load() != p_NPC->PresentAniType) {
			for (auto& rc_to : RemoteClient::remoteClients) {
				if (!rc_to.second->b_Enable.load())
					continue;
				SC_MONSTER_ANIMATION_TYPE_PACKET send_packet;
				send_packet.size = sizeof(SC_MONSTER_ANIMATION_TYPE_PACKET);
				send_packet.type = E_PACKET::E_PACKET_SC_ANIMATION_TYPE_MONSTER;
				send_packet.id = p_NPC->num;
				send_packet.Anitype = p_NPC->PresentAniType.load();
				rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
			}
			int expected = p_NPC->OldAniType.load();
			p_NPC->OldAniType.compare_exchange_strong(expected, p_NPC->PresentAniType.load());
		//	p_NPC->OldAniType = p_NPC->PresentAniType;
		}
	}
											break;
	case IO_TYPE::IO_TIMER_MONSTER_MOVE: {
		//Monster Pos
		for (auto& rc_to : RemoteClient::remoteClients) {
			if (!rc_to.second->b_Enable.load())
				continue;
			SC_MOVE_MONSTER_PACKET send_packet;
			send_packet.size = sizeof(SC_MOVE_MONSTER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_MOVE_MONSTER_PACKET;
			send_packet.id = p_NPC->num;
			send_packet.x = p_NPC->GetPosition().x;
			send_packet.y = p_NPC->GetPosition().y;
			send_packet.z = p_NPC->GetPosition().z;
			rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
	}
									   break;
	case IO_TYPE::IO_TIMER_MONSTER_WANDER: {
		//WanderPosition
		for (auto& rc_to : RemoteClient::remoteClients) {
			if (!rc_to.second->b_Enable.load())
				continue;
			SC_LOOK_MONSTER_PACKET send_packet;
			send_packet.size = sizeof(SC_LOOK_MONSTER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_LOOK_MONSTER_PACKET;
			send_packet.id = p_NPC->num;
			send_packet.x = p_NPC->GetComponent<CloseTypeFSMComponent>()->WanderPosition.x;
			send_packet.y = p_NPC->GetComponent<CloseTypeFSMComponent>()->WanderPosition.y;
			send_packet.z = p_NPC->GetComponent<CloseTypeFSMComponent>()->WanderPosition.z;
			rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
	}
										 break;
	case IO_TYPE::IO_TIMER_MONSTER_TARGET: {
		//Monster Target
		if (p_NPC->GetComponent<CloseTypeFSMComponent>()->GetTargetPlayer()) {
			for (auto& rc_to : RemoteClient::remoteClients) {
				if (!rc_to.second->b_Enable.load())
					continue;
				SC_AGGRO_PLAYER_PACKET send_packet;
				send_packet.size = sizeof(SC_AGGRO_PLAYER_PACKET);
				send_packet.type = E_PACKET::E_PACKET_SC_AGGRO_PLAYER_PACKET;
				send_packet.player_id = ((Player*)(p_NPC
					->GetComponent<CloseTypeFSMComponent>()->GetTargetPlayer()))->remoteClient->m_id;
				send_packet.monster_id = p_NPC->num;
				rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
			}
		}
	}
										 break;
	default:
		break;
	}
}