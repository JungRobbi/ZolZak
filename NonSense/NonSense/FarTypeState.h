#pragma once
#include "State.h"
#include "Object.h"
#include "FarTypeFSMComponent.h"

class WanderState_Far : public State<FarTypeFSMComponent>
{
public:
	~WanderState_Far() {}
	static WanderState_Far* GetInstance();
	virtual void Enter(FarTypeFSMComponent* pOwner);
	virtual void Execute(FarTypeFSMComponent* pOwner);
	virtual void Exit(FarTypeFSMComponent* pOwner);
};

class TrackEnemyState_Far : public State<FarTypeFSMComponent>
{
public:
	~TrackEnemyState_Far() {}
	static TrackEnemyState_Far* GetInstance();
	virtual void Enter(FarTypeFSMComponent* pOwner);
	virtual void Execute(FarTypeFSMComponent* pOwner);
	virtual void Exit(FarTypeFSMComponent* pOwner);
};

class IdleState_Far : public State<FarTypeFSMComponent>
{
public:
	~IdleState_Far() {}
	static IdleState_Far* GetInstance();
	virtual void Enter(FarTypeFSMComponent* pOwner);
	virtual void Execute(FarTypeFSMComponent* pOwner);
	virtual void Exit(FarTypeFSMComponent* pOwner);
};

class DeathState_Far : public State<FarTypeFSMComponent>
{
public:
	~DeathState_Far() {}
	static DeathState_Far* GetInstance();
	virtual void Enter(FarTypeFSMComponent* pOwner);
	virtual void Execute(FarTypeFSMComponent* pOwner);
	virtual void Exit(FarTypeFSMComponent* pOwner);
};