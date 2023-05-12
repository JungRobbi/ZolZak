#include "RushTypeFSMComponent.h"
#include "MonsterAttackComponent.h"
#include "RushTypeState.h"
#include "CloseTypeFSMComponent.h"
#include "Characters.h"
#include "GameFramework.h"
#include "AttackComponent.h"
void RushTypeFSMComponent::start()
{
	m_pFSM = new FSM<RushTypeFSMComponent>(this);
	m_pFSM->SetCurrentState(IdleState_Rush::GetInstance());

	SetTargetPlayer(GameFramework::MainGameFramework->m_pPlayer);
}

void RushTypeFSMComponent::update()
{
	m_pFSM->Update();
}

FSM<RushTypeFSMComponent>* RushTypeFSMComponent::GetFSM()
{
	return m_pFSM;
}

bool RushTypeFSMComponent::CheckDistanceFromPlayer()
{
	XMFLOAT3 OwnerPos = gameObject->GetPosition();
	XMFLOAT3 PlayerPos = TargetPlayer->GetPosition();
	float Distance = Vector3::Length(Vector3::Subtract(OwnerPos, PlayerPos));
	if (Distance < ChangeStateDistance)
		return true;
	else
		return false;
}

void RushTypeFSMComponent::ResetWanderPosition(float posx, float posz)
{
	XMFLOAT3 pos;
	if (GameScene::MainScene->GetTerrain())
	{
		float width = GameScene::MainScene->GetTerrain()->GetWidth();
		float length = GameScene::MainScene->GetTerrain()->GetLength();
		float height = GameScene::MainScene->GetTerrain()->GetHeight(posx + (width / 2), posz + (length / 2));

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
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(Animation_type);
}

void RushTypeFSMComponent::Move_Walk(float dist)
{
	if (!NetworkMGR::b_isNet) {
		gameObject->MoveForward(dist);
	}
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(Animation_type);
}
void RushTypeFSMComponent::Move_Run(float dist)
{
	if (!NetworkMGR::b_isNet) {
		gameObject->MoveForward(dist);
	}
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(Animation_type);
}
void RushTypeFSMComponent::Attack()
{
	if (!gameObject->GetComponent<MonsterAttackComponent>()->During_Attack)
		gameObject->GetComponent<MonsterAttackComponent>()->FarTypeAttack();
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
	if (Distance > 5.0f)
		Move_Run(2.0f * Timer::GetTimeElapsed());
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

	if (Distance > 0.5f)
	{
		Move_Walk(2.0f * Timer::GetTimeElapsed());
		return false;
	}
	return true;
}

void RushTypeFSMComponent::Death()
{
	DeathCount -= Timer::GetTimeElapsed();
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	if (DeathCount < 0.0f)
	{
		//GameScene::MainScene->PushDelete((Character*)gameObject);
		GameScene::MainScene->deletionMonsterQueue.push_back((Monster*)gameObject);
		gameObject->m_pSkinnedAnimationController->SetTrackEnable(0, false);
	}
}
