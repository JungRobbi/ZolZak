#include "RushTypeFSMComponent.h"
#include <algorithm>
#include <memory>
#include "../RemoteClients/RemoteClient.h"
#include "SphereCollideComponent.h"
#include "MoveForwardComponent.h"
#include "../RushTypeState.h"
#include "CloseTypeFSMComponent.h"
#include "../Characters.h"
#include "../Scene.h"
#include "AttackComponent.h"
#include "MonsterAttackComponent.h"
void RushTypeFSMComponent::start()
{
	m_pFSM = new FSM<RushTypeFSMComponent>(this);
	m_pFSM->SetCurrentState(IdleState_Rush::GetInstance());

//	SetTargetPlayer(GameFramework::MainGameFramework->m_pPlayer);
}

void RushTypeFSMComponent::update()
{
	if (m_pFSM)
		m_pFSM->Update();
}

FSM<RushTypeFSMComponent>* RushTypeFSMComponent::GetFSM()
{
	return m_pFSM;
}

bool RushTypeFSMComponent::CheckDistanceFromPlayer()
{
	XMFLOAT3 OwnerPos = gameObject->GetPosition();

	float Distance = ChangeStateDistance;
	std::shared_ptr<Player> cand_player = nullptr;
	for (auto& rc : Room::roomlist.at(gameObject->m_roomNum)->Clients) {
		if (!rc.second->b_Enable || !rc.second->m_pPlayer->alive)
			continue;
		auto PlayerPos = rc.second->m_pPlayer->GetPosition();
		float cand_length = Vector3::Length(Vector3::Subtract(PlayerPos, OwnerPos));
		if (Distance > cand_length) {
			Distance = cand_length;
			cand_player = rc.second->m_pPlayer;
		}
	}
	TargetPlayer = cand_player.get();

	if (Distance >= ChangeStateDistance ||
		Room::roomlist.at(gameObject->m_roomNum)->Clients.empty() ||
		cand_player->remoteClient->m_id == 0 ||
		!cand_player)
		return false;

	return true;
}

void RushTypeFSMComponent::ResetWanderPosition(float posx, float posz)
{
	XMFLOAT3 pos;
	if (Scene::terrain)
	{
		float width = Scene::terrain->GetWidth();
		float length = Scene::terrain->GetLength();
		float height = Scene::terrain->GetHeight(posx + (width / 2), posz + (length / 2));

		pos = { posx, height, posz };
	}
	else
	{
		pos = { posx,0.0f,posz };
	}
	WanderPosition = pos;
}

void RushTypeFSMComponent::ResetIdleTime(float time)
{
	IdleLeftTime = time;
}

XMFLOAT3 RushTypeFSMComponent::GetOwnerPosition()
{
	return gameObject->GetPosition();
}

bool RushTypeFSMComponent::Idle()
{
	if (IdleLeftTime > 0.0f)
	{
		IdleLeftTime -= Timer::GetTimeElapsed();
		return false;
	}
	return true;
}

void RushTypeFSMComponent::Stop()
{
	((Monster*)gameObject)->PresentAniType = E_MONSTER_ANIMATION_TYPE::E_M_IDLE;
}

void RushTypeFSMComponent::Move_Walk(float dist)
{
	((Monster*)gameObject)->PresentAniType = E_MONSTER_ANIMATION_TYPE::E_M_WALK;
	XMFLOAT3 xmf3Position = gameObject->GetPosition();
	XMFLOAT3 xmf3Look = gameObject->GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, dist);
	for (auto& client : Room::roomlist[gameObject->m_roomNum]->Clients) {
		auto cc = client.second->m_pPlayer->GetComponent<SphereCollideComponent>();
		if (dynamic_cast<Monster*>(gameObject)->num > 22000) { // Shield
			if (1.0f > Vector3::Length(Vector3::Subtract(xmf3Position, cc->GetBoundingObject()->Center))) {
				return;
			}
		}
		else if (dynamic_cast<Monster*>(gameObject)->num > 11000) { // Goblin
			if (1.3f > Vector3::Length(Vector3::Subtract(xmf3Position, cc->GetBoundingObject()->Center))) {
				return;
			}
		}
		else if (dynamic_cast<Monster*>(gameObject)->num > 10200) { // Skull
			if (1.3f > Vector3::Length(Vector3::Subtract(xmf3Position, cc->GetBoundingObject()->Center))) {
				return;
			}
		}
		else if (dynamic_cast<Monster*>(gameObject)->num > 10100) { // Orc
			if (1.3f > Vector3::Length(Vector3::Subtract(xmf3Position, cc->GetBoundingObject()->Center))) {
				return;
			}
		}
	}
	gameObject->MoveForward(dist);
}
void RushTypeFSMComponent::Move_Run(float dist)
{
	((Monster*)gameObject)->PresentAniType = E_MONSTER_ANIMATION_TYPE::E_M_RUN;
	XMFLOAT3 xmf3Position = gameObject->GetPosition();
	XMFLOAT3 xmf3Look = gameObject->GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, dist);
	for (auto& client : Room::roomlist[gameObject->m_roomNum]->Clients) {
		auto cc = client.second->m_pPlayer->GetComponent<SphereCollideComponent>();
		if (dynamic_cast<Monster*>(gameObject)->num > 22000) { // Shield
			if (1.0f > Vector3::Length(Vector3::Subtract(xmf3Position, cc->GetBoundingObject()->Center))) {
				return;
			}
		}
		else if (dynamic_cast<Monster*>(gameObject)->num > 11000) { // Goblin
			if (1.3f > Vector3::Length(Vector3::Subtract(xmf3Position, cc->GetBoundingObject()->Center))) {
				return;
			}
		}
		else if (dynamic_cast<Monster*>(gameObject)->num > 10200) { // Skull
			if (1.3f > Vector3::Length(Vector3::Subtract(xmf3Position, cc->GetBoundingObject()->Center))) {
				return;
			}
		}
		else if (dynamic_cast<Monster*>(gameObject)->num > 10100) { // Orc
			if (1.3f > Vector3::Length(Vector3::Subtract(xmf3Position, cc->GetBoundingObject()->Center))) {
				return;
			}
		}
	}
	gameObject->MoveForward(dist);
}
void RushTypeFSMComponent::Attack()
{
	if (!gameObject->GetComponent<MonsterAttackComponent>()->During_Attack)
	{
		if (!gameObject->GetComponent<MonsterAttackComponent>()->Targetting)
		{
			gameObject->GetComponent<MonsterAttackComponent>()->TargetOn();
		}
		else
		{
			gameObject->GetComponent<MonsterAttackComponent>()->RushTypeAttack();
		}
	}
}

void RushTypeFSMComponent::Track()
{
	XMFLOAT3 TargetPos = TargetPlayer->GetPosition();
	XMFLOAT3 CurrentPos = gameObject->GetPosition();
	XMFLOAT3 Direction = Vector3::Normalize(Vector3::Subtract(TargetPos, CurrentPos));
	XMFLOAT3 Look = gameObject->GetLook();
	XMFLOAT3 CrossProduct = Vector3::CrossProduct(Look, Direction);
	float Dot = Vector3::DotProduct(Look, Direction);
	float ToTargetAngle = XMConvertToDegrees(acos(Dot));
	float Angle = (CrossProduct.y > 0.0f) ? 180.0f : -180.0f;
	if (ToTargetAngle > 7.0f)
		gameObject->Rotate(0.0f, Angle * Timer::GetTimeElapsed(), 0.0f);
	float Distance = Vector3::Length(Vector3::Subtract(TargetPos, CurrentPos));
	if (Distance > 3.0f)
	{
		if (gameObject->GetComponent<MonsterAttackComponent>()->AttackTimeLeft < 1.5f)
			Move_Run(2.0f * Timer::GetTimeElapsed());
	}
	else
	{
		Stop();
		Attack();
	}
}
bool RushTypeFSMComponent::Wander()
{
	XMFLOAT3 CurrentPos = gameObject->GetPosition();
	XMFLOAT3 Direction = Vector3::Normalize(Vector3::Subtract(WanderPosition, CurrentPos));
	XMFLOAT3 Look = gameObject->GetLook();
	XMFLOAT3 CrossProduct = Vector3::CrossProduct(Look, Direction);
	float Dot = Vector3::DotProduct(Look, Direction);
	float ToTargetAngle = XMConvertToDegrees(acos(Dot));
	float Angle = (CrossProduct.y > 0.0f) ? 180.0f : -180.0f;
	if (ToTargetAngle > 7.0f)
		gameObject->Rotate(0.0f, Angle * Timer::GetTimeElapsed(), 0.0f);
	float Distance = Vector3::Length(Vector3::Subtract(WanderPosition, CurrentPos));

	for (auto& rc_to : Room::roomlist.at(gameObject->m_roomNum)->Clients) {
		if (!rc_to.second->b_Enable)
			continue;
		SC_TEMP_WANDER_MONSTER_PACKET send_packet;
		send_packet.size = sizeof(SC_TEMP_WANDER_MONSTER_PACKET);
		send_packet.type = E_PACKET::E_PACKET_SC_TEMP_WANDER_MONSTER_PACKET;
		send_packet.id = ((Goblin*)gameObject)->num;
		rc_to.second->tcpConnection.SendOverlapped(reinterpret_cast<char*>(&send_packet));
	}

	if (Distance > 0.5f)
	{
		Move_Walk(2.0f * Timer::GetTimeElapsed());
		return false;
	}
	return true;
}
