#include "CloseTypeState.h"
#include "Characters.h"
#include <random>

static std::uniform_real_distribution<float> RandomIdleTime(3.0, 8.0f);
static std::uniform_real_distribution<float> RandomPosDistance(-5.0f, 5.0f);
static std::random_device rd;
static std::default_random_engine dre(rd());


WanderState* WanderState::GetInstance()
{
	static WanderState state;
	return &state;
}

void WanderState::Enter(CloseTypeFSMComponent* pOwner)
{
	//std::cout << "Start Wandering" << std::endl;
	pOwner->Stop();
	XMFLOAT3 OwnerPos = pOwner->GetOwnerPosition();
	float NewPosx = OwnerPos.x + RandomPosDistance(dre);
	float NewPosz = OwnerPos.z + RandomPosDistance(dre);
	pOwner->ResetWanderPosition(NewPosx, NewPosz);
}

void WanderState::Execute(CloseTypeFSMComponent* pOwner)
{
	if (dynamic_cast<Character*>(pOwner->gameObject)->GetRemainHP() <= 0.0f)
	{
		pOwner->GetFSM()->ChangeState(DeathState::GetInstance());
	}

	if (pOwner->CheckDistanceFromPlayer())
	{
		pOwner->GetFSM()->ChangeState(TrackEnemyState::GetInstance());
	}
	if (pOwner->Wander())
	{
		pOwner->GetFSM()->ChangeState(IdleState::GetInstance());
	}
}

void WanderState::Exit(CloseTypeFSMComponent* pOwner)
{
	//std::cout << "Stop Wandering" << std::endl;
}

TrackEnemyState* TrackEnemyState::GetInstance()
{
	static TrackEnemyState state;
	return &state;
}

void TrackEnemyState::Enter(CloseTypeFSMComponent* pOwner)
{
	//std::cout << "Start Tracking" << std::endl;

}

void TrackEnemyState::Execute(CloseTypeFSMComponent* pOwner)
{
	if (dynamic_cast<Character*>(pOwner->gameObject)->GetRemainHP() <= 0.0f)
	{
		pOwner->GetFSM()->ChangeState(DeathState::GetInstance());
	}
	pOwner->Track();
	if (!pOwner->CheckDistanceFromPlayer())
	{
		pOwner->GetFSM()->ChangeState(IdleState::GetInstance());
	}
}

void TrackEnemyState::Exit(CloseTypeFSMComponent* pOwner)
{
	//std::cout << "Stop Tracking" << std::endl;
}

IdleState* IdleState::GetInstance()
{
	static IdleState state;
	return &state;
}

void IdleState::Enter(CloseTypeFSMComponent* pOwner)
{
	//std::cout << "Start Idle" << std::endl;
	pOwner->Stop();
	pOwner->ResetIdleTime(RandomIdleTime(rd));
}

void IdleState::Execute(CloseTypeFSMComponent* pOwner)
{
	if (dynamic_cast<Character*>(pOwner->gameObject)->GetRemainHP() <= 0.0f)
	{
		pOwner->GetFSM()->ChangeState(DeathState::GetInstance());
	}
	if (pOwner->CheckDistanceFromPlayer())
	{
		pOwner->GetFSM()->ChangeState(TrackEnemyState::GetInstance());
	}
	if (pOwner->Idle())
	{
		pOwner->GetFSM()->ChangeState(WanderState::GetInstance());
	}
}

void IdleState::Exit(CloseTypeFSMComponent* pOwner)
{
	//std::cout << "Stop Idel" << std::endl;
}

DeathState* DeathState::GetInstance()
{
	static DeathState state;
	return &state;
}

void DeathState::Enter(CloseTypeFSMComponent* pOwner)
{
	//std::cout << "Unit Die" << std::endl;
	pOwner->gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_M_DEATH);
}

void DeathState::Execute(CloseTypeFSMComponent* pOwner)
{
	pOwner->Death();
}

void DeathState::Exit(CloseTypeFSMComponent* pOwner)
{
}
