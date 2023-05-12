#include "RushTypeState.h"
#include "Characters.h"
#include <random>

static std::uniform_real_distribution<float> RandomIdleTime(3.0, 8.0f);
static std::uniform_real_distribution<float> RandomPosDistance(-5.0f, 5.0f);
static std::random_device rd;
static std::default_random_engine dre(rd());


WanderState_Rush* WanderState_Rush::GetInstance()
{
	static WanderState_Rush state;
	return &state;
}

void WanderState_Rush::Enter(RushTypeFSMComponent* pOwner)
{
	std::cout << "Start Wandering" << std::endl;
	pOwner->Stop();
	XMFLOAT3 OwnerPos = pOwner->GetOwnerPosition();
	float NewPosx = OwnerPos.x + RandomPosDistance(dre);
	float NewPosz = OwnerPos.z + RandomPosDistance(dre);
	pOwner->ResetWanderPosition(NewPosx, NewPosz);
}

void WanderState_Rush::Execute(RushTypeFSMComponent* pOwner)
{
	if (dynamic_cast<Character*>(pOwner->gameObject)->GetRemainHP() <= 0.0f)
	{
		pOwner->GetFSM()->ChangeState(DeathState_Rush::GetInstance());
	}

	if (pOwner->CheckDistanceFromPlayer())
	{
		pOwner->GetFSM()->ChangeState(TrackEnemyState_Rush::GetInstance());
	}
	if (pOwner->Wander())
	{
		pOwner->GetFSM()->ChangeState(IdleState_Rush::GetInstance());
	}
}

void WanderState_Rush::Exit(RushTypeFSMComponent* pOwner)
{
	std::cout << "Stop Wandering" << std::endl;
}

TrackEnemyState_Rush* TrackEnemyState_Rush::GetInstance()
{
	static TrackEnemyState_Rush state;
	return &state;
}

void TrackEnemyState_Rush::Enter(RushTypeFSMComponent* pOwner)
{
	std::cout << "Start Tracking" << std::endl;

}

void TrackEnemyState_Rush::Execute(RushTypeFSMComponent* pOwner)
{
	if (dynamic_cast<Character*>(pOwner->gameObject)->GetRemainHP() <= 0.0f)
	{
		pOwner->GetFSM()->ChangeState(DeathState_Rush::GetInstance());
	}
	pOwner->Track();
	if (!pOwner->CheckDistanceFromPlayer())
	{
		pOwner->GetFSM()->ChangeState(IdleState_Rush::GetInstance());
	}
}

void TrackEnemyState_Rush::Exit(RushTypeFSMComponent* pOwner)
{
	std::cout << "Stop Tracking" << std::endl;
}

IdleState_Rush* IdleState_Rush::GetInstance()
{
	static IdleState_Rush state;
	return &state;
}

void IdleState_Rush::Enter(RushTypeFSMComponent* pOwner)
{
	std::cout << "Start Idle" << std::endl;
	pOwner->Stop();
	pOwner->ResetIdleTime(RandomIdleTime(rd));
}

void IdleState_Rush::Execute(RushTypeFSMComponent* pOwner)
{
	if (dynamic_cast<Character*>(pOwner->gameObject)->GetRemainHP() <= 0.0f)
	{
		pOwner->GetFSM()->ChangeState(DeathState_Rush::GetInstance());
	}
	if (pOwner->CheckDistanceFromPlayer())
	{
		pOwner->GetFSM()->ChangeState(TrackEnemyState_Rush::GetInstance());
	}
	if (pOwner->Idle())
	{
		pOwner->GetFSM()->ChangeState(WanderState_Rush::GetInstance());
	}
}

void IdleState_Rush::Exit(RushTypeFSMComponent* pOwner)
{
	std::cout << "Stop Idel" << std::endl;
}

DeathState_Rush* DeathState_Rush::GetInstance()
{
	static DeathState_Rush state;
	return &state;
}

void DeathState_Rush::Enter(RushTypeFSMComponent* pOwner)
{
	std::cout << "Unit Die" << std::endl;
	pOwner->gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_M_DEATH);
}

void DeathState_Rush::Execute(RushTypeFSMComponent* pOwner)
{
	pOwner->Death();
}

void DeathState_Rush::Exit(RushTypeFSMComponent* pOwner)
{
}
