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

#include "Input.h"
#include "Timer.h"

#include "remoteClients/RemoteClient.h"
#include "Scene.h"
#include "Terrain.h"

#include "Components/PlayerMovementComponent.h"
#include "Components/CloseTypeFSMComponent.h"
#include "Components/SphereCollideComponent.h"
#include "Components/BoxCollideComponent.h"

using namespace std;
using namespace concurrency;

volatile bool stopWorking = false;

const int N_THREAD{ 1 };
static unsigned long long N_CLIENT_ID{ 10 };

shared_ptr<Scene> scene;

vector<shared_ptr<thread>> worker_threads;

enum class IO_TYPE;

void ProcessSignalAction(int sig_number)
{
	if (sig_number == SIGINT)
		stopWorking = true;
}

list<shared_ptr<RemoteClient>> deleteClinets;

void ProcessClientLeave(shared_ptr<RemoteClient> remoteClient)
{
	unsigned long long leave_id = remoteClient->m_id;
	//Scene의 ObjectList에서 떠난 Player 제거
//	Scene::scene->PushDelete(remoteClient->m_pPlayer.get());

	// 에러 혹은 소켓 종료이다.
	// 해당 소켓은 제거해버리자. 
	remoteClient->tcpConnection.Close();
	remoteClient->b_Enable = false;
	
	{
//		lock_guard<recursive_mutex> lock_rc(RemoteClient::mx_rc);
//		RemoteClient::remoteClients.unsafe_erase(remoteClient.get());
		cout << "Client left. There are " << RemoteClient::remoteClients.size() << " connections.\n";
	}

	//플레이어가 떠났다고 알림
	for (auto rc : RemoteClient::remoteClients) {
		if (!rc.second->b_Enable)
			continue;
		SC_REMOVE_PLAYER_PACKET send_packet;
		send_packet.size = sizeof(SC_REMOVE_PLAYER_PACKET);
		send_packet.type = E_PACKET::E_PACKET_SC_REMOVE_PLAYER;
		send_packet.id = leave_id;
		rc.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
	}

}

// IOCP를 준비한다.
Iocp iocp(N_THREAD); // 5개의 스레드 사용을 API에 힌트로 준다.

recursive_mutex mx_accept;
recursive_mutex mx_scene;

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
			/*		cout << " Send! - size : " << (int)p_readOverlapped->_buf[0] << endl;
					cout << " Send! - type : " << (int)p_readOverlapped->_buf[1] << endl;*/
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
					remoteClient = RemoteClient::remoteClients[(RemoteClient*)readEvent.lpCompletionKey];

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

	Scene::scene->LoadSceneObb();

	XMFLOAT3 xmf3Scale(1.0f, 0.38f, 1.0f);
	Scene::terrain = new HeightMapTerrain(_T("Terrain/terrain.raw"), 800, 800, xmf3Scale);
	Scene::terrain->SetPosition(-400, 0, -400);

	Object* TempObject = NULL;
	TempObject = new Goblin(MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(-9.0f, Scene::terrain->GetHeight(-9.0f, 9.0f), 87);
	((Goblin*)TempObject)->num = 101;
	TempObject = new Goblin(MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(-1.0f, Scene::terrain->GetHeight(-1.0f, 42.0f), 42.0f);
	((Goblin*)TempObject)->num = 102;
	TempObject = new Goblin(MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(16.0f, Scene::terrain->GetHeight(16.0f, 34.0f), 34.0f);
	((Goblin*)TempObject)->num = 103;
	TempObject = new Goblin(MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(53.0f, Scene::terrain->GetHeight(53.0f, 43.0f), 43.0f);
	((Goblin*)TempObject)->num = 104;
	TempObject = new Goblin(MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(89.0f, Scene::terrain->GetHeight(89.0f, 33.0f), 33.0f);
	((Goblin*)TempObject)->num = 105;
	TempObject = new Goblin(MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(113.0f, Scene::terrain->GetHeight(113.0f, 20.0f), 20.0f);
	((Goblin*)TempObject)->num = 106;

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
		Scene::scene->update();

		for (auto& rc : RemoteClient::remoteClients) {
			if (!rc.second->b_Enable)
				continue;

			// Animation Packet
			{ 
				if (rc.second->m_KeyInput.keys['w'] || rc.second->m_KeyInput.keys['W'] ||
					rc.second->m_KeyInput.keys['s'] || rc.second->m_KeyInput.keys['S'] ||
					rc.second->m_KeyInput.keys['a'] || rc.second->m_KeyInput.keys['A'] ||
					rc.second->m_KeyInput.keys['d'] || rc.second->m_KeyInput.keys['D']) {
					if (rc.second->m_KeyInput.keys[16]) { // LSHIFT
						rc.second->m_pPlayer->PresentAniType = E_PLAYER_ANIMATION_TYPE::E_WALK;
					}
					else {
						rc.second->m_pPlayer->PresentAniType = E_PLAYER_ANIMATION_TYPE::E_RUN;
					}
				}
				else {
					//	if (!((Player*)gameObject)->GetComponent<AttackComponent>()->During_Attack) 컴포넌트 추가 해야함
					rc.second->m_pPlayer->PresentAniType = E_PLAYER_ANIMATION_TYPE::E_IDLE;
				}

				if (rc.second->m_pPlayer->OldAniType != rc.second->m_pPlayer->PresentAniType) {
					for (auto& rc_to : RemoteClient::remoteClients) {
						if (!rc_to.second->b_Enable)
							continue;
						SC_PLAYER_ANIMATION_TYPE_PACKET send_packet;
						send_packet.size = sizeof(SC_PLAYER_ANIMATION_TYPE_PACKET);
						send_packet.type = E_PACKET::E_PACKET_SC_ANIMATION_TYPE_PLAYER;
						send_packet.id = rc.second->m_id;
						send_packet.Anitype = (char)rc.second->m_pPlayer->PresentAniType;
						rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
					}
					rc.second->m_pPlayer->OldAniType = rc.second->m_pPlayer->PresentAniType;
				}
			}

			rc.second->m_pPlayer->update();
			
			// Move Packet
			{ 
				auto vel = rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetVelocity();

				if (!Vector3::Length(vel))
					continue;

				auto rc_pos = rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetPosition();
				for (auto& rc_to : RemoteClient::remoteClients) {
					if (!rc_to.second->b_Enable)
						continue;
					SC_MOVE_PLAYER_PACKET send_packet;
					send_packet.size = sizeof(SC_MOVE_PLAYER_PACKET);
					send_packet.type = E_PACKET::E_PACKET_SC_MOVE_PLAYER;
					send_packet.id = rc.second->m_id;
					send_packet.x = rc_pos.x;
					send_packet.y = rc_pos.y;
					send_packet.z = rc_pos.z;
					rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
				}
			}

			// Camera Look Packet
			if (rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->is_Rotate){ 
				XMFLOAT3 xmf3FinalLook = rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetLookVector();
				for (auto& rc_to : RemoteClient::remoteClients) {
					if (!rc_to.second->b_Enable)
						continue;
					SC_LOOK_PLAYER_PACKET send_packet;
					send_packet.size = sizeof(SC_LOOK_PLAYER_PACKET);
					send_packet.type = E_PACKET::E_PACKET_SC_LOOK_PLAYER;
					send_packet.id = rc.second->m_id;
					send_packet.x = xmf3FinalLook.x;
					send_packet.y = xmf3FinalLook.y;
					send_packet.z = xmf3FinalLook.z;
					rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
				}
				rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->is_Rotate = false;
			}
		}

		//Monster test
		for (auto monster : Scene::scene->MonsterObjects) {
			if (monster->GetRemainHP() < 0.f)
				continue;

			if (((Character*)monster)->GetRemainHP() > 0.f) {
				//Monster Pos
				for (auto& rc_to : RemoteClient::remoteClients) {
					if (!rc_to.second->b_Enable)
						continue;
					SC_MOVE_MONSTER_PACKET send_packet;
					send_packet.size = sizeof(SC_MOVE_MONSTER_PACKET);
					send_packet.type = E_PACKET::E_PACKET_SC_MOVE_MONSTER_PACKET;
					send_packet.id = ((Goblin*)monster)->num;
					send_packet.x = monster->GetPosition().x;
					send_packet.y = monster->GetPosition().y;
					send_packet.z = monster->GetPosition().z;
					rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
				}

				//WanderPosition
				for (auto& rc_to : RemoteClient::remoteClients) {
					if (!rc_to.second->b_Enable)
						continue;
					SC_LOOK_MONSTER_PACKET send_packet;
					send_packet.size = sizeof(SC_LOOK_MONSTER_PACKET);
					send_packet.type = E_PACKET::E_PACKET_SC_LOOK_MONSTER_PACKET;
					send_packet.id = ((Goblin*)monster)->num;
					send_packet.x = monster->GetComponent<CloseTypeFSMComponent>()->WanderPosition.x;
					send_packet.y = monster->GetComponent<CloseTypeFSMComponent>()->WanderPosition.y;
					send_packet.z = monster->GetComponent<CloseTypeFSMComponent>()->WanderPosition.z;
					rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
				}

				//Monster Animation
				if (((Character*)monster)->OldAniType != ((Character*)monster)->PresentAniType) {
					for (auto& rc_to : RemoteClient::remoteClients) {
						if (!rc_to.second->b_Enable)
							continue;
						SC_MONSTER_ANIMATION_TYPE_PACKET send_packet;
						send_packet.size = sizeof(SC_MONSTER_ANIMATION_TYPE_PACKET);
						send_packet.type = E_PACKET::E_PACKET_SC_ANIMATION_TYPE_MOSTER;
						send_packet.id = ((Goblin*)monster)->num;
						send_packet.Anitype = ((Character*)monster)->PresentAniType;
						rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
					}
					((Character*)monster)->OldAniType = ((Character*)monster)->PresentAniType;
				}

				if (RemoteClient::remoteClients.empty())
					continue;

				//Monster Target
				if ((Goblin*)(monster)->GetComponent<CloseTypeFSMComponent>()->GetTargetPlayer()) {
					for (auto& rc_to : RemoteClient::remoteClients) {
						if (!rc_to.second->b_Enable)
							continue;
						SC_AGGRO_PLAYER_PACKET send_packet;
						send_packet.size = sizeof(SC_AGGRO_PLAYER_PACKET);
						send_packet.type = E_PACKET::E_PACKET_SC_AGGRO_PLAYER_PACKET;
						send_packet.player_id = ((Player*)((Goblin*)(monster)
							->GetComponent<CloseTypeFSMComponent>()->GetTargetPlayer()))->remoteClient->m_id;
						send_packet.monster_id = ((Goblin*)monster)->num;
						rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
					}
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
			lock_guard<recursive_mutex> lock(mx_accept);
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


void Process_Packet(shared_ptr<RemoteClient>& p_Client, char* p_Packet)
{
	switch (p_Packet[1]) // 패킷 타입
	{
	case E_PACKET::E_PACKET_CS_LOGIN: {
		CS_LOGIN_PACKET* recv_packet = reinterpret_cast<CS_LOGIN_PACKET*>(p_Packet);
		memcpy(p_Client->name,recv_packet->name,sizeof(recv_packet->name));

		//id 부여
		p_Client->m_id = N_CLIENT_ID++;

		{ // 접속한 클라이언트 본인 정보 송신
			SC_LOGIN_INFO_PACKET send_packet;
			send_packet.size = sizeof(SC_LOGIN_INFO_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_LOGIN_INFO;
			send_packet.id = p_Client->m_id;
			p_Client->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}

		// 접속한 클라이언트에게 모든 플레이어 정보 송신
		for (auto& rc : RemoteClient::remoteClients) {
			if (!rc.second->b_Enable)
				continue;
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
		for (auto& rc : RemoteClient::remoteClients) {
			if (!rc.second->b_Enable)
				continue;
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
	//	cout << recv_packet->key << " E_PACKET_CS_KEYDOWN" << endl;

		switch (recv_packet->key)
		{
		case VK_SPACE: {
			p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>()->Jump();
			for (auto& rc : RemoteClient::remoteClients) {
				if (!rc.second->b_Enable)
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
			for (auto& rc : RemoteClient::remoteClients) {
				if (!rc.second->b_Enable)
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
			for (auto& rc : RemoteClient::remoteClients) {
				if (!rc.second->b_Enable)
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
		for (auto& rc : RemoteClient::remoteClients) {
			if (!rc.second->b_Enable)
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
		for (auto& rc : RemoteClient::remoteClients) {
			if (!rc.second->b_Enable)
				continue;
			if (rc.second->m_id == recv_packet->player_id)
				player = rc.second->m_pPlayer;
		}

		player->GetHit(recv_packet->hit_damage);

		// 다른 클라이언트들에게 남은 HP 정보 통신
		for (auto& rc : RemoteClient::remoteClients) {
			if (!rc.second->b_Enable)
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

		for (auto& rc : RemoteClient::remoteClients) {
			if (!rc.second->b_Enable)
				continue;
			if (rc.second->m_id == p_Client->m_id)
				continue;
			SC_CHAT_PACKET send_packet;
			send_packet.size = sizeof(SC_CHAT_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_CHAT_PACKET;
			memcpy(send_packet.name, p_Client->name, sizeof(p_Client->name));
			memcpy(send_packet.chat, recv_packet->chat, sizeof(recv_packet->chat));
			rc.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
	}
		break;
	default:
		break;
	}
}