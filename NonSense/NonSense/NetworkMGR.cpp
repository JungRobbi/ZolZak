#include "../ImaysNet/ImaysNet.h"

#include <algorithm>
#include <string>

#include "NetworkMGR.h"
#include "GameScene.h"
#include "GameFramework.h"
#include "PlayerMovementComponent.h"
#include "CloseTypeFSMComponent.h"
#include "CloseTypeState.h"
#include "FarTypeFSMComponent.h"
#include "FarTypeState.h"
#include "RushTypeFSMComponent.h"
#include "RushTypeState.h"
#include "BossFSMComponent.h"
#include "BossState.h"
#include "AttackComponent.h"
#include "UILayer.h"
#include "Lobby_GameScene.h"
#include "UI.h"
#include "Stage_GameScene.h"
#pragma comment(lib, "WS2_32.LIB")

char* NetworkMGR::SERVERIP = "127.0.0.1";

recursive_mutex NetworkMGR::mutex;

shared_ptr<Socket> NetworkMGR::tcpSocket;

int	NetworkMGR::id{ -1 };
string			NetworkMGR::name{};
bool			NetworkMGR::is_mage = true;
bool			NetworkMGR::b_isNet{true};
bool			NetworkMGR::b_isLogin{ false };
bool			NetworkMGR::b_isLoginProg{ false };

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
	
	char isnet = 'n';
	std::cout << "NetworkMGR::start()�� cin�� �ּ�ó�� �ϸ� ����)" << endl;
	std::cout << "��Ʈ��ũ ���� ���� Ű �Է� (y/n - ����O/����X) :";
	std::cin >> isnet;

	if (isnet == 'n') {
		b_isNet = false;
		b_isLogin = true;
		return;
	}
	
	system("cls");
	//
	// ����
	//

	std::cout << std::endl << " ======== Login ======== " << std::endl << std::endl;

	std::cout << std::endl << "���� �� �����ּҸ� �Է����ּ���(ex 197.xxx.xxx.xxx) : " << std::endl;
	std::string server_s;
	std::cin >> server_s;
	SERVERIP = new char[server_s.size() + 1];
	SERVERIP[server_s.size()] = '\0';
	strcpy(SERVERIP, server_s.c_str());




	tcpSocket->Bind(Endpoint::Any);
	NetworkMGR::do_connetion();
	NetworkMGR::do_recv();
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

void NetworkMGR::do_connetion() {
	tcpSocket->Connect(Endpoint(SERVERIP, SERVERPORT));
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
		//SC_LOGIN_INFO_PACKET* recv_packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(p_Packet);
		//cout << "�α��� ���� ����!" << endl;

		//int RoomNum = GameScene::MainScene->SelectNum;
		//std::string ChannelName = "Channel_";
		//char buf[3];
		//::itoa(RoomNum, buf, 10);
		//ChannelName.append(buf);
		//GameFramework::MainGameFramework->GetVivoxSystem()->JoinChannel(ChannelName.c_str());
		//
		//GameFramework::MainGameFramework->ChangeScene(ROOM_SCENE);

	//	GameFramework::MainGameFramework->ChangeScene(SIGHT_SCENE);

		break;
	}
	case E_PACKET::E_PACKET_SC_ADD_PLAYER: {
		SC_ADD_PLAYER_PACKET* recv_packet = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(p_Packet);
		cout << "ADD PLAYER!" << endl;
		if (GameFramework::MainGameFramework->m_OtherPlayers.size() < 3) {
			Player* player;
			if (recv_packet->playerType == 0) { // mage
				player = GameFramework::MainGameFramework->m_OtherPlayersPool.front();
				dynamic_cast<MagePlayer*>(player)->fireball->ownerID = recv_packet->id;
				dynamic_cast<MagePlayer*>(player)->fireball->explode->ownerID = recv_packet->id;
			}
			else { // warrior
				player = GameFramework::MainGameFramework->m_OtherPlayersPool.back();
			}
			dynamic_cast<Player*>(player)->id = recv_packet->id;
			dynamic_cast<Player*>(player)->m_name = recv_packet->name;
			dynamic_cast<Player*>(player)->SetPosition(XMFLOAT3(recv_packet->x, recv_packet->y, recv_packet->z));
			dynamic_cast<Player*>(player)->SetHealth(recv_packet->maxHp);
			dynamic_cast<Player*>(player)->SetRemainHP(recv_packet->remainHp);

			GameFramework::MainGameFramework->m_OtherPlayers.push_back(player);
			auto& other_name = GameFramework::MainGameFramework->m_OtherPlayers.back()->m_name;
			if (recv_packet->playerType == 0) { // mage
				GameFramework::MainGameFramework->m_OtherPlayersPool.pop_front();
			}
			else { // warrior
				GameFramework::MainGameFramework->m_OtherPlayersPool.pop_back();
			}
		}
		else {
			cout << "OtherPlayer ���� ����!" << endl;
		}
		for (int i{}; i < GameFramework::MainGameFramework->m_OtherPlayers.size(); ++i) {
			std::wstring wname;
			wname.assign(GameFramework::MainGameFramework->m_OtherPlayers[i]->m_name.begin(), GameFramework::MainGameFramework->m_OtherPlayers[i]->m_name.end());
			wcscpy_s(ChatMGR::m_pUILayer->m_pUITextBlocks[PLAYER1_NAME_UI + i].m_pstrText, 256, wname.c_str());
		}
		break;
	}
	case E_PACKET::E_PACKET_SC_REMOVE_PLAYER: {
		SC_REMOVE_PLAYER_PACKET* recv_packet = reinterpret_cast<SC_REMOVE_PLAYER_PACKET*>(p_Packet);
		auto leave_id = recv_packet->id;
		auto& OtherPlayers = GameFramework::MainGameFramework->m_OtherPlayers;
		auto leave_player = find_if(OtherPlayers.begin(), OtherPlayers.end(), [&leave_id](Object* lhs) {
			return dynamic_cast<Player*>(lhs)->id == leave_id;
			});
		if (leave_player == OtherPlayers.end())
			break;
		(*leave_player)->SetUsed(false);
		OtherPlayers.erase(leave_player);
		break;
	}
	case E_PACKET::E_PACKET_SC_MOVE_PLAYER: {
		SC_MOVE_PLAYER_PACKET* recv_packet = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(p_Packet);
		Player* player = GameFramework::MainGameFramework->m_pPlayer;
		if (recv_packet->id == NetworkMGR::id) {
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

		player->SetPosition(XMFLOAT3(recv_packet->x, recv_packet->y, recv_packet->z));
		player->GetCamera()->Update(player->GetPosition(), Timer::GetTimeElapsed());
		/*cout << "recv_packet->x = " << recv_packet->x << endl;
		cout << "recv_packet->y = " << recv_packet->y << endl;
		cout << "recv_packet->z = " << recv_packet->z << endl;
		cout << "================ " << endl;*/

		break;
	}
	case E_PACKET::E_PACKET_SC_LOOK_PLAYER: {
		SC_LOOK_PLAYER_PACKET* recv_packet = reinterpret_cast<SC_LOOK_PLAYER_PACKET*>(p_Packet);
		Player* player = GameFramework::MainGameFramework->m_pPlayer;
		if (recv_packet->id == NetworkMGR::id) {
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
	case E_PACKET_SC_ANIMATION_TYPE_PLAYER: {
		SC_PLAYER_ANIMATION_TYPE_PACKET* recv_packet = reinterpret_cast<SC_PLAYER_ANIMATION_TYPE_PACKET*>(p_Packet);
		Player* player = nullptr;
		if (recv_packet->id == NetworkMGR::id) {
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
		
		if ((E_PLAYER_ANIMATION_TYPE)recv_packet->Anitype == E_PLAYER_ANIMATION_TYPE::E_JUMP) {
			player->GetComponent<PlayerMovementComponent>()->b_Jump = true;
		}
		else if ((E_PLAYER_ANIMATION_TYPE)recv_packet->Anitype == E_PLAYER_ANIMATION_TYPE::E_DASH) {
			player->GetComponent<PlayerMovementComponent>()->b_Dash = true;
		}
		else if ((E_PLAYER_ANIMATION_TYPE)recv_packet->Anitype == E_PLAYER_ANIMATION_TYPE::E_ATTACK0) {
			player->GetComponent<AttackComponent>()->b_Attack = true;
		}
		else {
			player->GetComponent<PlayerMovementComponent>()->Animation_type = (E_PLAYER_ANIMATION_TYPE)recv_packet->Anitype;
			cout << "NET Animation_type - " << player->GetComponent<PlayerMovementComponent>()->Animation_type << endl;
		}
		break;
	}
	case E_PACKET_SC_ANIMATION_TYPE_MONSTER: {
		SC_MONSTER_ANIMATION_TYPE_PACKET* recv_packet = reinterpret_cast<SC_MONSTER_ANIMATION_TYPE_PACKET*>(p_Packet);
		
		Character* Monster;
		auto p = find_if(GameScene::MainScene->MonsterObjects.begin(),
			GameScene::MainScene->MonsterObjects.end(),
			[&recv_packet](Object* lhs) {
				return dynamic_cast<Character*>(lhs)->GetNum() == recv_packet->id;
			});

		if (p == GameScene::MainScene->MonsterObjects.end())
			break;

		Monster = dynamic_cast<Character*>(*p);
		if (Monster->GetComponent<CloseTypeFSMComponent>()) {
			Monster->GetComponent<CloseTypeFSMComponent>()->Animation_type = (E_MONSTER_ANIMATION_TYPE)recv_packet->Anitype;
		}
		else if (Monster->GetComponent<FarTypeFSMComponent>()) {
			Monster->GetComponent<FarTypeFSMComponent>()->Animation_type = (E_MONSTER_ANIMATION_TYPE)recv_packet->Anitype;
			if ((E_MONSTER_ANIMATION_TYPE)recv_packet->Anitype == E_MONSTER_ANIMATION_TYPE::E_M_ATTACK)
				Monster->GetComponent<FarTypeFSMComponent>()->Attack();
		}
		else if (Monster->GetComponent<RushTypeFSMComponent>()) {
			Monster->GetComponent<RushTypeFSMComponent>()->Animation_type = (E_MONSTER_ANIMATION_TYPE)recv_packet->Anitype;
			if ((E_MONSTER_ANIMATION_TYPE)recv_packet->Anitype == E_MONSTER_ANIMATION_TYPE::E_M_ATTACK)
				Monster->GetComponent<RushTypeFSMComponent>()->Attack();
		}
		else if (Monster->GetComponent<BossFSMComponent>()) {
			Monster->GetComponent<BossFSMComponent>()->Animation_type = (E_MONSTER_ANIMATION_TYPE)recv_packet->Anitype;
			if ((E_BOSS_ANIMATION_TYPE)recv_packet->Anitype == E_BOSS_ANIMATION_TYPE::E_B_ATTACK)
				Monster->GetComponent<BossFSMComponent>()->Attack();
			else if ((E_BOSS_ANIMATION_TYPE)recv_packet->Anitype == E_BOSS_ANIMATION_TYPE::E_B_ROAR)
				Monster->GetComponent<BossFSMComponent>()->StealSense();
			else if ((E_BOSS_ANIMATION_TYPE)recv_packet->Anitype == E_BOSS_ANIMATION_TYPE::E_B_SUMMON)
				Monster->GetComponent<BossFSMComponent>()->Summon();
			else if ((E_BOSS_ANIMATION_TYPE)recv_packet->Anitype == E_BOSS_ANIMATION_TYPE::E_B_DEFENCE)
				Monster->GetComponent<BossFSMComponent>()->Defence();
			else if ((E_BOSS_ANIMATION_TYPE)recv_packet->Anitype == E_BOSS_ANIMATION_TYPE::E_B_JUMPATTACK)
				Monster->GetComponent<BossFSMComponent>()->JumpAttack();
			else if ((E_BOSS_ANIMATION_TYPE)recv_packet->Anitype == E_BOSS_ANIMATION_TYPE::E_B_TORNADO) {
				Monster->GetComponent<BossFSMComponent>()->Tornado();
				Monster->GetComponent<BossFSMComponent>()->IsTornado = true;
			}
			else if ((E_BOSS_ANIMATION_TYPE)recv_packet->Anitype == E_BOSS_ANIMATION_TYPE::E_B_IDLE) {
				Monster->GetComponent<BossAttackComponent>()->During_Skill = false;
			}
		}
		break;
	}
	case E_PACKET_SC_MOVE_MONSTER_PACKET: {
		SC_MOVE_MONSTER_PACKET* recv_packet = reinterpret_cast<SC_MOVE_MONSTER_PACKET*>(p_Packet);

		Character* Monster;
		auto p = find_if(GameScene::MainScene->MonsterObjects.begin(),
			GameScene::MainScene->MonsterObjects.end(),
			[&recv_packet](Object* lhs) {
				return dynamic_cast<Character*>(lhs)->GetNum() == recv_packet->id;
			});

		if (p == GameScene::MainScene->MonsterObjects.end())
			break;

		Monster = dynamic_cast<Character*>(*p);
		Monster->SetPosition(recv_packet->x, recv_packet->y, recv_packet->z);

		break;
	}
	case E_PACKET_SC_AGGRO_PLAYER_PACKET: {
		SC_AGGRO_PLAYER_PACKET* recv_packet = reinterpret_cast<SC_AGGRO_PLAYER_PACKET*>(p_Packet);

		Player* player;
		Character* Monster;
		if (recv_packet->player_id == NetworkMGR::id) {
			player = GameFramework::MainGameFramework->m_pPlayer;
		}
		else {
			auto p = find_if(GameFramework::MainGameFramework->m_OtherPlayers.begin(),
				GameFramework::MainGameFramework->m_OtherPlayers.end(),
				[&recv_packet](Object* lhs) {
					return dynamic_cast<Player*>(lhs)->id == recv_packet->player_id;
				});

			if (p == GameFramework::MainGameFramework->m_OtherPlayers.end())
				break;

			player = dynamic_cast<Player*>(*p);
		}

		{
			auto p = find_if(GameScene::MainScene->MonsterObjects.begin(),
				GameScene::MainScene->MonsterObjects.end(),
				[&recv_packet](Object* lhs) {
					return dynamic_cast<Character*>(lhs)->GetNum() == recv_packet->monster_id;
				});

			if (p == GameScene::MainScene->MonsterObjects.end())
				break;

			Monster = dynamic_cast<Character*>(*p);
		}

		if(Monster->GetComponent<CloseTypeFSMComponent>())
			Monster->GetComponent<CloseTypeFSMComponent>()->SetTargetPlayer(player);
		else if (Monster->GetComponent<FarTypeFSMComponent>())
			Monster->GetComponent<FarTypeFSMComponent>()->SetTargetPlayer(player);
		else if (Monster->GetComponent<RushTypeFSMComponent>())
			Monster->GetComponent<RushTypeFSMComponent>()->SetTargetPlayer(player);
		else if (Monster->GetComponent<BossFSMComponent>())
			Monster->GetComponent<BossFSMComponent>()->SetTargetPlayer(player);
		break;
	}
	case E_PACKET_SC_TEMP_HIT_MONSTER_PACKET: {
		SC_TEMP_HIT_MONSTER_PACKET* recv_packet = reinterpret_cast<SC_TEMP_HIT_MONSTER_PACKET*>(p_Packet);
		Character* monster;
		{
			auto p = find_if(GameScene::MainScene->MonsterObjects.begin(),
				GameScene::MainScene->MonsterObjects.end(),
				[&recv_packet](Object* lhs) {
					return dynamic_cast<Character*>(lhs)->GetNum() == recv_packet->monster_id;
				});

			if (p == GameScene::MainScene->MonsterObjects.end())
				break;

			monster = dynamic_cast<Character*>(*p);
		}

		monster->SetRemainHP(recv_packet->remain_hp);
		if (recv_packet->remain_hp > 0.0f) {
			monster->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_M_HIT);
			dynamic_cast<Monster*>(monster)->HitSound();
		}
		break;
	}
	case E_PACKET_SC_TEMP_HIT_PLAYER_PACKET: {
		SC_TEMP_HIT_PLAYER_PACKET* recv_packet = reinterpret_cast<SC_TEMP_HIT_PLAYER_PACKET*>(p_Packet);
		Player* player;
		if (recv_packet->player_id == NetworkMGR::id) {
			player = GameFramework::MainGameFramework->m_pPlayer;
		}
		else {
			auto p = find_if(GameFramework::MainGameFramework->m_OtherPlayers.begin(),
				GameFramework::MainGameFramework->m_OtherPlayers.end(),
				[&recv_packet](Object* lhs) {
					return dynamic_cast<Player*>(lhs)->id == recv_packet->player_id;
				});

			if (p == GameFramework::MainGameFramework->m_OtherPlayers.end())
				break;

			player = dynamic_cast<Player*>(*p);
		}

		player->SetRemainHP(recv_packet->remain_hp);

		for (auto& p : GameFramework::MainGameFramework->m_OtherPlayers)
		{
			if (p->m_RemainHP > p->m_Health)
			{
				p->m_RemainHP = p->m_Health;
			}
		}
		if (GameFramework::MainGameFramework->m_pPlayer->m_RemainHP > GameFramework::MainGameFramework->m_pPlayer->m_Health)
		{
			GameFramework::MainGameFramework->m_pPlayer->m_RemainHP = GameFramework::MainGameFramework->m_pPlayer->m_Health;
		}

		GameFramework::MainGameFramework->m_pPlayer->m_pHP_Dec_UI->Dec_HP = GameFramework::MainGameFramework->m_pPlayer->m_RemainHP / 1000;
		GameFramework::MainGameFramework->m_pPlayer->m_pOverHP_Dec_UI->Dec_HP = (GameFramework::MainGameFramework->m_pPlayer->m_RemainHP - 1000) / 1000;
		GameFramework::MainGameFramework->m_pPlayer->m_pHP_Dec_UI->HP = GameFramework::MainGameFramework->m_pPlayer->m_pHP_Dec_UI->Dec_HP;
		GameFramework::MainGameFramework->m_pPlayer->m_pOverHP_Dec_UI->HP = GameFramework::MainGameFramework->m_pPlayer->m_pOverHP_Dec_UI->Dec_HP;

		if (GameFramework::MainGameFramework->m_pPlayer->m_RemainHP <= 1000)
		{
			GameFramework::MainGameFramework->m_pPlayer->m_pOverHP_Dec_UI->Dec_HP = 0;
			GameFramework::MainGameFramework->m_pPlayer->m_pOverHP_Dec_UI->HP = 0;
		}

		if (GameFramework::MainGameFramework->m_pPlayer->m_RemainHP >= 1000)
		{
			GameFramework::MainGameFramework->m_pPlayer->m_pHP_Dec_UI->Dec_HP = 1;
			GameFramework::MainGameFramework->m_pPlayer->m_pHP_Dec_UI->HP = 1;
		}

		if (player->m_RemainHP <= 0)
		{
			player->m_die = true;
			if (recv_packet->player_id == NetworkMGR::id) {
				CS_DIE_PACKET send_packet;
				send_packet.size = sizeof(CS_DIE_PACKET);
				send_packet.type = E_PACKET::E_PACKET_CS_DIE_PACKET;
				PacketQueue::AddSendPacket(&send_packet);
			}
		}
		cout << "\n���� - " << GameFramework::MainGameFramework->m_pPlayer->m_RemainHP << endl;
		break;
	}
	case E_PACKET_SC_LOOK_MONSTER_PACKET: {
		SC_LOOK_MONSTER_PACKET* recv_packet = reinterpret_cast<SC_LOOK_MONSTER_PACKET*>(p_Packet);
		Character* Monster;
		{
			auto p = find_if(GameScene::MainScene->MonsterObjects.begin(),
				GameScene::MainScene->MonsterObjects.end(),
				[&recv_packet](Object* lhs) {
					return dynamic_cast<Character*>(lhs)->GetNum() == recv_packet->id;
				});

			if (p == GameScene::MainScene->MonsterObjects.end())
				break;

			Monster = dynamic_cast<Character*>(*p);
		}
		if (Monster->GetComponent<CloseTypeFSMComponent>())
			Monster->GetComponent<CloseTypeFSMComponent>()->WanderPosition = XMFLOAT3(recv_packet->x, recv_packet->y, recv_packet->z);
		else if (Monster->GetComponent<FarTypeFSMComponent>())
			Monster->GetComponent<FarTypeFSMComponent>()->WanderPosition = XMFLOAT3(recv_packet->x, recv_packet->y, recv_packet->z);
		else if (Monster->GetComponent<RushTypeFSMComponent>())
			Monster->GetComponent<RushTypeFSMComponent>()->WanderPosition = XMFLOAT3(recv_packet->x, recv_packet->y, recv_packet->z);
		else if (Monster->GetComponent<BossFSMComponent>())
			Monster->GetComponent<BossFSMComponent>()->WanderPosition = XMFLOAT3(recv_packet->x, recv_packet->y, recv_packet->z);
		break;
	}
	case E_PACKET_SC_TEMP_WANDER_MONSTER_PACKET: {
		SC_TEMP_WANDER_MONSTER_PACKET* recv_packet = reinterpret_cast<SC_TEMP_WANDER_MONSTER_PACKET*>(p_Packet);
		Character* Monster;
		{
			auto p = find_if(GameScene::MainScene->MonsterObjects.begin(),
				GameScene::MainScene->MonsterObjects.end(),
				[&recv_packet](Object* lhs) {
					return dynamic_cast<Character*>(lhs)->GetNum() == recv_packet->id;
				});

			if (p == GameScene::MainScene->MonsterObjects.end())
				break;

			Monster = dynamic_cast<Character*>(*p);
		}
		if (Monster->GetComponent<CloseTypeFSMComponent>() && Monster->GetComponent<CloseTypeFSMComponent>()->GetFSM()->GetCurrentState() != WanderState::GetInstance())
			Monster->GetComponent<CloseTypeFSMComponent>()->GetFSM()->ChangeState(WanderState::GetInstance());
		else if (Monster->GetComponent<FarTypeFSMComponent>() && Monster->GetComponent<FarTypeFSMComponent>()->GetFSM()->GetCurrentState() != WanderState_Far::GetInstance())
			Monster->GetComponent<FarTypeFSMComponent>()->GetFSM()->ChangeState(WanderState_Far::GetInstance());
		else if (Monster->GetComponent<RushTypeFSMComponent>() && Monster->GetComponent<RushTypeFSMComponent>()->GetFSM()->GetCurrentState() != WanderState_Rush::GetInstance())
			Monster->GetComponent<RushTypeFSMComponent>()->GetFSM()->ChangeState(WanderState_Rush::GetInstance());
		else if (Monster->GetComponent<BossFSMComponent>() && Monster->GetComponent<BossFSMComponent>()->GetFSM()->GetCurrentState() != WanderState_Boss::GetInstance())
			Monster->GetComponent<BossFSMComponent>()->GetFSM()->ChangeState(WanderState_Boss::GetInstance());
		break;
	}
	case E_PACKET_SC_CHAT_PACKET: {
		SC_CHAT_PACKET* recv_packet = reinterpret_cast<SC_CHAT_PACKET*>(p_Packet);
		
		auto p = ConverCtoWC(recv_packet->chat);
		ChatMGR::StoreText(p);
		delete p;
		break;
	}
	case E_PACKET_SC_LOGIN_FAIL_PACKET: {
		b_isLoginProg = false;
		b_isLogin = false;
		std::cout << "�α��� ����!" << std::endl;
		break;
	}
	case E_PACKET_SC_LOGIN_OK_PACKET: {
		std::cout << "�α��� ����!" << std::endl;
		if (b_isLoginProg) { // �α��� ���� �ϴ� ����
			b_isLogin = true;
			GameFramework::MainGameFramework->InitializeVivoxSystem(name);
			
			GameFramework::MainGameFramework->ChangeScene(LOBBY_SCENE);
			b_isLoginProg = false;
		}
		break;
	}
	case E_PACKET_SC_ROOM_CREATE_PACKET: {
		SC_ROOM_CREATE_PACKET* recv_packet = reinterpret_cast<SC_ROOM_CREATE_PACKET*>(p_Packet);
		
		if (GameFramework::MainGameFramework->scene_type == LOBBY_SCENE) {
			cout << "Make Room!" << endl;
			dynamic_cast<Lobby_GameScene*>(GameScene::MainScene)->MakeRoom(recv_packet->roomNum, std::string{ recv_packet->roomName }, std::string{ recv_packet->hostName });
			
			if (strcmp(recv_packet->hostName, NetworkMGR::name.c_str()) == 0) {
				CS_ROOM_JOIN_PACKET send_packet;
				send_packet.size = sizeof(CS_ROOM_JOIN_PACKET);
				send_packet.type = E_PACKET::E_PACKET_CS_ROOM_JOIN_PACKET;
				send_packet.roomNum = recv_packet->roomNum;
				PacketQueue::AddSendPacket(&send_packet);
			}
		}
		
		break;
	}
	case E_PACKET_SC_ROOM_JOIN_OK_PACKET: {
		SC_ROOM_JOIN_OK_PACKET* recv_packet = reinterpret_cast<SC_ROOM_JOIN_OK_PACKET*>(p_Packet);
		NetworkMGR::id = recv_packet->id;

		int RoomNum = GameScene::MainScene->SelectNum;
		std::string ChannelName = "Channel_";
		char buf[3];
		::itoa(RoomNum, buf, 10);
		ChannelName.append(buf);
		GameFramework::MainGameFramework->GetVivoxSystem()->JoinChannel(ChannelName.c_str());
	
		GameFramework::MainGameFramework->ChangeScene(ROOM_SCENE);
		break;
	}
	case E_PACKET_SC_ROOM_READY_PACKET: {
		SC_ROOM_READY_PACKET* recv_packet = reinterpret_cast<SC_ROOM_READY_PACKET*>(p_Packet);
		// Room UI ����
		if (recv_packet->playerType == 0) { // mage
			auto wname = ConverCtoWC(recv_packet->name);
			std::wstring wstr{ wname };
			wstr += L" Player - MAGE Ready!";

			ChatMGR::StoreText(wstr.c_str());
			delete wname;
		}
		else { // warrior
			auto wname = ConverCtoWC(recv_packet->name);
			std::wstring wstr{ wname };
			wstr += L" Player - WARRIOR Ready!";

			ChatMGR::StoreText(wstr.c_str());
			delete wname;
		}

		break;
	}
	case E_PACKET_SC_ROOM_UNREADY_PACKET: {
		SC_ROOM_UNREADY_PACKET* recv_packet = reinterpret_cast<SC_ROOM_UNREADY_PACKET*>(p_Packet);
		// Room UI ����

		auto wname = ConverCtoWC(recv_packet->name);
		std::wstring wstr{ wname };
		wstr += L" Player - UnReady!";

		ChatMGR::StoreText(wstr.c_str());
		delete wname;
		break; 
	}
	case E_PACKET_SC_JOIN_GAME_PACKET: {
		SC_JOIN_GAME_PACKET* recv_packet = reinterpret_cast<SC_JOIN_GAME_PACKET*>(p_Packet);
		NetworkMGR::id = recv_packet->id;
		GameFramework::MainGameFramework->ChangeScene(SIGHT_SCENE);
		auto& player = GameFramework::MainGameFramework->m_pPlayer;
		if (player) {
			player->SetPosition(XMFLOAT3(recv_packet->x, recv_packet->y, recv_packet->z));
			player->GetCamera()->Update(player->GetPosition(), Timer::GetTimeElapsed());
			player->SetHealth(recv_packet->maxHp);
			player->SetRemainHP(recv_packet->remainHp);
			GameFramework::MainGameFramework->m_clearStage = recv_packet->clearStage;
		}

		{
			CS_JOIN_COMPLETE_PACKET send_packet;
			send_packet.size = sizeof(CS_JOIN_COMPLETE_PACKET);
			send_packet.type = E_PACKET::E_PACKET_CS_JOIN_COMPLETE_PACKET;
			PacketQueue::AddSendPacket(&send_packet);
		}
		break;
	}
	case E_PACKET_SC_EVERYONE_JOIN_PACKET: {
		CS_PLAYERS_REQUEST_PACKET send_packet;
		send_packet.size = sizeof(CS_PLAYERS_REQUEST_PACKET);
		send_packet.type = E_PACKET::E_PACKET_CS_PLAYERS_REQUEST_PACKET;
		PacketQueue::AddSendPacket(&send_packet);
		break;
	}
	case E_PACKET_SC_PROJECTILE_ATTACK_PACKET: {
		SC_PROJECTILE_ATTACK_PACKET* recv_packet = reinterpret_cast<SC_PROJECTILE_ATTACK_PACKET*>(p_Packet);
		if (recv_packet->id < 10000) { //player
			Player* player;
			if (recv_packet->id == NetworkMGR::id) {
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
			player->GetComponent<AttackComponent>()->ProjectileAttack(XMFLOAT3{ recv_packet->x, recv_packet->y, recv_packet->z});
		}
		else { //monster
			Character* Monster;
			{
				auto p = find_if(GameScene::MainScene->MonsterObjects.begin(),
					GameScene::MainScene->MonsterObjects.end(),
					[&recv_packet](Object* lhs) {
						return dynamic_cast<Character*>(lhs)->GetNum() == recv_packet->id;
					});

				if (p == GameScene::MainScene->MonsterObjects.end())
					break;

				Monster = dynamic_cast<Character*>(*p);
			}
		}
		break;
	}
	case E_PACKET_SC_PLAYER_DIE_PACKET: {
		SC_PLAYER_DIE_PACKET* recv_packet = reinterpret_cast<SC_PLAYER_DIE_PACKET*>(p_Packet);
		Player* player;
		if (recv_packet->player_id == NetworkMGR::id) {
			player = GameFramework::MainGameFramework->m_pPlayer;
			Die = true;
		}
		else {
			auto p = find_if(GameFramework::MainGameFramework->m_OtherPlayers.begin(),
				GameFramework::MainGameFramework->m_OtherPlayers.end(),
				[&recv_packet](Object* lhs) {
					return dynamic_cast<Player*>(lhs)->id == recv_packet->player_id;
				});

			if (p == GameFramework::MainGameFramework->m_OtherPlayers.end())
				break;

			player = dynamic_cast<Player*>(*p);
		}
		player->m_die = true;
		break;
	}				
	case E_PACKET_SC_CLEAR_PACKET: {
		SC_CLEAR_PACKET* recv_packet = reinterpret_cast<SC_CLEAR_PACKET*>(p_Packet);

		if (recv_packet->ClearScene == (int)BOSS_SCENE) {
			GameFramework::MainGameFramework->GameSceneState = LOBBY_SCENE;
			GameFramework::MainGameFramework->ChangeScene(LOBBY_SCENE);
			GameFramework::MainGameFramework->GetVivoxSystem()->LeaveChannel();
			XMFLOAT3 p = { 0,0,0 };
			GameFramework::MainGameFramework->MainBGM = new Sound("Sound/LobbyBGM.mp3", FMOD_2D | FMOD_LOOP_NORMAL, &p);
			GameFramework::MainGameFramework->MainBGM->SetVolume(0.3);
		}
		else {
			GameFramework::MainGameFramework->GameSceneState = recv_packet->ClearScene;
			GameFramework::MainGameFramework->ChangeScene(recv_packet->ClearScene + 1);
		}
		break;
	}
	case E_PACKET_SC_EAT_ITEM_PACKET: {
		SC_EAT_ITEM_PACKET* recv_packet = reinterpret_cast<SC_EAT_ITEM_PACKET*>(p_Packet);
		auto p = find_if(GameScene::MainScene->ForwardObjects.begin(), GameScene::MainScene->ForwardObjects.end(),
			[&recv_packet](Object* lhs) { return lhs->ObjectID == recv_packet->itemNum; });
		if (p == GameScene::MainScene->ForwardObjects.end()) {
			cout << "알 수 없는 ObjectID (Item)" << endl;
			break;
		}
		Player* player;
		if (recv_packet->player_id == NetworkMGR::id) {
			player = GameFramework::MainGameFramework->m_pPlayer;
		}
		else {
			auto p = find_if(GameFramework::MainGameFramework->m_OtherPlayers.begin(),
				GameFramework::MainGameFramework->m_OtherPlayers.end(),
				[&recv_packet](Object* lhs) {
					return dynamic_cast<Player*>(lhs)->id == recv_packet->player_id;
				});

			if (p == GameFramework::MainGameFramework->m_OtherPlayers.end())
				break;

			player = dynamic_cast<Player*>(*p);
		}


		if (((Item*)(*p))->ItemID == 0) // ATK
		{
			player->m_Attack += 20;
		}

		else if (((Item*)(*p))->ItemID == 1) // DEF
		{
			player->m_Defense += 10;
		}

		else if (((Item*)(*p))->ItemID == 2) // HP
		{
			if (GameFramework::MainGameFramework->m_pPlayer->m_Health < 2000)
			{
				player->m_Health += 100;
				player->m_RemainHP += 100;
				player->m_pHP_Dec_UI->Dec_HP = (player->m_RemainHP) / 1000;
				player->m_pOverHP_Dec_UI->Dec_HP = (player->m_RemainHP - 1000) / 1000;
				if (player->m_RemainHP <= 1000)
				{
					player->m_pOverHP_Dec_UI->Dec_HP = 0;
				}

				if (player->m_RemainHP >= 1000)
				{
					player->m_pHP_Dec_UI->Dec_HP = 1;
				}
				player->m_pHP_Dec_UI->HP = player->m_pHP_Dec_UI->Dec_HP;
				player->m_pOverHP_Dec_UI->HP = player->m_pOverHP_Dec_UI->Dec_HP;
			}
		}
		else if (((Item*)(*p))->ItemID == 3) // Eye
		{
			GameScene::MainScene->HaveEye = true;
		}
		else if (((Item*)(*p))->ItemID == 4) // Ear
		{
			GameScene::MainScene->HaveEar = true;
		}
		else if (((Item*)(*p))->ItemID == 5) // Hand
		{
			GameScene::MainScene->HaveHand = true;
		}

		if (!((Item*)(*p))->erase) {
			GameScene::MainScene->deletionBlendQueue.push_back(*p);
			((Item*)(*p))->erase = true;
		}
		break;
	}
	case E_PACKET_SC_CREATE_ITEM_PACKET: {
		SC_CREATE_ITEM_PACKET* recv_packet = reinterpret_cast<SC_CREATE_ITEM_PACKET*>(p_Packet);
		dynamic_cast<Stage_GameScene*>(GameScene::MainScene)->CreateItemList.emplace_back(recv_packet->itemNum, recv_packet->itemID, recv_packet->x, recv_packet->y, recv_packet->z);
		break;
	}
	case E_PACKET_SC_SKILL_HEAL_PACKET: {
		cout << "�� ���� - " << GameFramework::MainGameFramework->m_pPlayer->m_RemainHP << endl;
		SC_SKILL_HEAL_PACKET* recv_packet = reinterpret_cast<SC_SKILL_HEAL_PACKET*>(p_Packet);

		Player* player;
		if (recv_packet->player_id == NetworkMGR::id) {
			player = GameFramework::MainGameFramework->m_pPlayer;
		}
		else {
			auto p = find_if(GameFramework::MainGameFramework->m_OtherPlayers.begin(),
				GameFramework::MainGameFramework->m_OtherPlayers.end(),
				[&recv_packet](Object* lhs) {
					return dynamic_cast<Player*>(lhs)->id == recv_packet->player_id;
				});

			if (p == GameFramework::MainGameFramework->m_OtherPlayers.end())
				break;

			player = dynamic_cast<Player*>(*p);
		}
		player->GetComponent<AttackComponent>()->SkillAnimate();

		//for (auto& p : GameFramework::MainGameFramework->m_OtherPlayers)
		//{
		//	p->m_RemainHP += 200;
		//	if (p->m_RemainHP > p->m_Health)
		//	{
		//		p->m_RemainHP = p->m_Health;
		//	}
		//	p->OnHealUI = Timer::GetTotalTime() + 2.5f;
		//}

		//GameFramework::MainGameFramework->m_pPlayer->m_RemainHP += 200;
		//if (GameFramework::MainGameFramework->m_pPlayer->m_RemainHP > GameFramework::MainGameFramework->m_pPlayer->m_Health)
		//{
		//	GameFramework::MainGameFramework->m_pPlayer->m_RemainHP = GameFramework::MainGameFramework->m_pPlayer->m_Health;
		//}
		////////////////////////////// ���� ��

		//GameFramework::MainGameFramework->m_pPlayer->m_pHP_Dec_UI->Dec_HP = GameFramework::MainGameFramework->m_pPlayer->m_RemainHP / 1000;
		//GameFramework::MainGameFramework->m_pPlayer->m_pOverHP_Dec_UI->Dec_HP = (GameFramework::MainGameFramework->m_pPlayer->m_RemainHP - 1000) / 1000;
		//GameFramework::MainGameFramework->m_pPlayer->m_pHP_Dec_UI->HP = GameFramework::MainGameFramework->m_pPlayer->m_pHP_Dec_UI->Dec_HP;
		//GameFramework::MainGameFramework->m_pPlayer->m_pOverHP_Dec_UI->HP = GameFramework::MainGameFramework->m_pPlayer->m_pOverHP_Dec_UI->Dec_HP;

		//if (GameFramework::MainGameFramework->m_pPlayer->m_RemainHP <= 1000)
		//{
		//	GameFramework::MainGameFramework->m_pPlayer->m_pOverHP_Dec_UI->Dec_HP = 0;
		//	GameFramework::MainGameFramework->m_pPlayer->m_pOverHP_Dec_UI->HP = 0;
		//}

		//if (GameFramework::MainGameFramework->m_pPlayer->m_RemainHP >= 1000)
		//{
		//	GameFramework::MainGameFramework->m_pPlayer->m_pHP_Dec_UI->Dec_HP = 1;
		//	GameFramework::MainGameFramework->m_pPlayer->m_pHP_Dec_UI->HP = 1;
		//}

		GameFramework::MainGameFramework->m_pPlayer->OnHealUI = Timer::GetTotalTime() + 2.5f;

		break;
	}
	case E_PACKET_SC_SKILL_HEALTHUP_PACKET: {
		SC_SKILL_HEALTHUP_PACKET* recv_packet = reinterpret_cast<SC_SKILL_HEALTHUP_PACKET*>(p_Packet);

		Player* player;
		if (recv_packet->player_id == NetworkMGR::id) {
			player = GameFramework::MainGameFramework->m_pPlayer;
		}
		else {
			auto p = find_if(GameFramework::MainGameFramework->m_OtherPlayers.begin(),
				GameFramework::MainGameFramework->m_OtherPlayers.end(),
				[&recv_packet](Object* lhs) {
					return dynamic_cast<Player*>(lhs)->id == recv_packet->player_id;
				});

			if (p == GameFramework::MainGameFramework->m_OtherPlayers.end())
				break;

			player = dynamic_cast<Player*>(*p);
		}
		player->GetComponent<AttackComponent>()->SkillAnimate();
		player->OnBuffUI = Timer::GetTotalTime() + 10.0f;

		break;
	}
	default:
		break;
	}
}
