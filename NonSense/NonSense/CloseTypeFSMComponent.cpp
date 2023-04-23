#include "CloseTypeFSMComponent.h"
#include "Characters.h"
#include "GameFramework.h"
#include "CloseTypeState.h"
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
	float Distance = Vector3::Length(Vector3::Subtract(OwnerPos, PlayerPos));
	if (Distance < ChangeStateDistance)
		return true;
	else 
		return false;
}

void CloseTypeFSMComponent::Move_Walk()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(1);
}
void CloseTypeFSMComponent::Move_Run()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(2);
}
void CloseTypeFSMComponent::Attack()
{
	gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 5);
	gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 5);
	gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 5);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
}

void CloseTypeFSMComponent::Track()
{
	Move_Run();
}

void CloseTypeFSMComponent::Wander()
{
	Move_Walk();
}
