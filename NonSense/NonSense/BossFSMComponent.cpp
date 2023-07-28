#include "BossFSMComponent.h"
#include "Characters.h"
#include "GameFramework.h"
#include "BossState.h"
#include "BossAttackComponent.h"
void BossFSMComponent::start()
{
	m_pFSM = new FSM<BossFSMComponent>(this);
	m_pFSM->SetCurrentState(IdleState_Boss::GetInstance());

	SetTargetPlayer(GameFramework::MainGameFramework->m_pPlayer);
}

void BossFSMComponent::update()
{
	m_pFSM->Update();
}

FSM<BossFSMComponent>* BossFSMComponent::GetFSM()
{
	return m_pFSM;
}

bool BossFSMComponent::CheckDistanceFromPlayer()
{
	XMFLOAT3 OwnerPos = gameObject->GetPosition();
	XMFLOAT3 PlayerPos = TargetPlayer->GetPosition();
	float Distance = Vector3::Length(Vector3::Subtract(OwnerPos, PlayerPos));
	if (Distance < ChangeStateDistance)
		return true;
	else
		return false;
}

void BossFSMComponent::ResetWanderPosition(float posx, float posz)
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
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(Animation_type);
}

void BossFSMComponent::Move_Walk(float dist)
{
	if (!NetworkMGR::b_isNet) {
		gameObject->MoveForward(dist);
	}
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(Animation_type);
}
void BossFSMComponent::Move_Run(float dist)
{
	if (!NetworkMGR::b_isNet) {
		gameObject->MoveForward(dist);
	}
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(Animation_type);
}
void BossFSMComponent::Attack()
{
	if (!gameObject->GetComponent<BossAttackComponent>()->During_Attack 
		&& !gameObject->GetComponent<BossAttackComponent>()->During_Skill)
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
	if (Distance > 4.0f)
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

	if (Distance > 0.5f)
	{
		Move_Walk(2.0f * Timer::GetTimeElapsed());
		return false;
	}
	return true;
}

void BossFSMComponent::Death()
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
}

void BossFSMComponent::Summon()
{
	gameObject->GetComponent<BossAttackComponent>()->SummonAnimation();
}

void BossFSMComponent::Defence()
{
	gameObject->GetComponent<BossAttackComponent>()->DefenceAnimation();
}

void BossFSMComponent::JumpAttack()
{
	gameObject->GetComponent<BossAttackComponent>()->JumpAttackAnimation();
}

void BossFSMComponent::Tornado()
{
	gameObject->GetComponent<BossAttackComponent>()->TornadoAnimation();
}
