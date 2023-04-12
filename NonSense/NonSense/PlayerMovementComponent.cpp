#include "PlayerMovementComponent.h"
#include "Input.h"
#include "Player.h"

void PlayerMovementComponent::start()
{
}

void PlayerMovementComponent::update()
{
	if (Input::InputKeyBuffer[VK_SPACE] & 0xF0)
	{
		XMFLOAT3 vel = ((Player*)gameObject)->GetVelocity();
		((Player*)gameObject)->SetVelocity(XMFLOAT3(vel.x, 40.0f, vel.z));
		((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 4);
		((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 4);
		((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 4);
		// �̰� �Լ��� ����
	}
}
