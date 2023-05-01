#pragma once
#include "State.h"
#include "Object.h"
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

class IdleState : public State<CloseTypeFSMComponent>
{
public:
	~IdleState() {}
	static IdleState* GetInstance();
	virtual void Enter(CloseTypeFSMComponent* pOwner);
	virtual void Execute(CloseTypeFSMComponent* pOwner);
	virtual void Exit(CloseTypeFSMComponent* pOwner);
};

class DeathState : public State<CloseTypeFSMComponent>
{
public:
	~DeathState() {}
	static DeathState* GetInstance();
	virtual void Enter(CloseTypeFSMComponent* pOwner);
	virtual void Execute(CloseTypeFSMComponent* pOwner);
	virtual void Exit(CloseTypeFSMComponent* pOwner);
};