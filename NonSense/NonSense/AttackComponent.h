#pragma once
#include "Component.h"
#include "Timer.h"

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
private:
	float NextAttackInputTime = 0.5;
	float AttackDuration = 1.56;
	float AttackTimeLeft = 0.0;
	
	AttackCombo type = Combo1;

public:
	void start();
	void update();

	void Attack();
};

