#include "PlayerMovementComponent.h"
#include "Input.h"
#include "Player.h"


void PlayerMovementComponent::Jump()
{
	XMFLOAT3 pos = ((Player*)gameObject)->GetPosition();
	HeightMapTerrain* pTerrain = (HeightMapTerrain*)(((Player*)gameObject)->m_pPlayerUpdatedContext);

	float fHeight = pTerrain->GetHeight(pos.x + 400.0f, pos.z + 400.0f);
	if (pos.y <= fHeight) {
		XMFLOAT3 vel = ((Player*)gameObject)->GetVelocity();
		((Player*)gameObject)->SetVelocity(XMFLOAT3(vel.x, 25.0f, vel.z));
		((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 4);
		((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 4);
		((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 4);
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
	((Player*)gameObject)->SetVelocity(vel);
	((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 3);
	((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 3);
	((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 3);
	((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackSpeed(0, 2.0f);
}

void PlayerMovementComponent::start()
{
	CursorExpose = false;
}

void PlayerMovementComponent::update()
{
	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;
	if (!CursorExpose)
	{

	
		//마우스 커서를 화면에서 없앤다(보이지 않게 한다).
		::SetCursor(NULL);
		//현재 마우스 커서의 위치를 가져온다.
		::GetCursorPos(&ptCursorPos);
		//마우스 버튼이 눌린 상태에서 마우스가 움직인 양을 구한다.
		cxDelta = (float)(ptCursorPos.x - 100.0f) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - 100.0f) / 3.0f;
		//마우스 커서의 위치를 마우스가 눌려졌던 위치로 설정한다.
		::SetCursorPos(100, 100);


	}
	if (cxDelta || cyDelta)
	{
		((Player*)gameObject)->Rotate(cyDelta, cxDelta, 0.0f);
	}

	if (((Player*)gameObject)->m_pSkinnedAnimationController)
	{
		if (Input::InputKeyBuffer[VK_SPACE] & 0xF0)
		{
			Jump();
		}
		if (Input::InputKeyBuffer['W'] & 0xF0)
		{
			((Player*)gameObject)->m_pSkinnedAnimationController->ChangeAnimationUseBlending(2);
		}
		else if (Input::InputKeyBuffer['A'] & 0xF0)
		{
			((Player*)gameObject)->m_pSkinnedAnimationController->ChangeAnimationUseBlending(2);
		}
		else if (Input::InputKeyBuffer['S'] & 0xF0)
		{
			((Player*)gameObject)->m_pSkinnedAnimationController->ChangeAnimationUseBlending(2);
		}
		else if (Input::InputKeyBuffer['D'] & 0xF0)
		{
			((Player*)gameObject)->m_pSkinnedAnimationController->ChangeAnimationUseBlending(2);
		}
		else
		{
			((Player*)gameObject)->m_pSkinnedAnimationController->ChangeAnimationUseBlending(0);
		}
		if (CursorCoolTime < MaxCursorCoolTime)
		{
			CursorCoolTime += Timer::GetTimeElapsed();
		}

		if ((Input::InputKeyBuffer[VK_MENU] & 0xF0) && CursorCoolTime >= MaxCursorCoolTime)
		{
			CursorCoolTime = 0.0f;
			CursorExpose = !CursorExpose; 
			::SetCursorPos(100, 100);
		}
		if ((Input::InputKeyBuffer[VK_LBUTTON] & 0xF0))
		{
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 6);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 6);
			((Player*)gameObject)->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 6);
		}
		if ((Input::InputKeyBuffer[VK_RBUTTON] & 0xF0) && !Dashing && CanDash)
		{
			Dash();
		}
		XMFLOAT3 v = ((Player*)gameObject)->GetVelocity();
		float len = Vector3::Length(v);
		std::cout << len << std::endl;
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
