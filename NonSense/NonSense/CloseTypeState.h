#pragma once
#include "State.h"
#include "CloseTypeFSMComponent.h"

class WanderState : public State<CloseTypeFSMComponent>
{
public:
	~WanderState() {}
	static WanderState* GetInstance();
	virtual void Enter(CloseTypeFSMComponent* pOwner);
	virtual void Execute(CloseTypeFSMComponent* pOwner);
	virtual void Exit(CloseTypeFSMComponent* pOwner);
};

class TrackEnemyState : public State<CloseTypeFSMComponent>
{
public:
	~TrackEnemyState() {}
	static TrackEnemyState* GetInstance();
	virtual void Enter(CloseTypeFSMComponent* pOwner);
	virtual void Execute(CloseTypeFSMComponent* pOwner);
	virtual void Exit(CloseTypeFSMComponent* pOwner);
};