#include "../ImaysNet/ImaysNet.h"
#include "NetworkMGR.h"
#include "GameScene.h"
#pragma comment(lib, "WS2_32.LIB")

char* NetworkMGR::SERVERIP;

recursive_mutex NetworkMGR::mutex;

shared_ptr<Socket> NetworkMGR::tcpSocket;

void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag)
{
	char* recv_buf = reinterpret_cast<EXP_OVER*>(recv_over)->_wsa_buf.buf;
	
	DataMSG rdm{ recv_buf + 5 };
	
	if (rdm.msg == E_MSG_POSITIONING) { // 위치 이동
		int x;
		int y;
		memcpy(&x, rdm.data, sizeof(int));
		memcpy(&y, &rdm.data[4], sizeof(int));
		Vectorint2 pos{ x, y };
		dynamic_cast<GameScene*>(Scene::scene)->player->setPosition(pos);
	}
	memset(&NetworkMGR::tcpSocket->m_readOverlappedStruct._send_msg, 0, sizeof(NetworkMGR::tcpSocket->m_readOverlappedStruct._send_msg));
	memset(&NetworkMGR::tcpSocket->m_readOverlappedStruct._wsa_over, 0, sizeof(NetworkMGR::tcpSocket->m_readOverlappedStruct._wsa_over));
	NetworkMGR::do_recv();
}

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD recv_flag)
{
	delete reinterpret_cast<EXP_OVER*>(send_over);
}

void NetworkMGR::start()
{
	tcpSocket = make_shared<Socket>(SocketType::Tcp);
	
	//
	// 연결
	//

	std::cout << std::endl << " ======== Login ======== " << std::endl << std::endl;

	std::cout << std::endl << "접속 할 서버주소를 입력해주세요(ex 197.xxx.xxx.xxx) : " << std::endl;
	std::string server_s;
	std::cin >> server_s;
	SERVERIP = new char[server_s.size() + 1];
	SERVERIP[server_s.size()] = '\0';
	strcpy(SERVERIP, server_s.c_str());

	tcpSocket->Bind(Endpoint::Any);
	tcpSocket->Connect(Endpoint(SERVERIP, SERVERPORT));

	system("cls");

	do_recv();
}

void NetworkMGR::Tick()
{
	SleepEx(0, true);

	// MSGSendQueue를 확인하고 있으면 서버로 전송
	if (MSGQueue::SendMSGQueue.empty() || tcpSocket->m_fd == INVALID_SOCKET)
		return;

	while (!MSGQueue::SendMSGQueue.empty()) {
		// 데이터 송신
		DataMSG msg = MSGQueue::PopFrontSendMSG();
		char buf[BUFSIZE];

		memcpy(buf, &msg, sizeof(DataMSG));

		do_send(0, BUFSIZE, buf);
	}
}

void NetworkMGR::do_recv() {
	DWORD recv_flag = 0;
	WSARecv(tcpSocket->m_fd, &(tcpSocket->m_readOverlappedStruct._wsa_buf), 1, 0, &recv_flag, &(tcpSocket->m_readOverlappedStruct._wsa_over), recv_callback);
}

void NetworkMGR::do_send(unsigned long long sender_id, int num_bytes, const char* buff) {
	EXP_OVER* send_over = new EXP_OVER(sender_id, num_bytes, buff);
	WSASend(tcpSocket->m_fd, &send_over->_wsa_buf, 1, 0, 0, &send_over->_wsa_over, send_callback);
}