#include "Room.h"
#include "Characters.h"
#include "RemoteClients/RemoteClient.h"

#include "Components/PlayerMovementComponent.h"
#include "Components/CloseTypeFSMComponent.h"
#include "Components/FarTypeFSMComponent.h"
#include "Components/RushTypeFSMComponent.h"
#include "Components/SphereCollideComponent.h"
#include "Components/BoxCollideComponent.h"

std::vector<shared_ptr<Room>> Room::roomlist{};

Room::Room()
{
	scene = make_shared<Scene>();
}

Room::~Room()
{
	Clients.clear();
}

void Room::start()
{
	Object* TempObject = NULL;
	TempObject = new Goblin(MONSTER_TYPE_FAR, m_roomNum);
	TempObject->SetPosition(-9.0f, Scene::terrain->GetHeight(-9.0f, 9.0f), 87);
	((Goblin*)TempObject)->num = 10001;
	TempObject->m_roomNum = m_roomNum;
	TempObject = new Goblin(MONSTER_TYPE_FAR, m_roomNum);
	TempObject->SetPosition(-1.0f, Scene::terrain->GetHeight(-1.0f, 42.0f), 42.0f);
	((Goblin*)TempObject)->num = 10002;
	TempObject->m_roomNum = m_roomNum;
	TempObject = new Goblin(MONSTER_TYPE_CLOSE, m_roomNum);
	TempObject->SetPosition(16.0f, Scene::terrain->GetHeight(16.0f, 34.0f), 34.0f);
	((Goblin*)TempObject)->num = 10003;
	TempObject->m_roomNum = m_roomNum;
	TempObject = new Goblin(MONSTER_TYPE_CLOSE, m_roomNum);
	TempObject->SetPosition(53.0f, Scene::terrain->GetHeight(53.0f, 43.0f), 43.0f);
	((Goblin*)TempObject)->num = 10004;
	TempObject->m_roomNum = m_roomNum;
	TempObject = new Goblin(MONSTER_TYPE_CLOSE, m_roomNum);
	TempObject->SetPosition(89.0f, Scene::terrain->GetHeight(89.0f, 33.0f), 33.0f);
	((Goblin*)TempObject)->num = 10005;
	TempObject->m_roomNum = m_roomNum;
	TempObject = new Goblin(MONSTER_TYPE_CLOSE, m_roomNum);
	TempObject->SetPosition(113.0f, Scene::terrain->GetHeight(113.0f, 20.0f), 20.0f);
	((Goblin*)TempObject)->num = 10006;
	TempObject->m_roomNum = m_roomNum;

}

void Room::update()
{
	scene->update();

	//for (auto& rc : Clients) {
		//	if (!rc.second->b_Enable.load())
		//		continue;

		//	// Animation Packet
		//	{ 
		//		if (rc.second->m_KeyInput.keys['w'] || rc.second->m_KeyInput.keys['W'] ||
		//			rc.second->m_KeyInput.keys['s'] || rc.second->m_KeyInput.keys['S'] ||
		//			rc.second->m_KeyInput.keys['a'] || rc.second->m_KeyInput.keys['A'] ||
		//			rc.second->m_KeyInput.keys['d'] || rc.second->m_KeyInput.keys['D']) {
		//			if (rc.second->m_KeyInput.keys[16]) { // LSHIFT
		//				rc.second->m_pPlayer->PresentAniType = E_PLAYER_ANIMATION_TYPE::E_WALK;
		//			}
		//			else {
		//				rc.second->m_pPlayer->PresentAniType = E_PLAYER_ANIMATION_TYPE::E_RUN;
		//			}
		//		}
		//		else {
		//			//	if (!((Player*)gameObject)->GetComponent<AttackComponent>()->During_Attack) 컴포넌트 추가 해야함
		//			rc.second->m_pPlayer->PresentAniType = E_PLAYER_ANIMATION_TYPE::E_IDLE;
		//		}

		//		if (rc.second->m_pPlayer->OldAniType.load() != rc.second->m_pPlayer->PresentAniType) {

		//			shared_ptr<EXP_OVER> p = make_shared<EXP_OVER>();
		//			p->m_ioType = IO_TYPE::IO_TIMER_PLAYER_ANIMATION;
		//			PostQueuedCompletionStatus(iocp.m_hIocp, 1, (ULONG_PTR)rc.second.get(), &p->_wsa_over);

		//			int expected = rc.second->m_pPlayer->OldAniType.load();
		//			rc.second->m_pPlayer->OldAniType.compare_exchange_strong(expected, rc.second->m_pPlayer->PresentAniType.load());
		//		}
		//	}

		//	rc.second->m_pPlayer->update();
		//	
		//	// Move Packet
		//	{ 
		//		auto vel = rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetVelocity();

		//		if (Vector3::Length(vel) > 3.0001f) {
		//			//cout << "Vector3::Length(vel) = " << Vector3::Length(vel) << endl;
		//			//cout << "vel->x = " << vel.x << endl;
		//			//cout << "vel->y = " << vel.y << endl;
		//			//cout << "vel->z = " << vel.z << endl;
		//			//cout << "================ " << endl;

		//			shared_ptr<EXP_OVER> p = make_shared<EXP_OVER>();
		//			p->m_ioType = IO_TYPE::IO_TIMER_PLAYER_MOVE;
		//			PostQueuedCompletionStatus(iocp.m_hIocp, 1, (ULONG_PTR)rc.second.get(), &p->_wsa_over);
		//		}
		//	}

		//	// Camera Look Packet
		//	if (rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->is_Rotate){
		//		shared_ptr<EXP_OVER> p = make_shared<EXP_OVER>();
		//		p->m_ioType = IO_TYPE::IO_TIMER_PLAYER_LOOK;
		//		PostQueuedCompletionStatus(iocp.m_hIocp, 1, (ULONG_PTR)rc.second.get(), &p->_wsa_over);
		//		rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->is_Rotate = false;
		//	}
		//}

		////Monster test
		//++MonsterTimerDelay;
		//if (MonsterTimerDelay <= MAX_MonsterTimerDelay) 
		//	continue;
		//MonsterTimerDelay = 0;
		//for (auto& monster : Scene::scene->MonsterObjects) {
		//	if (monster->GetRemainHP() <= 0.f)
		//		continue;

		//	/*{
		//		shared_ptr<EXP_OVER> p = make_shared<EXP_OVER>();
		//		p->m_ioType = IO_TYPE::IO_TIMER_MONSTER_MOVE;
		//		PostQueuedCompletionStatus(iocp.m_hIocp, 1, (ULONG_PTR)(monster), &p->_wsa_over);
		//	}*/
		//	/*{
		//		shared_ptr<EXP_OVER> p = make_shared<EXP_OVER>();
		//		p->m_ioType = IO_TYPE::IO_TIMER_MONSTER_WANDER;
		//		PostQueuedCompletionStatus(iocp.m_hIocp, 1, (ULONG_PTR)(monster), &p->_wsa_over);
		//	}
		//	{
		//		shared_ptr<EXP_OVER> p = make_shared<EXP_OVER>();
		//		p->m_ioType = IO_TYPE::IO_TIMER_MONSTER_ANIMATION;
		//		PostQueuedCompletionStatus(iocp.m_hIocp, 1, (ULONG_PTR)(monster), &p->_wsa_over);
		//	}
		//	{
		//		shared_ptr<EXP_OVER> p = make_shared<EXP_OVER>();
		//		p->m_ioType = IO_TYPE::IO_TIMER_MONSTER_TARGET;
		//		PostQueuedCompletionStatus(iocp.m_hIocp, 1, (ULONG_PTR)(monster), &p->_wsa_over);
		//	}*/
		//}

	for (auto& rc : Clients) {
		if (!rc.second->b_Enable)
			continue;

		// Animation Packet
		{
			if (rc.second->m_KeyInput.keys['w'] || rc.second->m_KeyInput.keys['W'] ||
				rc.second->m_KeyInput.keys['s'] || rc.second->m_KeyInput.keys['S'] ||
				rc.second->m_KeyInput.keys['a'] || rc.second->m_KeyInput.keys['A'] ||
				rc.second->m_KeyInput.keys['d'] || rc.second->m_KeyInput.keys['D']) {
				if (rc.second->m_KeyInput.keys[16]) { // LSHIFT
					rc.second->m_pPlayer->PresentAniType = E_PLAYER_ANIMATION_TYPE::E_WALK;
				}
				else {
					rc.second->m_pPlayer->PresentAniType = E_PLAYER_ANIMATION_TYPE::E_RUN;
				}
			}
			else {
				//	if (!((Player*)gameObject)->GetComponent<AttackComponent>()->During_Attack) 컴포넌트 추가 해야함
				rc.second->m_pPlayer->PresentAniType = E_PLAYER_ANIMATION_TYPE::E_IDLE;
			}

			if (rc.second->m_pPlayer->OldAniType != rc.second->m_pPlayer->PresentAniType) {
				for (auto& rc_to : Clients) {
					if (!rc_to.second->b_Enable)
						continue;
					SC_PLAYER_ANIMATION_TYPE_PACKET send_packet;
					send_packet.size = sizeof(SC_PLAYER_ANIMATION_TYPE_PACKET);
					send_packet.type = E_PACKET::E_PACKET_SC_ANIMATION_TYPE_PLAYER;
					send_packet.id = rc.second->m_id;
					send_packet.Anitype = (char)rc.second->m_pPlayer->PresentAniType;
					rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
				}
				int expected = rc.second->m_pPlayer->OldAniType.load();
				rc.second->m_pPlayer->OldAniType.compare_exchange_strong(expected, rc.second->m_pPlayer->PresentAniType.load());
			}
		}

		rc.second->m_pPlayer->update();

		// Move Packet
		{
			auto vel = rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetVelocity();

			if (!Vector3::Length(vel))
				continue;

			auto rc_pos = rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetPosition();
			for (auto& rc_to : Clients) {
				if (!rc_to.second->b_Enable)
					continue;
				SC_MOVE_PLAYER_PACKET send_packet;
				send_packet.size = sizeof(SC_MOVE_PLAYER_PACKET);
				send_packet.type = E_PACKET::E_PACKET_SC_MOVE_PLAYER;
				send_packet.id = rc.second->m_id;
				send_packet.x = rc_pos.x;
				send_packet.y = rc_pos.y;
				send_packet.z = rc_pos.z;
				rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
			}
		}

		// Camera Look Packet
		if (rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->is_Rotate) {
			XMFLOAT3 xmf3FinalLook = rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->GetLookVector();
			for (auto& rc_to : Clients) {
				if (!rc_to.second->b_Enable)
					continue;
				SC_LOOK_PLAYER_PACKET send_packet;
				send_packet.size = sizeof(SC_LOOK_PLAYER_PACKET);
				send_packet.type = E_PACKET::E_PACKET_SC_LOOK_PLAYER;
				send_packet.id = rc.second->m_id;
				send_packet.x = xmf3FinalLook.x;
				send_packet.y = xmf3FinalLook.y;
				send_packet.z = xmf3FinalLook.z;
				rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
			}
			rc.second->m_pPlayer->GetComponent<PlayerMovementComponent>()->is_Rotate = false;
		}
	}

	//Monster test
	for (auto& monster : scene->MonsterObjects) {
		if (monster->GetRemainHP() < 0.f)
			continue;

		if (((Character*)monster)->GetRemainHP() > 0.f) {
			//Monster Pos
			for (auto& rc_to : Clients) {
				if (!rc_to.second->b_Enable)
					continue;
				SC_MOVE_MONSTER_PACKET send_packet;
				send_packet.size = sizeof(SC_MOVE_MONSTER_PACKET);
				send_packet.type = E_PACKET::E_PACKET_SC_MOVE_MONSTER_PACKET;
				send_packet.id = ((Goblin*)monster)->num;
				send_packet.x = monster->GetPosition().x;
				send_packet.y = monster->GetPosition().y;
				send_packet.z = monster->GetPosition().z;
				rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
			}

			//WanderPosition
			for (auto& rc_to : Clients) {
				if (!rc_to.second->b_Enable)
					continue;
				SC_LOOK_MONSTER_PACKET send_packet;
				send_packet.size = sizeof(SC_LOOK_MONSTER_PACKET);
				send_packet.type = E_PACKET::E_PACKET_SC_LOOK_MONSTER_PACKET;
				send_packet.id = ((Goblin*)monster)->num;
				switch (monster->GetMonsterType())
				{
				case MONSTER_TYPE_CLOSE:
					send_packet.x = monster->GetComponent<CloseTypeFSMComponent>()->WanderPosition.x;
					send_packet.y = monster->GetComponent<CloseTypeFSMComponent>()->WanderPosition.y;
					send_packet.z = monster->GetComponent<CloseTypeFSMComponent>()->WanderPosition.z;
					break;
				case MONSTER_TYPE_FAR:
					send_packet.x = monster->GetComponent<FarTypeFSMComponent>()->WanderPosition.x;
					send_packet.y = monster->GetComponent<FarTypeFSMComponent>()->WanderPosition.y;
					send_packet.z = monster->GetComponent<FarTypeFSMComponent>()->WanderPosition.z;
					break;
				case MONSTER_TYPE_RUSH:
					send_packet.x = monster->GetComponent<RushTypeFSMComponent>()->WanderPosition.x;
					send_packet.y = monster->GetComponent<RushTypeFSMComponent>()->WanderPosition.y;
					send_packet.z = monster->GetComponent<RushTypeFSMComponent>()->WanderPosition.z;
					break;
				case MONSTER_TYPE_BOSS:
					break;
				default:
					break;
				}
				rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
			}

			//Monster Animation
			if (((Character*)monster)->OldAniType != ((Character*)monster)->PresentAniType) {
				for (auto& rc_to : Clients) {
					if (!rc_to.second->b_Enable)
						continue;
					SC_MONSTER_ANIMATION_TYPE_PACKET send_packet;
					send_packet.size = sizeof(SC_MONSTER_ANIMATION_TYPE_PACKET);
					send_packet.type = E_PACKET::E_PACKET_SC_ANIMATION_TYPE_MONSTER;
					send_packet.id = ((Goblin*)monster)->num;
					send_packet.Anitype = ((Character*)monster)->PresentAniType;
					rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
				}
				int expected = ((Character*)monster)->OldAniType.load();
				((Character*)monster)->OldAniType.compare_exchange_strong(expected, ((Character*)monster)->PresentAniType.load());
			}

			if (Clients.empty())
				continue;

			//Monster Targer
			switch (monster->GetMonsterType())
			{
			case MONSTER_TYPE_CLOSE:
				if (!monster->GetComponent<CloseTypeFSMComponent>()->GetTargetPlayer())
					break;
				for (auto& rc_to : Clients) {
					if (!rc_to.second->b_Enable)
						continue;
					SC_AGGRO_PLAYER_PACKET send_packet;
					send_packet.size = sizeof(SC_AGGRO_PLAYER_PACKET);
					send_packet.type = E_PACKET::E_PACKET_SC_AGGRO_PLAYER_PACKET;
					send_packet.player_id = ((Player*)((Goblin*)(monster)
						->GetComponent<CloseTypeFSMComponent>()->GetTargetPlayer()))->remoteClient->m_id;
					send_packet.monster_id = ((Goblin*)monster)->num;
					rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
				}
				break;
			case MONSTER_TYPE_FAR:
				if (!monster->GetComponent<FarTypeFSMComponent>()->GetTargetPlayer())
					break;
				for (auto& rc_to : Clients) {
					if (!rc_to.second->b_Enable)
						continue;
					SC_AGGRO_PLAYER_PACKET send_packet;
					send_packet.size = sizeof(SC_AGGRO_PLAYER_PACKET);
					send_packet.type = E_PACKET::E_PACKET_SC_AGGRO_PLAYER_PACKET;
					send_packet.player_id = ((Player*)((Goblin*)(monster)
						->GetComponent<FarTypeFSMComponent>()->GetTargetPlayer()))->remoteClient->m_id;
					send_packet.monster_id = ((Goblin*)monster)->num;
					rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
				}
				break;
			case MONSTER_TYPE_RUSH:
				if (!monster->GetComponent<RushTypeFSMComponent>()->GetTargetPlayer())
					break;
				for (auto& rc_to : Clients) {
					if (!rc_to.second->b_Enable)
						continue;
					SC_AGGRO_PLAYER_PACKET send_packet;
					send_packet.size = sizeof(SC_AGGRO_PLAYER_PACKET);
					send_packet.type = E_PACKET::E_PACKET_SC_AGGRO_PLAYER_PACKET;
					send_packet.player_id = ((Player*)((Goblin*)(monster)
						->GetComponent<RushTypeFSMComponent>()->GetTargetPlayer()))->remoteClient->m_id;
					send_packet.monster_id = ((Goblin*)monster)->num;
					rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
				}
				break;
			case MONSTER_TYPE_BOSS:

				for (auto& rc_to : Clients) {

				}
				break;
			default:
				break;
			}
			
		}
	}
}
