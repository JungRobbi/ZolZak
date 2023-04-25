#include "CloseTypeFSMComponent.h"
#include "GameFramework.h"
#include "CloseTypeState.h"
#include "SphereCollideComponent.h"

void CloseTypeFSMComponent::start()
{
	m_pFSM = new FSM<CloseTypeFSMComponent>(this);
	m_pFSM->SetCurrentState(WanderState::GetInstance());
}

void CloseTypeFSMComponent::update()
{
	if (AttackTimeLeft > 0.0f)
	{
		if (AttackTimeLeft > 0.0)
		{
			AttackTimeLeft -= Timer::GetTimeElapsed();
		}

		if (AttackTimeLeft < NextAttackInputTime)
		{
			During_Attack = false;
		}
	}
	AttackRangeupdate();
	m_pFSM->Update();
}
void CloseTypeFSMComponent::Attack()
{
	AttackTimeLeft = AttackDuration + NextAttackInputTime;
	During_Attack = true;
	if (AttackRange) {
		if (AttackRange->Intersects(*GameFramework::MainGameFramework->m_pPlayer->GetComponent<SphereCollideComponent>()->GetBoundingObject()))
		{
			printf("공격");
			GameFramework::MainGameFramework->m_pPlayer->GetHit(100);
		};
	}
	gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 4);
	gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 4);
	gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 4);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
}

FSM<CloseTypeFSMComponent>* CloseTypeFSMComponent::GetFSM()
{
	return m_pFSM;
}
void CloseTypeFSMComponent::AttackRangeupdate()
{
	if (AttackRange)
	{
		AttackRange->Center = XMFLOAT3(0, 0.3, 1.0);
		AttackRange->Extents = XMFLOAT3(0.3, 0.3, 0.5);
		AttackRange->Orientation = XMFLOAT4(0, 0, 0, 1);

		AttackRange->Transform(*AttackRange, XMLoadFloat4x4(&gameObject->GetWorld()));

		/// 그리기 위한 코드
		AttackRange->m_xmf4x4ToParent = gameObject->GetWorld();
		AttackRange->SetScale(AttackRange->Extents.x, AttackRange->Extents.y, AttackRange->Extents.z);
		AttackRange->SetPosition(AttackRange->Center.x, AttackRange->Center.y, AttackRange->Center.z);
	}
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
		float height = GameScene::MainScene->GetTerrain()->GetHeight(posx + (width/2), posz +(length/2));

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
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(0);
}

void CloseTypeFSMComponent::Move_Walk(float dist)
{
	gameObject->MoveForward(dist);
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(1);
}
void CloseTypeFSMComponent::Move_Run(float dist)
{
	gameObject->MoveForward(dist);
	gameObject->m_pSkinnedAnimationController->ChangeAnimationUseBlending(2);
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
		if (!During_Attack)
		{
			Attack();
		}
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
	if (Distance > 0.1f)
	{
		Move_Walk(0.5f * Timer::GetTimeElapsed());
		return false;
	}
	return true;

}
