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
	gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(AttackAnimationNumber);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	During_Attack = true;
	Targetting = false;
	TargetCoolTime = 1.0f;
	RushTime = 1.0f;
	AttackTimeLeft = AttackDuration;
	((Monster*)gameObject)->RushTypeAttack();
}
void MonsterAttackComponent::TargetOn()
{
	if (TargetCoolTime > 0.0f)
	{
		TargetCoolTime -= Timer::GetTimeElapsed();
	}
	else
	{
		Targetting = true;
	}
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
	if (RushTime > 0)
	{
		RushTime -= Timer::GetTimeElapsed();
		gameObject->MoveForward(6.0f * Timer::GetTimeElapsed());
	}
}

void MonsterAttackComponent::ResetWeapon()
{

}
void MonsterAttackComponent::SetAttackSpeed(float speed)
{
	AttackDuration = speed;
}