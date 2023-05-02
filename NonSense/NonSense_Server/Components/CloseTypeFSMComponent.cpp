#include "CloseTypeFSMComponent.h"
#include "../Characters.h"
#include "../RemoteClients/RemoteClient.h"
#include "../Scene.h"
#include "../CloseTypeState.h"
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
	auto first = RemoteClient::remoteClients.begin()->second;
	XMFLOAT3 PlayerPos = first->m_pPlayer->GetPosition();
	TargetPlayer = first->m_pPlayer.get();
	float Distance = Vector3::Length(Vector3::Subtract(OwnerPos, PlayerPos));
	if (Distance < ChangeStateDistance)
		return true;
	else
		return false;
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
	dynamic_cast<Character*>(gameObject)->SetAniType(E_MONSTER_ANIMATION_TYPE::E_M_IDLE);
}

void CloseTypeFSMComponent::Move_Walk(float dist)
{
	gameObject->MoveForward(dist);
	dynamic_cast<Character*>(gameObject)->SetAniType(E_MONSTER_ANIMATION_TYPE::E_M_WALK);
}
void CloseTypeFSMComponent::Move_Run(float dist)
{
	gameObject->MoveForward(dist);
	dynamic_cast<Character*>(gameObject)->SetAniType(E_MONSTER_ANIMATION_TYPE::E_M_RUN);
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