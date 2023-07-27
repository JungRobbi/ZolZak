#include "FarTypeFSMComponent.h"
#include "MonsterAttackComponent.h"
#include "FarTypeState.h"
#include "CloseTypeFSMComponent.h"
#include "Characters.h"
#include "GameFramework.h"
#include "AttackComponent.h"
#include "SphereCollideComponent.h"
#include "MoveForwardComponent.h"
void FarTypeFSMComponent::start()
{
	m_pFSM = new FSM<FarTypeFSMComponent>(this);
	m_pFSM->SetCurrentState(IdleState_Far::GetInstance());

	SetTargetPlayer(GameFramework::MainGameFramework->m_pPlayer);
}

void FarTypeFSMComponent::update()
{
	m_pFSM->Update();
	if (((Monster*)gameObject)->WeaponFrame) {
		if (((Monster*)gameObject)->WeaponFrame->GetComponent<SphereCollideComponent>()->GetBoundingObject()->Intersects(*GameFramework::MainGameFramework->m_pPlayer->GetComponent<SphereCollideComponent>()->GetBoundingObject()) && ((Monster*)gameObject)->WeaponFrame->GetComponent<MoveForwardComponent>()->MoveTimeLeft > 0)
		{
			GameFramework::MainGameFramework->m_pPlayer->GetHit(dynamic_cast<Character*>(gameObject)->GetAttack() * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100)));
			if (GameFramework::MainGameFramework->GameSceneState == SIGHT_SCENE)
				GameFramework::MainGameFramework->m_pPlayer->Sight_DeBuff(5);
			printf("%f -> %f = %f\n", dynamic_cast<Character*>(gameObject)->GetAttack(), GameFramework::MainGameFramework->m_pPlayer->GetDefense(), GameFramework::MainGameFramework->m_pPlayer->GetRemainHP());
			((Monster*)gameObject)->WeaponFrame->GetComponent<MoveForwardComponent>()->MoveTimeLeft = 0;
		}
	}
}

FSM<FarTypeFSMComponent>* FarTypeFSMComponent::GetFSM()
{
	return m_pFSM;
}

bool FarTypeFSMComponent::CheckDistanceFromPlayer()
{
	XMFLOAT3 OwnerPos = gameObject->GetPosition();
	XMFLOAT3 PlayerPos = TargetPlayer->GetPosition();
	float Distance = Vector3::Length(Vector3::Subtract(OwnerPos, PlayerPos));
	if (Distance < ChangeStateDistance)
		return true;
	else
		return false;
}

void FarTypeFSMComponent::ResetWanderPosition(float posx, float posz)
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
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(Animation_type);
}

void FarTypeFSMComponent::Move_Walk(float dist)
{
	if (!NetworkMGR::b_isNet) {
		gameObject->MoveForward(dist);
	}
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(Animation_type);
}
void FarTypeFSMComponent::Move_Run(float dist)
{
	if (!NetworkMGR::b_isNet) {
		gameObject->MoveForward(dist);
	}
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(Animation_type);
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

void FarTypeFSMComponent::Death()
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
