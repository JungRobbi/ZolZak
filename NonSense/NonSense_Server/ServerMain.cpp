#pragma once
#include "../ImaysNet/ImaysNet.h"
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


using namespace std;

volatile bool stopWorking = false;

const int N_THREAD{ 3 };

// shared_ptr<Scene> scene;
vector<shared_ptr<thread>> worker_threads;

enum class IO_TYPE;

void ProcessSignalAction(int sig_number)
{
	if (sig_number == SIGINT)
		stopWorking = true;
}

class RemoteClient {
public:
	shared_ptr<thread> thread; // Ŭ���̾�Ʈ ������
	Socket tcpConnection;		// accept�� TCP ����

//	shared_ptr<Player> m_pPlayer;

	RemoteClient() : thread(), tcpConnection(SocketType::Tcp) {}
	RemoteClient(SocketType socketType) :tcpConnection(socketType) {}
};

unordered_map<RemoteClient*, shared_ptr<RemoteClient>> remoteClients;
recursive_mutex mx_rc;

list<shared_ptr<RemoteClient>> deleteClinets;

void ProcessClientLeave(shared_ptr<RemoteClient> remoteClient)
{
	// ���� Ȥ�� ���� �����̴�.
	// �ش� ������ �����ع�����. 
	remoteClient->tcpConnection.Close();
	{
		lock_guard<recursive_mutex> lock_rc(mx_rc);
		remoteClients.erase(remoteClient.get());

		cout << "Client left. There are " << remoteClients.size() << " connections.\n";
	}
}

// IOCP�� �غ��Ѵ�.
Iocp iocp(N_THREAD); // 5���� ������ ����� API�� ��Ʈ�� �ش�.

recursive_mutex mx_accept;

shared_ptr<Socket> p_listenSocket;
shared_ptr<RemoteClient> remoteClientCandidate;

void CloseServer();
void ProcessAccept();

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
				auto p_readOverlapped = (OVERLAPPEDEX*)readEvent.lpOverlapped;

				if (IO_TYPE::IO_SEND == p_readOverlapped->m_ioType) {
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
						remoteClient = remoteClients[(RemoteClient*)readEvent.lpCompletionKey];
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
							char* echoData = remoteClient->tcpConnection.m_recvOverlapped.m_dataBuffer;
							int echoDataLength = ec;

							double id;
							unsigned int RecvMSG = 0;
							unsigned int SendMSG = 0;

							//{ // ������ ó��
							//	memcpy(&id, echoData, sizeof(double));
							//	memcpy(&RecvMSG, &echoData[8], sizeof(int));

							//}

							//{ // ������ ������
							///* ������ ����
							//	int msg - � �������� (������ ���� ��ġ)
							//	int x,
							//	int y
							//*/
							//	char buf[BUFSIZE];
							//	memcpy(buf, &id, sizeof(double));
							//	memcpy(&buf[8], &SendMSG, sizeof(int));

							//	{
							//		lock_guard<recursive_mutex> lock_rc(mx_rc);
							//		for (auto rc : remoteClients)
							//		{
							//			if (rc.second->tcpConnection.SendOverlapped(buf, BUFSIZE) != 0
							//				&& WSAGetLastError() != ERROR_IO_PENDING)
							//			{
							//				ProcessClientLeave(remoteClient);
							//			}
							//		}
							//	}
							//}

							//	std::cout << std::endl << SendMSG << " ���� " << std::endl << std::endl;

								// �ٽ� ������ �������� overlapped I/O�� �ɾ�� �Ѵ�.
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

		for (auto i : remoteClients)
		{
			i.second->tcpConnection.Close();
		}


		// ������ �����ϱ� ���� ������
		cout << "������ �����ϰ� �ֽ��ϴ�...\n";
		while (remoteClients.size() > 0)
		{
			// I/O completion�� ���� ������ RemoteClient�� �����Ѵ�.
			for (auto i = remoteClients.begin(); i != remoteClients.end(); ++i)
			{
				if (!i->second->tcpConnection.m_isReadOverlapped)
					remoteClients.erase(i);
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
					shared_ptr<RemoteClient> remoteClient = remoteClients[(RemoteClient*)readEvent.lpCompletionKey];
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
				remoteClients.insert({ remoteClient.get(), remoteClient });

				cout << "Client joined. There are " << remoteClients.size() << " connections.\n";
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