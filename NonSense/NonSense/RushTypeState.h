#pragma once
#include "State.h"
#include "Object.h"
#include "RushTypeFSMComponent.h"

class WanderState_Rush : public State<RushTypeFSMComponent>
{
public:
	~WanderState_Rush() {}
	static WanderState_Rush* GetInstance();
	virtual void Enter(RushTypeFSMComponent* pOwner);
	virtual void Execute(RushTypeFSMComponent* pOwner);
	virtual void Exit(RushTypeFSMComponent* pOwner);
};

class TrackEnemyState_Rush : public State<RushTypeFSMComponent>
{
public:
	~TrackEnemyState_Rush() {}
	static TrackEnemyState_Rush* GetInstance();
	virtual void Enter(RushTypeFSMComponent* pOwner);
	virtual void Execute(RushTypeFSMComponent* pOwner);
	virtual void Exit(RushTypeFSMComponent* pOwner);
};

class IdleState_Rush : public State<RushTypeFSMComponent>
{
public:
	~IdleState_Rush() {}
	static IdleState_Rush* GetInstance();
	virtual void Enter(RushTypeFSMComponent* pOwner);
	virtual void Execute(RushTypeFSMComponent* pOwner);
	virtual void Exit(RushTypeFSMComponent* pOwner);
};

class DeathState_Rush : public State<RushTypeFSMComponent>
{
public:
	~DeathState_Rush() {}
	static DeathState_Rush* GetInstance();
	virtual void Enter(RushTypeFSMComponent* pOwner);
	virtual void Execute(RushTypeFSMComponent* pOwner);
	virtual void Exit(RushTypeFSMComponent* pOwner);
};