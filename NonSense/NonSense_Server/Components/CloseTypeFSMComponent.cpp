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
	auto first = RemoteClient::remoteClients.begin();
	if ((first == RemoteClient::remoteClients.end()) || 
		RemoteClient::remoteClients.empty() ||
		first->second->m_id == 0)
		return false;
	XMFLOAT3 PlayerPos = first->second->m_pPlayer->GetPosition();
	TargetPlayer = first->second->m_pPlayer.get();
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
	((Character*)gameObject)->PresentAniType = E_MONSTER_ANIMATION_TYPE::E_M_IDLE;
}

void CloseTypeFSMComponent::Move_Walk(float dist)
{
	((Character*)gameObject)->PresentAniType = E_MONSTER_ANIMATION_TYPE::E_M_WALK;
	gameObject->MoveForward(dist);
}
void CloseTypeFSMComponent::Move_Run(float dist)
{
	((Character*)gameObject)->PresentAniType = E_MONSTER_ANIMATION_TYPE::E_M_RUN;
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
	
	for (auto& rc_to : RemoteClient::remoteClients) {
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