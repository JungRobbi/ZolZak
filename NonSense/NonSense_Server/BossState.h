#pragma once
#include "State.h"
#include "Object.h"
#include "Components/BossFSMComponent.h"
class WanderState_Boss : public State<BossFSMComponent>
{
public:
	~WanderState_Boss() {}
	static WanderState_Boss* GetInstance();
	virtual void Enter(BossFSMComponent* pOwner);
	virtual void Execute(BossFSMComponent* pOwner);
	virtual void Exit(BossFSMComponent* pOwner);
};

class TrackEnemyState_Boss : public State<BossFSMComponent>
{
public:
	~TrackEnemyState_Boss() {}
	static TrackEnemyState_Boss* GetInstance();
	virtual void Enter(BossFSMComponent* pOwner);
	virtual void Execute(BossFSMComponent* pOwner);
	virtual void Exit(BossFSMComponent* pOwner);
};

class IdleState_Boss : public State<BossFSMComponent>
{
public:
	~IdleState_Boss() {}
	static IdleState_Boss* GetInstance();
	virtual void Enter(BossFSMComponent* pOwner);
	virtual void Execute(BossFSMComponent* pOwner);
	virtual void Exit(BossFSMComponent* pOwner);
};

class DeathState_Boss : public State<BossFSMComponent>
{
public:
	~DeathState_Boss() {}
	static DeathState_Boss* GetInstance();
	virtual void Enter(BossFSMComponent* pOwner);
	virtual void Execute(BossFSMComponent* pOwner);
	virtual void Exit(BossFSMComponent* pOwner);
};

class SkillState_Boss : public State<BossFSMComponent>
{
	float m_Timer = 0.0f;
	bool b_Enter = true;
public:
	~SkillState_Boss() {}
	static SkillState_Boss* GetInstance();
	virtual void Enter(BossFSMComponent* pOwner);
	virtual void Execute(BossFSMComponent* pOwner);
	virtual void Exit(BossFSMComponent* pOwner);
	void SetTimer(float f) { m_Timer = f; }
};