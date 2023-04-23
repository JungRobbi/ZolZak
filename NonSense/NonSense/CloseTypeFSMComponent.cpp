#include "CloseTypeFSMComponent.h"
#include "Characters.h"
#include "GameFramework.h"
#include "CloseTypeState.h"
#include "AttackComponent.h"
void CloseTypeFSMComponent::start()
{
	m_pFSM = new FSM<CloseTypeFSMComponent>(this);
	m_pFSM->SetCurrentState(WanderState::GetInstance());
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

void CloseTypeFSMComponent::Stop()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(0);
}

void CloseTypeFSMComponent::Move_Walk(float dist)
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(1);
}
void CloseTypeFSMComponent::Move_Run(float dist)
{
	gameObject->MoveForward(dist);
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(2);
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
void CloseTypeFSMComponent::Wander()
{

}
