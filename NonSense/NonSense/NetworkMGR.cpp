#include <iostream>

#include "NetworkMGR.h"

Scene* NetworkMGR::scene;

char* NetworkMGR::SERVERIP = (char*)"127.0.0.1";
SOCKET NetworkMGR::sock;

HANDLE NetworkMGR::ConnectEvent;

void NetworkMGR::start()
{
	HANDLE hThread;
	ConnectEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	hThread = CreateThread(NULL, 0, ConnectThread, NULL, 0, NULL);
	CloseHandle(hThread);

	int retval = WaitForSingleObject(ConnectEvent, INFINITE);
	if (retval != WAIT_OBJECT_0) {
		// error
	}
}

void NetworkMGR::Tick()
{
	// MSGSendQueue�� Ȯ���ϰ� ������ ������ ����
	if (MSGQueue::SendMSGQueue.empty())
		return;

	while (!MSGQueue::SendMSGQueue.empty()) {
		DataMSG msg = MSGQueue::PopFrontSendMSG();
		char buf[BUFSIZE];

		memcpy(buf, &msg, sizeof(DataMSG));

		int retval = send(sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
		}
	}
}


DWORD WINAPI NetworkMGR::RecvThread(LPVOID lParam)
{
	char buf[BUFSIZE];

	while (true) {
		// ������ �ޱ� (���� ��ġ)
		/* ������ ����
			int msg - � �������� (������ ���� ��ġ)
			int x,
			int y
		*/

		::ZeroMemory(buf, sizeof(buf));
		int retval = recv(sock, buf, BUFSIZE, MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// MSGRecvQueue�� ������ ����
		MSGQueue::RecvMSGQueue.push_back(DataMSG{ buf });
	}

	// ���� �ݱ�
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}

DWORD WINAPI NetworkMGR::ConnectThread(LPVOID lParam)
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	std::cout << std::endl << " ======== Login ======== " << std::endl << std::endl;

	std::cout << std::endl << "���� �� �����ּҸ� �Է����ּ���(ex 197.xxx.xxx.xxx) : ";
	std::string server_s;
	std::cin >> server_s;

	SERVERIP = new char[server_s.size() + 1];
	SERVERIP[server_s.size()] = '\0';
	strcpy(SERVERIP, server_s.c_str());

	// ���� ����
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	HANDLE hThread;

	hThread = CreateThread(NULL, 0, RecvThread, (LPVOID)sock, 0, NULL);
	CloseHandle(hThread);

	std::cout << std::endl << " ���� �Ϸ� " << std::endl;
	system("cls");

	SetEvent(ConnectEvent);
	return 0;
}