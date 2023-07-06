#pragma once
#include "Component.h"
#include "Timer.h"
#include "Object.h"

class BossAttackComponent : Component
{
public:
	bool During_Attack = false;
	BoundBox* AttackRange = NULL;

private:
	float AttackDuration = 1.06;
	float AttackTimeLeft = 0.0;

public:
	~BossAttackComponent() {}
	void start();
	void update();
	void SetAttackSpeed(float speed);
	void SetAttackDuration(float d) { AttackDuration = d; }

	void AttackAnimation();
	void Attack();

	void StealSenseAnimation();
	void StealSense();

	void SummonAnimation();
	void Summon();
	
	void DefenceAnimation();
	void Defence();

	void JumpAttackAnimation();
	void JumpAttack();

	void TornadoAnimation();
	void Tornado();
};