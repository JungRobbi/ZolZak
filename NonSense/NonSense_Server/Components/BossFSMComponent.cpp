#include "BossFSMComponent.h"
#include "../Characters.h"
#include "../RemoteClients/RemoteClient.h"
#include "../BossState.h"
#include "BossAttackComponent.h"
#include "../Scene.h"
#include "../Room.h"
void BossFSMComponent::start()
{
	m_pFSM = new FSM<BossFSMComponent>(this);
	m_pFSM->SetCurrentState(IdleState_Boss::GetInstance());

}

void BossFSMComponent::update()
{
	if(m_pFSM)
		m_pFSM->Update();
}

FSM<BossFSMComponent>* BossFSMComponent::GetFSM()
{
	return m_pFSM;
}

bool BossFSMComponent::CheckDistanceFromPlayer()
{
	XMFLOAT3 OwnerPos = gameObject->GetPosition();

	float Distance = ChangeStateDistance;
	shared_ptr<Player> cand_player = nullptr;
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

void BossFSMComponent::ResetWanderPosition(float posx, float posz)
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

void BossFSMComponent::ResetIdleTime(float time)
{
	IdleLeftTime = time;
}

float BossFSMComponent::GetSkillCoolTime()
{
	return SkillCoolTime;
}

void BossFSMComponent::SetSkillCoolTime(float time)
{
	SkillCoolTime = time;
}

XMFLOAT3 BossFSMComponent::GetOwnerPosition()
{
	return gameObject->GetPosition();
}

bool BossFSMComponent::Idle()
{
	if (IdleLeftTime > 0.0f)
	{
		IdleLeftTime -= Timer::GetTimeElapsed();
		return false;
	}
	return true;
}

void BossFSMComponent::Stop()
{
	((Monster*)gameObject)->PresentAniType = E_BOSS_ANIMATION_TYPE::E_B_IDLE;
}

void BossFSMComponent::Move_Walk(float dist)
{
	((Monster*)gameObject)->PresentAniType = E_BOSS_ANIMATION_TYPE::E_B_WALK;
	gameObject->MoveForward(dist);
}
void BossFSMComponent::Move_Run(float dist)
{
	((Monster*)gameObject)->PresentAniType = E_BOSS_ANIMATION_TYPE::E_B_RUN;
	gameObject->MoveForward(dist);
}
void BossFSMComponent::Attack()
{
	if (!gameObject->GetComponent<BossAttackComponent>()->During_Attack)
		gameObject->GetComponent<BossAttackComponent>()->AttackAnimation();
}

void BossFSMComponent::Track()
{
	SkillCoolTime -= Timer::GetTimeElapsed();

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
		Move_Run(2.5f * Timer::GetTimeElapsed());
	else
	{
		Stop();
		Attack();
	}
}
bool BossFSMComponent::Wander()
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

void BossFSMComponent::TornadoTrack()
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
		gameObject->MoveForward(2.5f * Timer::GetTimeElapsed());
	else
	{
		Stop();
	}
}

void BossFSMComponent::StealSense()
{
	gameObject->GetComponent<BossAttackComponent>()->StealSenseAnimation();
	dynamic_cast<Shield*>(gameObject)->BossStealSenseEvent();
}

void BossFSMComponent::Summon()
{
	gameObject->GetComponent<BossAttackComponent>()->SummonAnimation();
	dynamic_cast<Shield*>(gameObject)->BossSummonEvent();
}

void BossFSMComponent::Defence()
{
	gameObject->GetComponent<BossAttackComponent>()->DefenceAnimation();
	dynamic_cast<Shield*>(gameObject)->BossDefenceEvent();
}

void BossFSMComponent::JumpAttack()
{
	gameObject->GetComponent<BossAttackComponent>()->JumpAttackAnimation();
	dynamic_cast<Shield*>(gameObject)->BossJumpAttackEvent();
}

void BossFSMComponent::Tornado()
{
	gameObject->GetComponent<BossAttackComponent>()->TornadoAnimation();
	dynamic_cast<Shield*>(gameObject)->BossTorandoEvent();
}
