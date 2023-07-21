#pragma once
#include "Component.h"
#include "../Timer.h"
#include "../Object.h"

class BoundBox;

enum AttackCombo
{
	Combo1,
	Combo2,
	Combo3
};

class AttackComponent : public Component
{
public:
	bool During_Attack = false;
	BoundBox* AttackRange = NULL;
	bool Type_ComboAttack = true;
	int AttackCombo1_AnineSetNum = 6;
	int AttackCombo2_AnineSetNum = 8;
	int AttackCombo3_AnineSetNum = 9;
private:
	float NextAttackInputTime = 0.5;
	float AttackDuration = 1.56;
	float AttackTimeLeft = 0.0;
	

	AttackCombo type = Combo1;

public:
	void start();
	void update();
	void SetAttackSpeed(float speed);

	void Attack();
	void SetBoundingObject(BoundBox* bd) { AttackRange = bd; }
	void CheckMonsterAttackRange();
};

