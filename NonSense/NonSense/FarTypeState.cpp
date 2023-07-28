#include "FarTypeState.h"
#include "Characters.h"
#include <random>

static std::uniform_real_distribution<float> RandomIdleTime(3.0, 8.0f);
static std::uniform_real_distribution<float> RandomPosDistance(-5.0f, 5.0f);
static std::random_device rd;
static std::default_random_engine dre(rd());


WanderState_Far* WanderState_Far::GetInstance()
{
	static WanderState_Far state;
	return &state;
}

void WanderState_Far::Enter(FarTypeFSMComponent* pOwner)
{
	//std::cout << "Start Wandering" << std::endl;
	pOwner->Stop();
	XMFLOAT3 OwnerPos = pOwner->GetOwnerPosition();
	float NewPosx = OwnerPos.x + RandomPosDistance(dre);
	float NewPosz = OwnerPos.z + RandomPosDistance(dre);
	pOwner->ResetWanderPosition(NewPosx, NewPosz);
}

void WanderState_Far::Execute(FarTypeFSMComponent* pOwner)
{
	if (dynamic_cast<Character*>(pOwner->gameObject)->GetRemainHP() <= 0.0f)
	{
		pOwner->GetFSM()->ChangeState(DeathState_Far::GetInstance());
	}

	if (pOwner->CheckDistanceFromPlayer())
	{
		pOwner->GetFSM()->ChangeState(TrackEnemyState_Far::GetInstance());
	}
	if (pOwner->Wander())
	{
		pOwner->GetFSM()->ChangeState(IdleState_Far::GetInstance());
	}
}

void WanderState_Far::Exit(FarTypeFSMComponent* pOwner)
{
	//std::cout << "Stop Wandering" << std::endl;
}

TrackEnemyState_Far* TrackEnemyState_Far::GetInstance()
{
	static TrackEnemyState_Far state;
	return &state;
}

void TrackEnemyState_Far::Enter(FarTypeFSMComponent* pOwner)
{
	//std::cout << "Start Tracking" << std::endl;

}

void TrackEnemyState_Far::Execute(FarTypeFSMComponent* pOwner)
{
	if (dynamic_cast<Character*>(pOwner->gameObject)->GetRemainHP() <= 0.0f)
	{
		pOwner->GetFSM()->ChangeState(DeathState_Far::GetInstance());
	}
	pOwner->Track();
	if (!pOwner->CheckDistanceFromPlayer())
	{
		pOwner->GetFSM()->ChangeState(IdleState_Far::GetInstance());
	}
}

void TrackEnemyState_Far::Exit(FarTypeFSMComponent* pOwner)
{
	//std::cout << "Stop Tracking" << std::endl;
}

IdleState_Far* IdleState_Far::GetInstance()
{
	static IdleState_Far state;
	return &state;
}

void IdleState_Far::Enter(FarTypeFSMComponent* pOwner)
{
	//std::cout << "Start Idle" << std::endl;
	pOwner->Stop();
	pOwner->ResetIdleTime(RandomIdleTime(rd));
}

void IdleState_Far::Execute(FarTypeFSMComponent* pOwner)
{
	if (dynamic_cast<Character*>(pOwner->gameObject)->GetRemainHP() <= 0.0f)
	{
		pOwner->GetFSM()->ChangeState(DeathState_Far::GetInstance());
	}
	if (pOwner->CheckDistanceFromPlayer())
	{
		pOwner->GetFSM()->ChangeState(TrackEnemyState_Far::GetInstance());
	}
	if (pOwner->Idle())
	{
		pOwner->GetFSM()->ChangeState(WanderState_Far::GetInstance());
	}
}

void IdleState_Far::Exit(FarTypeFSMComponent* pOwner)
{
	//std::cout << "Stop Idel" << std::endl;
}

DeathState_Far* DeathState_Far::GetInstance()
{
	static DeathState_Far state;
	return &state;
}

void DeathState_Far::Enter(FarTypeFSMComponent* pOwner)
{
	//std::cout << "Unit Die" << std::endl;
	pOwner->gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_M_DEATH);
	((Monster*)pOwner->gameObject)->DeadSound();
}

void DeathState_Far::Execute(FarTypeFSMComponent* pOwner)
{
	pOwner->Death();
}

void DeathState_Far::Exit(FarTypeFSMComponent* pOwner)
{
}
