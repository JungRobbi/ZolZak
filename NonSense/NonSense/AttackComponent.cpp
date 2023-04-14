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
		if (AttackTimeLeft > 0.0f) {
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 7);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 7);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 7);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackEnable(1, false);
		}
		else
		{
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 6);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 6);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 6);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackEnable(1, false);
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
	if (During_Attack || AttackTimeLeft > 0.0f)
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
}


