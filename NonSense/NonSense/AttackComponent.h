#pragma once
#include "Component.h"
#include "Timer.h"
#include "Object.h"

class BoundBox;

enum AttackCombo
{
	Combo1,
	Combo2,
	Combo3
};

class AttackComponent : public Component
{
private:
	bool Type_ComboAttack = true;

public:
	bool During_Attack = false;
	BoundBox* AttackRange;
private:
	float NextAttackInputTime = 0.5;
	float AttackDuration = 0.8;
	float AttackTimeLeft = 0.0;
	
	AttackCombo type = Combo1;

public:
	void start();
	void update();

	void Attack();
	void SetBoundingObject(BoundBox* bd) { AttackRange = bd; }
};

