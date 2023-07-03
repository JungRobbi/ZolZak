#include "FarTypeFSMComponent.h"
#include <algorithm>
#include <memory>
#include "../RemoteClients/RemoteClient.h"
#include "AttackComponent.h"
#include "MonsterAttackComponent.h"
#include "../FarTypeState.h"
#include "../Characters.h"
#include "../Scene.h"
#include "AttackComponent.h"
#include "SphereCollideComponent.h"
#include "MoveForwardComponent.h"
void FarTypeFSMComponent::start()
{
	m_pFSM = new FSM<FarTypeFSMComponent>(this);
	m_pFSM->SetCurrentState(IdleState_Far::GetInstance());

//	SetTargetPlayer(GameFramework::MainGameFramework->m_pPlayer);
}

void FarTypeFSMComponent::update()
{
	m_pFSM->Update();
	/*if (((Monster*)gameObject)->WeaponFrame) {
		if (((Monster*)gameObject)->WeaponFrame->GetComponent<SphereCollideComponent>()->GetBoundingObject()->Intersects(*GameFramework::MainGameFramework->m_pPlayer->GetComponent<SphereCollideComponent>()->GetBoundingObject()) && ((Monster*)gameObject)->WeaponFrame->GetComponent<MoveForwardComponent>()->MoveTimeLeft > 0)
		{
			GameFramework::MainGameFramework->m_pPlayer->GetHit(dynamic_cast<Goblin*>(gameObject)->GetAttack() * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100)));
			GameFramework::MainGameFramework->m_pPlayer->Sight_DeBuff(5);
			printf("%f -> %f = %f\n", dynamic_cast<Goblin*>(gameObject)->GetAttack(), GameFramework::MainGameFramework->m_pPlayer->GetDefense(), GameFramework::MainGameFramework->m_pPlayer->GetRemainHP());
			((Monster*)gameObject)->WeaponFrame->GetComponent<MoveForwardComponent>()->MoveTimeLeft = 0;
		}
	}*/
}

FSM<FarTypeFSMComponent>* FarTypeFSMComponent::GetFSM()
{
	return m_pFSM;
}

bool FarTypeFSMComponent::CheckDistanceFromPlayer()
{
	XMFLOAT3 OwnerPos = gameObject->GetPosition();
	float Distance = ChangeStateDistance;
	std::shared_ptr<Player> cand_player = nullptr;
	for (auto& rc : Room::roomlist.at(gameObject->m_roomNum)->Clients) {
		if (!rc.second->b_Enable)
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

void FarTypeFSMComponent::ResetWanderPosition(float posx, float posz)
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

void FarTypeFSMComponent::ResetIdleTime(float time)
{
	IdleLeftTime = time;
}

XMFLOAT3 FarTypeFSMComponent::GetOwnerPosition()
{
	return gameObject->GetPosition();
}

bool FarTypeFSMComponent::Idle()
{
	if (IdleLeftTime > 0.0f)
	{
		IdleLeftTime -= Timer::GetTimeElapsed();
		return false;
	}
	return true;
}

void FarTypeFSMComponent::Stop()
{
	((Monster*)gameObject)->PresentAniType = E_MONSTER_ANIMATION_TYPE::E_M_IDLE;
}

void FarTypeFSMComponent::Move_Walk(float dist)
{
	((Monster*)gameObject)->PresentAniType = E_MONSTER_ANIMATION_TYPE::E_M_WALK;
}
void FarTypeFSMComponent::Move_Run(float dist)
{
	((Monster*)gameObject)->PresentAniType = E_MONSTER_ANIMATION_TYPE::E_M_RUN;
}
void FarTypeFSMComponent::Attack()
{
	if (!gameObject->GetComponent<MonsterAttackComponent>()->During_Attack)
		gameObject->GetComponent<MonsterAttackComponent>()->FarTypeAttack();
}

void FarTypeFSMComponent::Track()
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
	if (Distance > 5.0f)
		Move_Run(2.0f * Timer::GetTimeElapsed());
	else
	{
		Stop();
		Attack();
	}
}
bool FarTypeFSMComponent::Wander()
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

	if (Distance > 0.5f)
	{
		Move_Walk(2.0f * Timer::GetTimeElapsed());
		return false;
	}
	return true;
}
