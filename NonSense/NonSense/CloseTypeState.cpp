#include "CloseTypeState.h"

WanderState* WanderState::GetInstance()
{
	static WanderState state;
	return &state;
}

void WanderState::Enter(CloseTypeFSMComponent* pOwner)
{
	std::cout << "Start Wandering" << std::endl;
}

void WanderState::Execute(CloseTypeFSMComponent* pOwner)
{
	std::cout << "Wandering" << std::endl;
	if (pOwner->CheckDistanceFromPlayer())
	{
		pOwner->GetFSM()->ChangeState(TrackEnemyState::GetInstance());
	}
}

void WanderState::Exit(CloseTypeFSMComponent* pOwner)
{
	std::cout << "Stop Wandering" << std::endl;
}

TrackEnemyState* TrackEnemyState::GetInstance()
{
	static TrackEnemyState state;
	return &state;
}

void TrackEnemyState::Enter(CloseTypeFSMComponent* pOwner)
{
	std::cout << "Start Tracking" << std::endl;
}

void TrackEnemyState::Execute(CloseTypeFSMComponent* pOwner)
{
	std::cout << "Tracking" << std::endl;
	if (!pOwner->CheckDistanceFromPlayer())
	{
		pOwner->GetFSM()->ChangeState(WanderState::GetInstance());
	}
}

void TrackEnemyState::Exit(CloseTypeFSMComponent* pOwner)
{
	std::cout << "Stop Tracking" << std::endl;
}
