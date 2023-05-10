#include "MonsterAttackComponent.h"

void MonsterAttackComponent::FarTypeAttack()
{
	During_Attack = true;
	((Monster*)gameObject)->FarTypeAttack(); 
}
void MonsterAttackComponent::RushTypeAttack()
{
	During_Attack = true;
	((Monster*)gameObject)->FarTypeAttack();
}

void MonsterAttackComponent::start()
{
}

void MonsterAttackComponent::update()
{
	if (AttackTimeLeft > 0.0f)
	{
		AttackTimeLeft -= Timer::GetTimeElapsed();
	}
	else
	{
		During_Attack = false;
	}
}
void MonsterAttackComponent::SetAttackSpeed(float speed)
{
	AttackDuration = speed;
}