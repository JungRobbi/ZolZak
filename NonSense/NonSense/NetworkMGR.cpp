#include "../ImaysNet/ImaysNet.h"
#include "NetworkMGR.h"
#include "GameScene.h"
#pragma comment(lib, "WS2_32.LIB")

char* NetworkMGR::SERVERIP = "127.0.0.1";

recursive_mutex NetworkMGR::mutex;

shared_ptr<Socket> NetworkMGR::tcpSocket;

void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag)
{
	char* recv_buf = reinterpret_cast<EXP_OVER*>(recv_over)->_wsa_buf.buf;
	int recv_buf_Length = MAX_SOCKBUF; // ���� ���� ����� Ȯ���� ����� ã�ƾ� ��

	{ // ��Ŷ ó��
		int remain_data = recv_buf_Length + NetworkMGR::tcpSocket->m_prev_remain;
		while (remain_data > 0) {
			unsigned char packet_size = recv_buf[0];
			// ���� �����Ͱ� ���� ó���� ��Ŷ ũ�⺸�� ������ �߸� ���̴�. (Ȥ�� �� �°� ������ ���̴�.)
			// Ȥ�� packet_size�� 0�� ��� ������ �� �κ��� ã�� ���̰ų� �����̴�.
			if (packet_size > remain_data)
				break;
			else if (packet_size == 0) {
				remain_data = 0;
				break;
			}

			//��Ŷ ó��
			NetworkMGR::Process_Packet(recv_buf);

			//���� ��Ŷ �̵�, ���� ������ ����
			recv_buf += packet_size;
			remain_data -= packet_size;
		}
		//���� ������ ����
		NetworkMGR::tcpSocket->m_prev_remain = remain_data;

		//���� �����Ͱ� 0���� ũ�� recv_buf�� �� ������ �����Ѵ�.
		if (remain_data > 0) {
			memcpy(NetworkMGR::tcpSocket->m_recvOverlapped._buf, recv_buf, remain_data);
		}
	}

	memset(&NetworkMGR::tcpSocket->m_recvOverlapped._buf, 0, sizeof(NetworkMGR::tcpSocket->m_recvOverlapped._buf));
	memset(&NetworkMGR::tcpSocket->m_recvOverlapped._wsa_over, 0, sizeof(NetworkMGR::tcpSocket->m_recvOverlapped._wsa_over));
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
	// ����
	//

//	std::cout << std::endl << " ======== Login ======== " << std::endl << std::endl;

//	std::cout << std::endl << "���� �� �����ּҸ� �Է����ּ���(ex 197.xxx.xxx.xxx) : " << std::endl;
//	std::string server_s;
//	std::cin >> server_s;
//	SERVERIP = new char[server_s.size() + 1];
//	SERVERIP[server_s.size()] = '\0';
//	strcpy(SERVERIP, server_s.c_str());

	tcpSocket->Bind(Endpoint::Any);
	tcpSocket->Connect(Endpoint(SERVERIP, SERVERPORT));

	system("cls");

	do_recv();
}

void NetworkMGR::Tick()
{
	SleepEx(0, true);

	// MSGSendQueue�� Ȯ���ϰ� ������ ������ ����
	if (PacketQueue::SendQueue.empty() || tcpSocket->m_fd == INVALID_SOCKET)
		return;

	while (!PacketQueue::SendQueue.empty()) {
		// ������ �۽�
		char* send_buf = PacketQueue::SendQueue.front();

		int buf_size{};
		while (1) {
			if (buf_size + send_buf[buf_size] > MAX_BUFSIZE_CLIENT || send_buf[buf_size] == 0)
				break;
			buf_size += send_buf[buf_size];
		}

		// EXP_OVER ���·� ���� Ȥ�� buf ���·� ���� �� send �ؾ���

		do_send(send_buf, buf_size);
		PacketQueue::PopSendPacket();
	}
}

void NetworkMGR::do_recv() {
	DWORD recv_flag = 0;
	WSARecv(tcpSocket->m_fd, &(tcpSocket->m_recvOverlapped._wsa_buf), 1, 0, &recv_flag, &(tcpSocket->m_recvOverlapped._wsa_over), recv_callback);
}

void NetworkMGR::do_send(const char* buf, short buf_size) {
	EXP_OVER* send_over = new EXP_OVER(buf, buf_size);
	WSASend(tcpSocket->m_fd, &send_over->_wsa_buf, 1, 0, 0, &send_over->_wsa_over, send_callback);
}

void NetworkMGR::Process_Packet(char* p_Packet)
{
	switch (p_Packet[1]) // ��Ŷ Ÿ��
	{
	case E_PACKET::E_PACKET_SC_POSITIONING_PLAYER: {
		SC_MOVE_PLAYER_PACKET* recv_packet = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(p_Packet);

		break;
	}
	default:
		break;
	}
}