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

#include "remoteClients/RemoteClient.h"
#include "Scene.h"

#include "Components/PlayerMovementComponent.h"

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

list<shared_ptr<RemoteClient>> deleteClinets;

void ProcessClientLeave(shared_ptr<RemoteClient> remoteClient)
{
	// ���� Ȥ�� ���� �����̴�.
	// �ش� ������ �����ع�����. 
	remoteClient->tcpConnection.Close();
	{
		lock_guard<recursive_mutex> lock_rc(mx_rc);
		RemoteClient::remoteClients.erase(remoteClient.get());

		cout << "Client left. There are " << RemoteClient::remoteClients.size() << " connections.\n";
	}
}

// IOCP�� �غ��Ѵ�.
Iocp iocp(N_THREAD); // 5���� ������ ����� API�� ��Ʈ�� �ش�.

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

				if (readEvent.lpCompletionKey == 0) // ���������̸�
				{
					ProcessAccept(); // Ŭ���̾�Ʈ ���� �۾�
				}
				else  // TCP ���� �����̸�
				{
					// ó���� Ŭ���̾�Ʈ �޾ƿ���
					shared_ptr<RemoteClient> remoteClient;
					{
						lock_guard<recursive_mutex> lock_rc(mx_rc);
						remoteClient = RemoteClient::remoteClients[(RemoteClient*)readEvent.lpCompletionKey];
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

							cout << " recv! - recv_buf_Length : " << recv_buf_Length << endl;

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
	Timer::Initialize();
	Timer::Reset();

	p_listenSocket = make_shared<Socket>(SocketType::Tcp);
	p_listenSocket->Bind(Endpoint("0.0.0.0", SERVERPORT));
	p_listenSocket->Listen();
	// IOCP�� �߰��Ѵ�.
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

	//while (true) {
	//	Timer::Tick(0.0f);
	//	Scene::scene->update();

	//	/*{
	//		lock_guard<recursive_mutex> lock_rc(mx_rc);

	//		for (auto& rc : RemoteClient::remoteClients) {
	//			if (!rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>())
	//				continue;
	//			auto rc_pos = rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetPosition();
	//			for (auto& rc_to : RemoteClient::remoteClients) {
	//				SC_MOVE_PLAYER_PACKET send_packet;
	//				send_packet.size = sizeof(SC_MOVE_PLAYER_PACKET);
	//				send_packet.type = E_PACKET::E_PACKET_SC_MOVE_PLAYER;
	//				send_packet.id = rc.second->m_id;
	//				send_packet.x = rc_pos.x;
	//				send_packet.y = rc_pos.y;
	//				send_packet.z = rc_pos.z;
	//				rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
	//			}
	//		}
	//	}*/
	//}

	for (auto& th : worker_threads) th->join();

	// ���� ����
	CloseServer();
}

void CloseServer()
{
	lock_guard<recursive_mutex> lock_accept(mx_accept);
	// i/o �Ϸ� üũ
	p_listenSocket->Close();
	{
		lock_guard<recursive_mutex> lock_rc(mx_rc);

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
				if (!i->second->tcpConnection.m_isReadOverlapped)
					RemoteClient::remoteClients.erase(i);
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
		remoteClient->m_id = N_CLIENT_ID++;
		remoteClient->m_pPlayer = make_shared<Player>();
		remoteClient->m_pPlayer->start();
		remoteClient->m_pPlayer->remoteClient = remoteClient.get();
	//	RemoteClient::remoteClients_ptr_v.emplace_back(remoteClient.get());

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
			{
				lock_guard<recursive_mutex> lock_rc(mx_rc);
				RemoteClient::remoteClients.insert({ remoteClient.get(), remoteClient });

				cout << "Client joined. There are " << RemoteClient::remoteClients.size() << " connections.\n";
			}
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
	lock_guard<recursive_mutex> lock_rc(mx_rc);

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
		CS_MOVE_PACKET* recv_packet = reinterpret_cast<CS_MOVE_PACKET*>(p_Packet);
		XMFLOAT3 xmf3Dir{ XMFLOAT3(recv_packet->dirX, recv_packet->dirY, recv_packet->dirZ) };
		auto pm = p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>();
		pm->Move(xmf3Dir, false);

		for (auto& rc_to : RemoteClient::remoteClients) {
			SC_MOVE_PLAYER_PACKET send_packet;
			send_packet.size = sizeof(SC_MOVE_PLAYER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_MOVE_PLAYER;
			send_packet.id = p_Client->m_id;
			send_packet.x = pm->GetPosition().x;
			send_packet.y = pm->GetPosition().y;
			send_packet.z = pm->GetPosition().z;
			rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
		break;
	}
	case E_PACKET::E_PACKET_CS_LOOK: {
		CS_LOOK_PACKET* recv_packet = reinterpret_cast<CS_LOOK_PACKET*>(p_Packet);
		auto pm = p_Client->m_pPlayer->GetComponent<PlayerMovementComponent>();
		XMFLOAT3 xmf3Look{ XMFLOAT3(recv_packet->x, recv_packet->y, recv_packet->z) };
		XMFLOAT3 xmf3RightVector = pm->GetRightVector();
		XMFLOAT3 xmf3UpVector = pm->GetUpVector();

		pm->SetLookVector(xmf3Look);
		pm->SetRightVector(Vector3::CrossProduct(xmf3UpVector, xmf3Look, true));
		xmf3RightVector = pm->GetRightVector();
		pm->SetUpVector(Vector3::CrossProduct(xmf3Look, xmf3RightVector, true));
		xmf3UpVector = pm->GetUpVector();

		cout << "xmf3Look.x - " << xmf3Look.x << endl;
		cout << "xmf3Look.y - " << xmf3Look.y << endl;
		cout << "xmf3Look.z - " << xmf3Look.z << endl;

		for (auto& rc_to : RemoteClient::remoteClients) {
			SC_LOOK_PLAYER_PACKET send_packet;
			send_packet.size = sizeof(SC_LOOK_PLAYER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_SC_LOOK_PLAYER;
			send_packet.id = p_Client->m_id;
			send_packet.x = xmf3Look.x;
			send_packet.y = xmf3Look.y;
			send_packet.z = xmf3Look.z;
			rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
		}
		break;
	}
	default:
		break;
	}
}