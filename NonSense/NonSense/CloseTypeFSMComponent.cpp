#include "CloseTypeFSMComponent.h"
#include "Characters.h"
#include "GameFramework.h"
#include "CloseTypeState.h"
#include "AttackComponent.h"
void CloseTypeFSMComponent::start()
{
	m_pFSM = new FSM<CloseTypeFSMComponent>(this);
	m_pFSM->SetCurrentState(IdleState::GetInstance());
}

void CloseTypeFSMComponent::update()
{
	m_pFSM->Update();
}

FSM<CloseTypeFSMComponent>* CloseTypeFSMComponent::GetFSM()
{
	return m_pFSM;
}

bool CloseTypeFSMComponent::CheckDistanceFromPlayer()
{
	XMFLOAT3 OwnerPos = gameObject->GetPosition();
	XMFLOAT3 PlayerPos = GameFramework::MainGameFramework->m_pPlayer->GetPosition();
	TargetPlayer = GameFramework::MainGameFramework->m_pPlayer;
	float Distance = Vector3::Length(Vector3::Subtract(OwnerPos, PlayerPos));
	if (Distance < ChangeStateDistance)
		return true;
	else
		return false;
}

void CloseTypeFSMComponent::ResetWanderPosition(float posx, float posz)
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
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(E_MONSTER_ANIMATION_TYPE::E_M_IDLE);
}

void CloseTypeFSMComponent::Move_Walk(float dist)
{
	gameObject->MoveForward(dist);
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(E_MONSTER_ANIMATION_TYPE::E_M_WALK);
}
void CloseTypeFSMComponent::Move_Run(float dist)
{
	gameObject->MoveForward(dist);
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(E_MONSTER_ANIMATION_TYPE::E_M_RUN);
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
	if (Distance > 0.5f)
	{
		Move_Walk(2.0f * Timer::GetTimeElapsed());
		return false;
	}
	return true;
}

void CloseTypeFSMComponent::Death()
{
	DeathCount -= Timer::GetTimeElapsed();
	if (DeathCount < 0.0f)
	{
		//GameScene::MainScene->PushDelete((Character*)gameObject);
		GameScene::MainScene->deletionMonsterQueue.push_back((Character*)gameObject);
	}
}
