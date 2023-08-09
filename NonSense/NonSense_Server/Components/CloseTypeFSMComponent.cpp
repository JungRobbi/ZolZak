#include <algorithm>
#include "CloseTypeFSMComponent.h"
#include "../Characters.h"
#include "../RemoteClients/RemoteClient.h"
#include "../Scene.h"
#include "../CloseTypeState.h"
#include "AttackComponent.h"
#include "SphereCollideComponent.h"
void CloseTypeFSMComponent::start()
{
	m_pFSM = new FSM<CloseTypeFSMComponent>(this);
	m_pFSM->SetCurrentState(IdleState::GetInstance());
}

void CloseTypeFSMComponent::update()
{
	if (m_pFSM)
		m_pFSM->Update();
}

FSM<CloseTypeFSMComponent>* CloseTypeFSMComponent::GetFSM()
{
	return m_pFSM;
}

bool CloseTypeFSMComponent::CheckDistanceFromPlayer()
{
	XMFLOAT3 OwnerPos = gameObject->GetPosition();

	float Distance = ChangeStateDistance;
	shared_ptr<Player> cand_player = nullptr;
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

void CloseTypeFSMComponent::ResetWanderPosition(float posx, float posz)
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

void CloseTypeFSMComponent::ResetIdleTime(float time)
{
	IdleLeftTime = time;
}

XMFLOAT3 CloseTypeFSMComponent::GetOwnerPosition()
{
	return gameObject->GetPosition();
}

bool CloseTypeFSMComponent::Idle()
{
	if (IdleLeftTime > 0.0f)
	{
		IdleLeftTime -= Timer::GetTimeElapsed();
		return false;
	}
	return true;
}

void CloseTypeFSMComponent::Stop()
{
	((Monster*)gameObject)->PresentAniType = E_MONSTER_ANIMATION_TYPE::E_M_IDLE;
}

void CloseTypeFSMComponent::Move_Walk(float dist)
{
	((Monster*)gameObject)->PresentAniType = E_MONSTER_ANIMATION_TYPE::E_M_WALK;
	XMFLOAT3 xmf3Position = gameObject->GetPosition();
	XMFLOAT3 xmf3Look = gameObject->GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, dist);
	for (auto& client : Room::roomlist[gameObject->m_roomNum]->Clients) {
		auto cc = client.second->m_pPlayer->GetComponent<SphereCollideComponent>();
		if (dynamic_cast<Monster*>(gameObject)->num > 22000) { // Shield
			if (2.2f > Vector3::Length(Vector3::Subtract(xmf3Position, cc->GetBoundingObject()->Center))) {
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
void CloseTypeFSMComponent::Move_Run(float dist)
{
	((Monster*)gameObject)->PresentAniType = E_MONSTER_ANIMATION_TYPE::E_M_RUN;
	XMFLOAT3 xmf3Position = gameObject->GetPosition();
	XMFLOAT3 xmf3Look = gameObject->GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, dist);
	for (auto& client : Room::roomlist[gameObject->m_roomNum]->Clients) {
		auto cc = client.second->m_pPlayer->GetComponent<SphereCollideComponent>();
		if (dynamic_cast<Monster*>(gameObject)->num > 22000) { // Shield
			if (2.2f > Vector3::Length(Vector3::Subtract(xmf3Position, cc->GetBoundingObject()->Center))) {
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
void CloseTypeFSMComponent::Attack()
{
	if (!gameObject->GetComponent<AttackComponent>()->During_Attack)
		gameObject->GetComponent<AttackComponent>()->Attack();
}

void CloseTypeFSMComponent::Track()
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
	if (Distance > 1.5f)
		Move_Run(2.0f * Timer::GetTimeElapsed());
	else
	{
		Stop();
		Attack();
	}
}
bool CloseTypeFSMComponent::Wander()
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
