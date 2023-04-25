#include "../ImaysNet/ImaysNet.h"

#include <algorithm>

#include "NetworkMGR.h"
#include "GameScene.h"
#include "GameFramework.h"
#pragma comment(lib, "WS2_32.LIB")

char* NetworkMGR::SERVERIP = "127.0.0.1";

recursive_mutex NetworkMGR::mutex;

shared_ptr<Socket> NetworkMGR::tcpSocket;

unsigned int	NetworkMGR::id{};
string			NetworkMGR::name{};
bool			NetworkMGR::b_isNet{true};

void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag)
{
	char* recv_buf = reinterpret_cast<EXP_OVER*>(recv_over)->_buf;
	int recv_buf_Length = num_bytes;

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

	memset(NetworkMGR::tcpSocket->m_recvOverlapped._buf + NetworkMGR::tcpSocket->m_prev_remain, 0,
		sizeof(NetworkMGR::tcpSocket->m_recvOverlapped._buf) - NetworkMGR::tcpSocket->m_prev_remain);
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

	char isnet = 'n';
	std::cout << "NetworkMGR::start()�� cin�� �ּ�ó�� �ϸ� ����)" << endl;
	std::cout << "��Ʈ��ũ ���� ���� Ű �Է� (y/n - ����O/����X) :";

	std::cin >> isnet;

	if (isnet == 'n') {
		b_isNet = false;
		return;
	}
	std::cout << "�̸� �Է� : ";
	std::cin >> name;

	tcpSocket->Bind(Endpoint::Any);
	tcpSocket->Connect(Endpoint(SERVERIP, SERVERPORT));

	CS_LOGIN_PACKET send_packet;
	send_packet.size = sizeof(CS_LOGIN_PACKET);
	send_packet.type = E_PACKET::E_PACKET_CS_LOGIN;
	memcpy(send_packet.name, name.c_str(), name.size());
	PacketQueue::AddSendPacket(&send_packet);

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
	tcpSocket->m_readFlags = 0;
	ZeroMemory(&tcpSocket->m_recvOverlapped._wsa_over, sizeof(tcpSocket->m_recvOverlapped._wsa_over));
	tcpSocket->m_recvOverlapped._wsa_buf.len = MAX_SOCKBUF - tcpSocket->m_prev_remain;
	tcpSocket->m_recvOverlapped._wsa_buf.buf = tcpSocket->m_recvOverlapped._buf + tcpSocket->m_prev_remain;

	WSARecv(tcpSocket->m_fd, &(tcpSocket->m_recvOverlapped._wsa_buf), 1, 0, &tcpSocket->m_readFlags, &(tcpSocket->m_recvOverlapped._wsa_over), recv_callback);
}

void NetworkMGR::do_send(const char* buf, short buf_size) {
	EXP_OVER* send_over = new EXP_OVER(buf, buf_size);
	WSASend(tcpSocket->m_fd, &send_over->_wsa_buf, 1, 0, 0, &send_over->_wsa_over, send_callback);
}

void NetworkMGR::Process_Packet(char* p_Packet)
{
	switch (p_Packet[1]) // ��Ŷ Ÿ��
	{
	case E_PACKET::E_PACKET_SC_LOGIN_INFO: {
		SC_LOGIN_INFO_PACKET* recv_packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(p_Packet);

		GameFramework::MainGameFramework->m_pPlayer->id = recv_packet->id;

		break;
	}
	case E_PACKET::E_PACKET_SC_ADD_PLAYER: {
		SC_ADD_PLAYER_PACKET* recv_packet = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(p_Packet);

		auto player = GameFramework::MainGameFramework->m_OtherPlayersPool.back();
		GameFramework::MainGameFramework->m_OtherPlayersPool.pop_back();
		dynamic_cast<Player*>(player)->id = recv_packet->id;
		dynamic_cast<Player*>(player)->m_name = recv_packet->name;

		GameFramework::MainGameFramework->m_OtherPlayers.push_back(player);
		break;
	}
	case E_PACKET::E_PACKET_SC_REMOVE_PLAYER: {
		SC_REMOVE_PLAYER_PACKET* recv_packet = reinterpret_cast<SC_REMOVE_PLAYER_PACKET*>(p_Packet);
		auto leave_id = recv_packet->id;
		auto OtherPlayers = GameFramework::MainGameFramework->m_OtherPlayers;
		auto leave_player = find_if(OtherPlayers.begin(), OtherPlayers.end(), [&leave_id](Object* lhs) {
			return dynamic_cast<Player*>(lhs)->id == leave_id;
			});
		if (leave_player == OtherPlayers.end())
			break;
		OtherPlayers.erase(leave_player);
		GameScene::MainScene->PushDelete(*leave_player);
		break;
	}
	case E_PACKET::E_PACKET_SC_MOVE_PLAYER: {
		SC_MOVE_PLAYER_PACKET* recv_packet = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(p_Packet);
		Player* player = GameFramework::MainGameFramework->m_pPlayer;
		if (recv_packet->id == GameFramework::MainGameFramework->m_pPlayer->id) {
			player = GameFramework::MainGameFramework->m_pPlayer;
		}
		else {
			auto p = find_if(GameFramework::MainGameFramework->m_OtherPlayers.begin(),
				GameFramework::MainGameFramework->m_OtherPlayers.end(),
				[&recv_packet](Object* lhs) {
					return dynamic_cast<Player*>(lhs)->id == recv_packet->id;
				});

			if (p == GameFramework::MainGameFramework->m_OtherPlayers.end())
				break;

			player = dynamic_cast<Player*>(*p);
			dynamic_cast<Player*>(*p)->SetPosition(XMFLOAT3(recv_packet->x, recv_packet->y, recv_packet->z));
		}

		player->SetPosition(XMFLOAT3(recv_packet->x, recv_packet->y, recv_packet->z));

		break;
	}
	case E_PACKET::E_PACKET_SC_LOOK_PLAYER: {
		SC_LOOK_PLAYER_PACKET* recv_packet = reinterpret_cast<SC_LOOK_PLAYER_PACKET*>(p_Packet);
		Player* player = GameFramework::MainGameFramework->m_pPlayer;
		if (recv_packet->id == GameFramework::MainGameFramework->m_pPlayer->id) {
			player = GameFramework::MainGameFramework->m_pPlayer;
		}
		else {
			auto p = find_if(GameFramework::MainGameFramework->m_OtherPlayers.begin(),
				GameFramework::MainGameFramework->m_OtherPlayers.end(),
				[&recv_packet](Object* lhs) {
					return dynamic_cast<Player*>(lhs)->id == recv_packet->id;
				});

			if (p == GameFramework::MainGameFramework->m_OtherPlayers.end())
				break;

			player = dynamic_cast<Player*>(*p);
		}

		XMFLOAT3 xmf3Look{ XMFLOAT3(recv_packet->x, recv_packet->y, recv_packet->z) };
		XMFLOAT3 xmf3UpVector = player->GetUpVector();
		XMFLOAT3 xmf3RightVector = Vector3::CrossProduct(xmf3UpVector, xmf3Look, true);

		player->SetLookVector(xmf3Look);
		player->SetRightVector(xmf3RightVector);
		player->SetUpVector(Vector3::CrossProduct(xmf3Look, xmf3RightVector, true));

		if (player == GameFramework::MainGameFramework->m_pPlayer) {
			player->GetCamera()->SetLookVector(xmf3Look);
			player->GetCamera()->SetRightVector(xmf3RightVector);
			player->GetCamera()->SetUpVector(Vector3::CrossProduct(xmf3Look, xmf3RightVector, true));
			player->GetCamera()->Rotate(player->GetPitch(), 0, 0);
		}
		break;
	}
	default:
		break;
	}
}
