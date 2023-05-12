#include "MonsterAttackComponent.h"

void MonsterAttackComponent::FarTypeAttack()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(AttackAnimationNumber);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	During_Attack = true;
	WeaponFire = true;
	AttackTimeLeft = AttackDuration;
	((Monster*)gameObject)->FarTypeAttack();
}
void MonsterAttackComponent::RushTypeAttack()
{
	During_Attack = true;

	AttackTimeLeft = AttackDuration;
	((Monster*)gameObject)->RushTypeAttack();
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

void MonsterAttackComponent::ResetWeapon()
{

}
void MonsterAttackComponent::SetAttackSpeed(float speed)
{
	AttackDuration = speed;
}