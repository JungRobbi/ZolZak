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

// unordered map Ž�� ����ϱ� ���� index �߰�
// Player ��, Monster �� �þ� ó��
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
	//Scene�� ObjectList���� ���� Player ����
//	Scene::scene->PushDelete(remoteClient->m_pPlayer.get());

	// ���� Ȥ�� ���� �����̴�.
	// �ش� ������ �����ع�����. 
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
	//�÷��̾ �����ٰ� �˸�
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

// IOCP�� �غ��Ѵ�.
Iocp iocp(N_THREAD); // N_THREAD���� ������ ����� API�� ��Ʈ�� �ش�.

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
							ProcessClientLeave(remoteClient, db_mgr);
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
									Process_Packet(remoteClient, recv_buf, db_mgr);

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
								ProcessClientLeave(remoteClient, db_mgr);
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
		for (auto& room : Room::roomlist) {
			room.second->update();
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


void Process_Packet(shared_ptr<RemoteClient>& p_Client, char* p_Packet, shared_ptr<DBMGR> p_DBMGR)
{
	switch (p_Packet[1]) // ��Ŷ Ÿ��
	{
	case E_PACKET::E_PACKET_CS_LOGIN: {
		CS_LOGIN_PACKET* recv_packet = reinterpret_cast<CS_LOGIN_PACKET*>(p_Packet);
		if (DBMGR::db_connection) {
			//DB�� �����Ͱ� �ִ��� Ȯ���ϰ� ���̵� ���� Ȥ�� �ҷ�����
			wchar_t* wname = ChartoWChar(recv_packet->name);
			if (!p_DBMGR->Get_SELECT_PLAYER(wname)) { // LOGIN_FAIL - ���� ID
				// �� ID ����
				p_DBMGR->Set_INSERT_ID(wname);
				cout << "Login FAIL! (�� ���̵� ����) " << endl;
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
				if (login) { // LOGIN_FAIL - �̹� ������ ID
					cout << "Login FAIL! (�̹� ����) " << endl;
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
		//id �ο�
		p_Client->m_id = N_CLIENT_ID++;

		for (auto& room : Room::roomlist) { // roomList ����
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

		// �ٸ� Ŭ���̾�Ʈ�鿡�� ���� HP ���� ���
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

		// �ٸ� Ŭ���̾�Ʈ�鿡�� ���� HP ���� ���
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
		// �ӽ� �� ����
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

		//{ // ������ Ŭ���̾�Ʈ ���� ���� �۽�
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

		//// ������ Ŭ���̾�Ʈ���� ��� �÷��̾� ���� �۽�
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

		//// �ٸ� Ŭ���̾�Ʈ�鿡�� ������ Ŭ���̾�Ʈ ���� �۽�
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