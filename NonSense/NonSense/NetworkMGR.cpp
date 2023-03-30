#include "../ImaysNet/ImaysNet.h"
#include "NetworkMGR.h"
#include "GameScene.h"

char* NetworkMGR::SERVERIP;

list<shared_ptr<thread>> NetworkMGR::threads;
recursive_mutex NetworkMGR::mx_out;

shared_ptr<Socket> NetworkMGR::tcpSocket;

double NetworkMGR::ID{};

void NetworkMGR::start()
{
	tcpSocket = make_shared<Socket>(SocketType::Tcp);

	// Connect_Thread ����
	threads.emplace_back(make_shared<thread>(&ClientThread)); // tcp ����, ���� ������ ����
	
	// threads.emplace_back(make_shared<thread>(&ClientThread)); // �۽� ������ ���� - �ʿ��ϸ� �Լ� �ۼ�, ����
}

void NetworkMGR::Tick()
{
	// MSGSendQueue�� Ȯ���ϰ� ������ ������ ����
	if (MSGQueue::SendMSGQueue.empty() || tcpSocket->m_fd == INVALID_SOCKET)
		return;

	while (!MSGQueue::SendMSGQueue.empty()) {
		DataMSG msg = MSGQueue::PopFrontSendMSG();
		char buf[BUFSIZE];

		memcpy(buf, &msg, sizeof(DataMSG));

		tcpSocket->Send(buf, BUFSIZE);
	}
}

void NetworkMGR::ClientThread()
{
	//
	// ����
	//
	{
		lock_guard<recursive_mutex> lock_out(mx_out);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 10 });
		std::cout << " ======== Login ======== " << std::endl;

		std::cout << "���̵� �Է� : ";
	}
	std::cin >> ID;

	std::string server_s{ "127.0.0.1" };
	SERVERIP = new char[server_s.size() + 1];
	SERVERIP[server_s.size()] = '\0';
	strcpy(SERVERIP, server_s.c_str());

	tcpSocket->Bind(Endpoint::Any);
	tcpSocket->Connect(Endpoint(SERVERIP, SERVERPORT));

	system("cls");

	DataMSG dm;
	dm.id = ID;
	dm.msg = E_MSG_CONNECT;
	ZeroMemory(dm.data, sizeof(dm.data));
	MSGQueue::AddSendMSG(dm);

	//
	// ���� ����
	//

	while (true) {
		if (tcpSocket->Receive() == SOCKET_ERROR) {
			// ���� ���ῡ ������ ����.
			break;
		}

		// MSGRecvQueue�� ������ ����
		MSGQueue::RecvMSGQueue.push_back(DataMSG{ tcpSocket->m_recvOverlapped.m_dataBuffer });
	}

	// ���� �ݱ�
	tcpSocket->Close();
}