#include "AttackComponent.h"
#include "Input.h"
#include "Player.h"

void AttackComponent::Attack()
{
	if (!Type_ComboAttack)
	{
		((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 6);
		((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 6);
		((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 6);
		((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	}
	else
	{

		switch (type)
		{
		case Combo1:
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 6);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 6);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 6);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackEnable(1, false);
			type = Combo2;
			break;
		case Combo2:
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 8);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 8);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 8);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackEnable(1, false);
			type = Combo3;
			break;
		case Combo3:
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 9);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 9);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 9);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackEnable(1, false);			
			type = Combo1;
			break;
		}

	}
}

void AttackComponent::start()
{
}

void AttackComponent::update()
{
	if (((Player*)gameObject)->m_pSkinnedAnimationController)
	{
		if ((Input::InputKeyBuffer[VK_LBUTTON] & 0xF0))
		{
			if (!During_Attack)
			{

				Attack();
				AttackTimeLeft = AttackDuration + NextAttackInputTime;
				During_Attack = true;
		
			}
		}
	}
	if (AttackTimeLeft > 0.0f)
	{
		if (AttackTimeLeft > 0.0)
		{
			AttackTimeLeft -= Timer::GetTimeElapsed();
		}

		if (AttackTimeLeft < NextAttackInputTime)
		{
			During_Attack = false;
		}
	}
	else
	{
		type = Combo1;
	}
}


