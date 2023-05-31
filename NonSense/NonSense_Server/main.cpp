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
#include "Terrain.h"

#include "Components/PlayerMovementComponent.h"
#include "Components/CloseTypeFSMComponent.h"
#include "Components/SphereCollideComponent.h"
#include "Components/BoxCollideComponent.h"

// unordered map Ž�� ����ϱ� ���� index �߰�
// Player ��, Monster �� �þ� ó��
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
	//Scene�� ObjectList���� ���� Player ����
//	Scene::scene->PushDelete(remoteClient->m_pPlayer.get());

	// ���� Ȥ�� ���� �����̴�.
	// �ش� ������ �����ع�����. 
	remoteClient->tcpConnection.Close();
	remoteClient->b_Enable = false;
	
	{
//		lock_guard<recursive_mutex> lock_rc(RemoteClient::mx_rc);
//		RemoteClient::remoteClients.unsafe_erase(remoteClient.get());
		cout << "Client left. There are " << RemoteClient::remoteClients.size() << " connections.\n";
	}

	//�÷��̾ �����ٰ� �˸�
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

// IOCP�� �غ��Ѵ�.
Iocp iocp(N_THREAD); // N_THREAD���� ������ ����� API�� ��Ʈ�� �ش�.

recursive_mutex mx_accept;
recursive_mutex mx_scene;

shared_ptr<Socket> p_listenSocket;
shared_ptr<RemoteClient> remoteClientCandidate;

void CloseServer();
void ProcessAccept();

void Process_Packet(shared_ptr<RemoteClient>& p_Client, char* p_Packet);
void Process_Timer_Event(void* p_Client, IO_TYPE type);
void Process_Timer_Event_for_NPC(Character* p_NPC, IO_TYPE type);

void Worker_Thread()
{
	try
	{
		while (!stopWorking) {
			// I/O �Ϸ� �̺�Ʈ�� ���� ������ ��ٸ��ϴ�.
			IocpEvents readEvents;
			iocp.Wait(readEvents, 100);

			// ���� �̺�Ʈ ������ ó���մϴ�.
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

				if (readEvent.lpCompletionKey == (ULONG_PTR)p_listenSocket.get()) // ���������̸�
				{
					ProcessAccept(); // Ŭ���̾�Ʈ ���� �۾�
				}
				else  // TCP ���� �����̸�
				{
					if (IO_TYPE::IO_RECV != p_readOverlapped->m_ioType &&
						IO_TYPE::IO_TIMER_MONSTER_ANIMATION <= p_readOverlapped->m_ioType &&
						IO_TYPE::IO_TIMER_MONSTER_TARGET >= p_readOverlapped->m_ioType) {
						// Monster Event ó��
					//	cout << "Timer_Monster! index - " << readEvent.lpCompletionKey << endl;
						Process_Timer_Event_for_NPC((Character*)readEvent.lpCompletionKey, p_readOverlapped->m_ioType);
						continue;
					}

					// ó���� Ŭ���̾�Ʈ �޾ƿ���
					shared_ptr<RemoteClient> remoteClient;
					remoteClient = RemoteClient::remoteClients[(RemoteClient*)readEvent.lpCompletionKey];

					// Timer Event ó��
					if (IO_TYPE::IO_RECV != p_readOverlapped->m_ioType) {
						// Player Event ó��
						if (remoteClient)
							Process_Timer_Event(remoteClient.get(), p_readOverlapped->m_ioType);
						continue;
					}

					//
					if (remoteClient)
					{
						// �̹� ���ŵ� �����̴�. ���� �Ϸ�� ���� �׳� ���� ����.
						remoteClient->tcpConnection.m_isReadOverlapped = false;
						int ec = readEvent.dwNumberOfBytesTransferred;

						if (ec <= 0)
						{
							// ���� ����� 0 �� TCP ������ ������...
							// Ȥ�� ���� �� ���� ������ �� �����̴�...
							ProcessClientLeave(remoteClient);
						}
						else
						{
							// �̹� ���ŵ� �����̴�. ���� �Ϸ�� ���� �׳� ���� ����.
							char* recv_buf = remoteClient->tcpConnection.m_recvOverlapped._buf;
							int recv_buf_Length = ec;

		
							{ // ��Ŷ ó��
								int remain_data = recv_buf_Length + remoteClient->tcpConnection.m_prev_remain;
								while (remain_data > 0) {
									unsigned char packet_size = recv_buf[0];
									// ���� �����Ͱ� ���� ó���� ��Ŷ ũ�⺸�� ������ �߸� ���̴�. (Ȥ�� �� �°� ������ ���̴�.)
									if (packet_size > remain_data)
										break;

									//��Ŷ ó��
									Process_Packet(remoteClient, recv_buf);

									//���� ��Ŷ �̵�, ���� ������ ����
									recv_buf += packet_size;
									remain_data -= packet_size;

								}
								//���� ������ ����
								remoteClient->tcpConnection.m_prev_remain = remain_data;

								//���� �����Ͱ� 0�� �ƴ� ���� ������ recv_buf�� �� ������ �����Ѵ�.
								if (remain_data > 0) {
									memcpy(remoteClient->tcpConnection.m_recvOverlapped._buf, recv_buf, remain_data);
								}
							}

							// ���� ���� �غ� �Ѵ�.
							if (remoteClient->tcpConnection.ReceiveOverlapped() != 0
								&& WSAGetLastError() != ERROR_IO_PENDING)
							{
								ProcessClientLeave(remoteClient);
							}
							else
							{
								// I/O�� �ɾ���. �ϷḦ ����ϴ� �� ���·� �ٲ���.
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
	// ����ڰ� ctl-c�� ������ ���η����� �����ϰ� ����ϴ�.
	signal(SIGINT, ProcessSignalAction);

	scene = make_shared<Scene>();

	Scene::scene->LoadSceneObb();

	std::cout << "Terrain Loding..." << std::endl;
	XMFLOAT3 xmf3Scale(1.0f, 0.38f, 1.0f);
	Scene::terrain = new HeightMapTerrain(_T("Terrain/terrain.raw"), 800, 800, xmf3Scale);
	Scene::terrain->SetPosition(-400, 0, -400);
	std::cout << "Terrain Loding Complete!" << std::endl;

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
	// IOCP�� �߰��Ѵ�.
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
					//	if (!((Player*)gameObject)->GetComponent<AttackComponent>()->During_Attack) ������Ʈ �߰� �ؾ���
					rc.second->m_pPlayer->PresentAniType = E_PLAYER_ANIMATION_TYPE::E_IDLE;
				}

				if (rc.second->m_pPlayer->OldAniType != rc.second->m_pPlayer->PresentAniType) {

					shared_ptr<EXP_OVER> p = make_shared<EXP_OVER>();
					p->m_ioType = IO_TYPE::IO_TIMER_PLAYER_ANIMATION;
					PostQueuedCompletionStatus(iocp.m_hIocp, 1, (ULONG_PTR)rc.second.get(), &p->_wsa_over);

					rc.second->m_pPlayer->OldAniType = rc.second->m_pPlayer->PresentAniType;
				}
			}

			rc.second->m_pPlayer->update();
			
			// Move Packet
			{ 
				auto vel = rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetVelocity();

				if (!Vector3::Length(vel))
					continue;

				shared_ptr<EXP_OVER> p = make_shared<EXP_OVER>();
				p->m_ioType = IO_TYPE::IO_TIMER_PLAYER_MOVE;
				PostQueuedCompletionStatus(iocp.m_hIocp, 1, (ULONG_PTR)rc.second.get(), &p->_wsa_over);
			}

			// Camera Look Packet
			if (rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->is_Rotate){
				shared_ptr<EXP_OVER> p = make_shared<EXP_OVER>();
				p->m_ioType = IO_TYPE::IO_TIMER_PLAYER_LOOK;
				PostQueuedCompletionStatus(iocp.m_hIocp, 1, (ULONG_PTR)rc.second.get(), &p->_wsa_over);
				rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->is_Rotate = false;
			}
		}

		//Monster test
	//	for (auto monster : Scene::scene->MonsterObjects) {
		++MonsterTimerDelay;
		if (MonsterTimerDelay <= MAX_MonsterTimerDelay) 
			continue;
		MonsterTimerDelay = 0;
		for (auto& monster : Scene::scene->MonsterObjects) {
			if (monster->GetRemainHP() <= 0.f)
				continue;

			/*{
				shared_ptr<EXP_OVER> p = make_shared<EXP_OVER>();
				p->m_ioType = IO_TYPE::IO_TIMER_MONSTER_MOVE;
				PostQueuedCompletionStatus(iocp.m_hIocp, 1, (ULONG_PTR)(monster), &p->_wsa_over);
			}
			{
				shared_ptr<EXP_OVER> p = make_shared<EXP_OVER>();
				p->m_ioType = IO_TYPE::IO_TIMER_MONSTER_WANDER;
				PostQueuedCompletionStatus(iocp.m_hIocp, 1, (ULONG_PTR)(monster), &p->_wsa_over);
			}
			{
				shared_ptr<EXP_OVER> p = make_shared<EXP_OVER>();
				p->m_ioType = IO_TYPE::IO_TIMER_MONSTER_ANIMATION;
				PostQueuedCompletionStatus(iocp.m_hIocp, 1, (ULONG_PTR)(monster), &p->_wsa_over);
			}
			{
				shared_ptr<EXP_OVER> p = make_shared<EXP_OVER>();
				p->m_ioType = IO_TYPE::IO_TIMER_MONSTER_TARGET;
				PostQueuedCompletionStatus(iocp.m_hIocp, 1, (ULONG_PTR)(monster), &p->_wsa_over);
			}*/
		}
	}

	for (auto& th : worker_threads) th->join();

	// ���� ����
	CloseServer();
}

void CloseServer()
{
	lock_guard<recursive_mutex> lock_accept(mx_accept);
	// i/o �Ϸ� üũ
	p_listenSocket->Close();


	for (auto i : RemoteClient::remoteClients)
	{
		i.second->tcpConnection.Close();
	}


	// ������ �����ϱ� ���� ������
	cout << "������ �����ϰ� �ֽ��ϴ�...\n";
	while (RemoteClient::remoteClients.size() > 0)
	{
		// I/O completion�� ���� ������ RemoteClient�� �����Ѵ�.
		for (auto i = RemoteClient::remoteClients.begin(); i != RemoteClient::remoteClients.end(); ++i)
		{
			if (!i->second->tcpConnection.m_isReadOverlapped) {
				RemoteClient::remoteClients.unsafe_erase(i);
			}
		}

		// I/O completion�� �߻��ϸ� �� �̻� Overlapped I/O�� ���� ���� '���� �����ص� ��...'�� �÷����Ѵ�.
		IocpEvents readEvents;
		iocp.Wait(readEvents, 100);

		// ���� �̺�Ʈ ������ ó���մϴ�.
		for (int i = 0; i < readEvents.m_eventCount; i++)
		{
			auto& readEvent = readEvents.m_events[i];
			if (readEvent.lpCompletionKey == 0) // ���������̸�
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
	
	cout << "���� ��.\n";
}
void ProcessAccept()
{
	lock_guard<recursive_mutex> lock_accept(mx_accept);
	p_listenSocket->m_isReadOverlapped = false;
	// �̹� accept�� �Ϸ�Ǿ���. ��������, Win32 AcceptEx ������ ������ ������ �۾��� ����. 
	if (remoteClientCandidate->tcpConnection.UpdateAcceptContext(*p_listenSocket) != 0)
	{
		//���������� �ݾҴ��� �ϸ� ���⼭ �������Ŵ�. �׷��� �������� �Ҵɻ��·� ������.
		p_listenSocket->Close();
	}
	else // �� ó����
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
		// �� TCP ���ϵ� IOCP�� �߰��Ѵ�.
		iocp.Add(remoteClient->tcpConnection, remoteClient.get());

		// overlapped ������ ���� �� �־�� �ϹǷ� ���⼭ I/O ���� ��û�� �ɾ�д�.
		if (remoteClient->tcpConnection.ReceiveOverlapped() != 0
			&& WSAGetLastError() != ERROR_IO_PENDING)
		{
			// ����. ������ ��������. �׸��� �׳� ������.
			remoteClient->tcpConnection.Close();
		}
		else
		{
			// I/O�� �ɾ���. �ϷḦ ����ϴ� �� ���·� �ٲ���.
			remoteClient->tcpConnection.m_isReadOverlapped = true;

			// �� Ŭ���̾�Ʈ�� ��Ͽ� �߰�.

			RemoteClient::remoteClients.insert({ remoteClient.get(), remoteClient });
			cout << "Client joined. There are " << RemoteClient::remoteClients.size() << " connections.\n";
			
		}

		// ����ؼ� ���� �ޱ⸦ �ؾ� �ϹǷ� �������ϵ� overlapped I/O�� ����.
		remoteClientCandidate = make_shared<RemoteClient>(SocketType::Tcp);
		string errorText;
		if (!p_listenSocket->AcceptOverlapped(remoteClientCandidate->tcpConnection, errorText)
			&& WSAGetLastError() != ERROR_IO_PENDING)
		{
			// �������� �������� �Ҵ� ���·� ������. 
			p_listenSocket->Close();
		}
		else
		{
			// ���������� ������ ������ ��ٸ��� ���°� �Ǿ���.
			p_listenSocket->m_isReadOverlapped = true;
		}
	}
}


void Process_Packet(shared_ptr<RemoteClient>& p_Client, char* p_Packet)
{
	switch (p_Packet[1]) // ��Ŷ Ÿ��
	{
	case E_PACKET::E_PACKET_CS_LOGIN: {
		CS_LOGIN_PACKET* recv_packet = reinterpret_cast<CS_LOGIN_PACKET*>(p_Packet);
		memcpy(p_Client->name,recv_packet->name,sizeof(recv_packet->name));

		//id �ο�
		p_Client->m_id = N_CLIENT_ID++;

		{ // ������ Ŭ���̾�Ʈ ���� ���� �۽�
			SC_LOGIN_INFO_PACKET send_packet;
			send_packet.size = sizeof(SC_LOGIN_INFO_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_LOGIN_INFO;
			send_packet.id = p_Client->m_id;
			p_Client->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}

		// ������ Ŭ���̾�Ʈ���� ��� �÷��̾� ���� �۽�
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

		// �ٸ� Ŭ���̾�Ʈ�鿡�� ������ Ŭ���̾�Ʈ ���� �۽�
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

		// �ٸ� Ŭ���̾�Ʈ�鿡�� ���� HP ���� ���
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

		// �ٸ� Ŭ���̾�Ʈ�鿡�� ���� HP ���� ���
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
			memcpy(send_packet.chat, recv_packet->chat, sizeof(recv_packet->chat));
			rc.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
	}
		break;
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
			if (!rc_to.second->b_Enable)
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
			if (!rc_to.second->b_Enable)
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
			if (!rc_to.second->b_Enable)
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
		if (((Character*)p_Client)->OldAniType != ((Character*)p_Client)->PresentAniType) {
			for (auto& rc_to : RemoteClient::remoteClients) {
				if (!rc_to.second->b_Enable)
					continue;
				SC_MONSTER_ANIMATION_TYPE_PACKET send_packet;
				send_packet.size = sizeof(SC_MONSTER_ANIMATION_TYPE_PACKET);
				send_packet.type = E_PACKET::E_PACKET_SC_ANIMATION_TYPE_MOSTER;
				send_packet.id = ((Character*)p_Client)->num;
				send_packet.Anitype = ((Character*)p_Client)->PresentAniType;
				rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
			}
			((Character*)p_Client)->OldAniType = ((Character*)p_Client)->PresentAniType;
		}
	}
		break;
	case IO_TYPE::IO_TIMER_MONSTER_MOVE: {
		//Monster Pos
		for (auto& rc_to : RemoteClient::remoteClients) {
			if (!rc_to.second->b_Enable)
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
			if (!rc_to.second->b_Enable)
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
				if (!rc_to.second->b_Enable)
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
		if (p_NPC->OldAniType != p_NPC->PresentAniType) {
			for (auto& rc_to : RemoteClient::remoteClients) {
				if (!rc_to.second->b_Enable)
					continue;
				SC_MONSTER_ANIMATION_TYPE_PACKET send_packet;
				send_packet.size = sizeof(SC_MONSTER_ANIMATION_TYPE_PACKET);
				send_packet.type = E_PACKET::E_PACKET_SC_ANIMATION_TYPE_MOSTER;
				send_packet.id = p_NPC->num;
				send_packet.Anitype = p_NPC->PresentAniType;
				rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
			}
			p_NPC->OldAniType = p_NPC->PresentAniType;
		}
	}
											break;
	case IO_TYPE::IO_TIMER_MONSTER_MOVE: {
		//Monster Pos
		for (auto& rc_to : RemoteClient::remoteClients) {
			if (!rc_to.second->b_Enable)
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
			if (!rc_to.second->b_Enable)
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
				if (!rc_to.second->b_Enable)
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