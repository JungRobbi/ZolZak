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

	// Connect_Thread 생성
	threads.emplace_back(make_shared<thread>(&ClientThread)); // tcp 연결, 수신 스레드 생성
	
	// threads.emplace_back(make_shared<thread>(&ClientThread)); // 송신 스레드 생성 - 필요하면 함수 작성, 생성
}

void NetworkMGR::Tick()
{
	// MSGSendQueue를 확인하고 있으면 서버로 전송
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
	// 연결
	//
	{
		lock_guard<recursive_mutex> lock_out(mx_out);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 10 });
		std::cout << " ======== Login ======== " << std::endl;

		std::cout << "아이디 입력 : ";
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
	// 수신 루프
	//

	while (true) {
		if (tcpSocket->Receive() == SOCKET_ERROR) {
			// 소켓 연결에 문제가 생김.
			break;
		}

		// MSGRecvQueue에 데이터 저장
		MSGQueue::RecvMSGQueue.push_back(DataMSG{ tcpSocket->m_recvOverlapped.m_dataBuffer });
	}

	// 소켓 닫기
	tcpSocket->Close();
}