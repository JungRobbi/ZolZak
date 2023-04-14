#pragma once
#include "Component.h"
#include "Timer.h"
#include "Component.h"
class AttackComponent : public Component
{
private:
	bool Type_ComboAttack = true;

public:
	bool During_Attack = false;
private:
	float NextAttackInputTime = 0.5;
	float AttackDuration = 0.9;
	float AttackTimeLeft = 0.0;
	
public:
	void start();
	void update();

	void Attack();
};

