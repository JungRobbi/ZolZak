#pragma once
#include "Component.h"
#include "../Timer.h"
#include "../Object.h"

class BossAttackComponent : public Component
{
public:
	bool During_Attack = false;
	BoundBox* AttackRange = NULL;
	bool During_Skill = false;
	bool End_Skill = false;
private:
	float AttackDuration = 2.0;
	float AttackTimeLeft = 0.0;
	float DefenceTimeLeft = 0;
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
	void Summon(XMFLOAT3 Pos);
	
	void DefenceAnimation();
	void Defence();

	void JumpAttackAnimation();
	void JumpAttack();

	void TornadoAnimation();
	void Tornado();
};