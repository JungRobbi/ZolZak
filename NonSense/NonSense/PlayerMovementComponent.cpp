#include "PlayerMovementComponent.h"
#include "AttackComponent.h"
#include "Input.h"
#include "Player.h"
#include "NetworkMGR.h"

#include "../AnimationType.h"

void PlayerMovementComponent::Jump()
{

		if (NetworkMGR::b_isNet) {
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(0, (int)E_PLAYER_ANIMATION_TYPE::E_JUMP);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(1, (int)E_PLAYER_ANIMATION_TYPE::E_JUMP);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(2, (int)E_PLAYER_ANIMATION_TYPE::E_JUMP);
			return;
		}

		XMFLOAT3 pos = ((Player*)gameObject)->GetPosition();
		HeightMapTerrain* pTerrain = (HeightMapTerrain*)(((Player*)gameObject)->m_pPlayerUpdatedContext);

		float fHeight = pTerrain->GetHeight(pos.x + 400.0f, pos.z + 400.0f);
		if (pos.y <= fHeight) {
			XMFLOAT3 vel = ((Player*)gameObject)->GetVelocity();
			if (!NetworkMGR::b_isNet)
				((Player*)gameObject)->SetVelocity(XMFLOAT3(vel.x, 25.0f, vel.z));
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(0, (int)E_PLAYER_ANIMATION_TYPE::E_JUMP);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(1, (int)E_PLAYER_ANIMATION_TYPE::E_JUMP);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(2, (int)E_PLAYER_ANIMATION_TYPE::E_JUMP);
		}

}

void PlayerMovementComponent::Dash()
{

		Dashing = true;
		CanDash = false;
		DashTimeLeft = DashDuration;
		DashCoolTimeLeft = DashCoolTime;

		XMFLOAT3 look = ((Player*)gameObject)->GetLook();
		float DistanceRatio = DashDistance / DashDuration;
		XMFLOAT3 vel = XMFLOAT3(look.x * DistanceRatio, look.y * DistanceRatio, look.z * DistanceRatio);
		if (!NetworkMGR::b_isNet) {
			((Player*)gameObject)->SetMaxVelocityXZ(6.5f);
			((Player*)gameObject)->SetVelocity(vel);
		}
		((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(0, (int)E_PLAYER_ANIMATION_TYPE::E_DASH);
		((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(1, (int)E_PLAYER_ANIMATION_TYPE::E_DASH);
		((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(2, (int)E_PLAYER_ANIMATION_TYPE::E_DASH);
		((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackSpeed(0, 2.0f);

}

void PlayerMovementComponent::SetWindowPos(RECT pos)
{
	WindowPos = pos;
	CenterOfWindow.x = (pos.right - pos.left) / 2;
	CenterOfWindow.y = (pos.bottom - pos.top) / 2;
}

void PlayerMovementComponent::start()
{
	CursorExpose = false;
}

void PlayerMovementComponent::update()
{

	if (!ScriptMode) {
		if (((Player*)gameObject)->m_pSkinnedAnimationController)
		{
			if (Input::InputKeyBuffer[VK_SPACE] & 0xF0 && !NetworkMGR::b_isNet)
			{
				Jump();
			}
			else if (b_Jump)
			{
				Jump();
				b_Jump = false;
			}
			if (Input::InputKeyBuffer[VK_LSHIFT] & 0xF0)
			{
				if (!Dashing)
					((Player*)gameObject)->SetMaxVelocityXZ(0.7f);
			}
			else
			{
				if (!Dashing)
					((Player*)gameObject)->SetMaxVelocityXZ(3.5f);
			}
			if (!NetworkMGR::b_isNet) {
				if (Input::InputKeyBuffer['W'] & 0xF0)
				{
					if (Input::InputKeyBuffer[VK_LSHIFT] & 0xF0)
					{
						Animation_type = E_PLAYER_ANIMATION_TYPE::E_WALK;
					}
					else
					{
						Animation_type = E_PLAYER_ANIMATION_TYPE::E_RUN;
					}

				}
				else if (Input::InputKeyBuffer['A'] & 0xF0)
				{
					if (Input::InputKeyBuffer[VK_LSHIFT] & 0xF0)
					{
						Animation_type = E_PLAYER_ANIMATION_TYPE::E_WALK;
					}
					else
					{
						Animation_type = E_PLAYER_ANIMATION_TYPE::E_RUN;
					}
				}
				else if (Input::InputKeyBuffer['S'] & 0xF0)
				{
					if (Input::InputKeyBuffer[VK_LSHIFT] & 0xF0)
					{
						Animation_type = E_PLAYER_ANIMATION_TYPE::E_WALK;
					}
					else
					{
						Animation_type = E_PLAYER_ANIMATION_TYPE::E_RUN;
					}
				}
				else if (Input::InputKeyBuffer['D'] & 0xF0)
				{
					if (Input::InputKeyBuffer[VK_LSHIFT] & 0xF0)
					{
						Animation_type = E_PLAYER_ANIMATION_TYPE::E_WALK;
					}
					else
					{
						Animation_type = E_PLAYER_ANIMATION_TYPE::E_RUN;
					}
				}
				else
				{
					if (!((Player*)gameObject)->GetComponent<AttackComponent>()->During_Attack)
						Animation_type = E_PLAYER_ANIMATION_TYPE::E_IDLE;
				}
			}

			((Player*)gameObject)->m_pSkinnedAnimationController->ChangeAnimationUseBlending((int)Animation_type);

			if (CursorCoolTime < MaxCursorCoolTime)
			{
				CursorCoolTime += Timer::GetTimeElapsed();
			}

			if ((Input::InputKeyBuffer[VK_MENU] & 0xF0) && CursorCoolTime >= MaxCursorCoolTime)
			{
				CursorCoolTime = 0.0f;

				CursorExpose = !CursorExpose;
			}

			if ((Input::InputKeyBuffer[VK_RBUTTON] & 0xF0) && !Dashing && CanDash && !NetworkMGR::b_isNet)
			{
				Dash();
			}
			else if (b_Dash)
			{
				Dash();
				b_Dash = false;
			}
			if (Dashing)
			{
				if (DashTimeLeft > 0.0f)
				{
					DashTimeLeft -= Timer::GetTimeElapsed();
				}
				else
				{
					Dashing = false;
					((Player*)gameObject)->SetVelocity(XMFLOAT3(0, 0, 0));
					((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackSpeed(0, 1.0f);
				}
			}
			if (!CanDash)
			{
				if (DashCoolTimeLeft > 0.0f)
				{
					DashCoolTimeLeft -= Timer::GetTimeElapsed();
				}
				else
				{
					CanDash = true;
				}
			}

		}
	}
}
