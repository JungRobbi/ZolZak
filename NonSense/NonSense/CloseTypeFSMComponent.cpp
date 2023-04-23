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
