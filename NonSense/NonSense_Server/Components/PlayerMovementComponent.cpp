#include "../stdafx.h"
#include <directxcollision.h>
#include <vector>
#include "PlayerMovementComponent.h"
#include "BoxCollideComponent.h"
#include "SphereCollideComponent.h"
#include "../Timer.h"
#include "../RemoteClients/RemoteClient.h"
#include "../Player.h"
#include "../Scene.h"


void PlayerMovementComponent::start()
{
	scene = dynamic_cast<Scene*>(Scene::scene);

	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, -60.0f, 0.0f);
	m_fMaxVelocityXZ = 3.5f;
	m_fMaxVelocityY = 100.0f;
	m_fFriction = 30.0f;
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
}

void PlayerMovementComponent::update()
{
	DWORD direction = 0;

	/*if (keyboard['W'] || keyboard['w']) 
		direction |= DIR_FORWARD;
	if (keyboard['S'] || keyboard['s']) 
		direction |= DIR_BACKWARD;
	if (keyboard['A'] || keyboard['a']) 
		direction |= DIR_LEFT;
	if (keyboard['D'] || keyboard['d']) 
		direction |= DIR_RIGHT;*/

	if (direction) {
		/*if (dynamic_cast<Player*>(gameObject)->PresentAniType == E_PLAYER_ANIMATION_TYPE::E_WALK) {
			SetMaxVelocityXZ(3.f);
			Move(direction, 10.0f * Timer::GetTimeElapsed(), false);
		}
		else {
			Move(direction, 40.0f * Timer::GetTimeElapsed(), false);
		}*/
	}
	updateValocity();

	if (Dashing)
	{
		if (DashTimeLeft > 0.0f)
		{
			DashTimeLeft -= Timer::GetTimeElapsed();
		}
		else
		{
			Dashing = false;
			SetVelocity(XMFLOAT3(0, 0, 0));
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

void PlayerMovementComponent::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);
		Move(xmf3Shift, bUpdateVelocity);
	}
}
void PlayerMovementComponent::Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		auto cc = gameObject->GetComponent<SphereCollideComponent>();
		if (cc) {
			BoundSphere nextBB{ false };
			nextBB.Center = cc->GetBoundingObject()->Center;
			nextBB.Radius = cc->GetBoundingObject()->Radius;

			nextBB.Center.x += xmf3Shift.x;
			nextBB.Center.y += xmf3Shift.y;
			nextBB.Center.z += xmf3Shift.z;

			for (auto mapObject : Scene::scene->GetMapObjects()) {
				if (nextBB.Intersects(*(BoundBox*)mapObject)) {
					XMFLOAT3 Corners[8];
					XMVECTOR vCorners[8];
					XMVECTOR vOrigin = XMLoadFloat3(&cc->GetBoundingObject()->Center);
					XMVECTOR vDirection = XMLoadFloat3(&xmf3Shift);
					XMVECTOR vNorDirection = XMVector3Normalize(vDirection);

					((BoundBox*)mapObject)->GetCorners(Corners);
		
					// XMFLOAT3 to XMVECTOR
					for (int i{}; i < 8; ++i)
						vCorners[i] = XMLoadFloat3(&Corners[i]);
			
					std::vector<std::vector<int>> CornersIndex{
						{0, 1, 2},{0, 2, 3},
						{0, 3, 7},{0, 7, 4},
						{4, 7, 5},{5, 7, 6},
						{1, 5, 6},{1, 6, 2},
						{6, 3, 2},{7, 3, 6},
						{5, 1, 0},{5, 0, 4}
					};
					
					for (int i{}; i < 12; ++i) {
						auto Normal = Vector3::CrossProduct(
							Vector3::Subtract(Corners[CornersIndex[i][1]], Corners[CornersIndex[i][0]]),
							Vector3::Subtract(Corners[CornersIndex[i][2]], Corners[CornersIndex[i][0]]), true);

						// Normal과 평행하면 충돌 처리 금지
						auto dotProduct = Vector3::DotProduct(xmf3Shift, Normal);

						if (dotProduct < 0 && DirectX::TriangleTests::Intersects(vOrigin, vNorDirection,
							vCorners[CornersIndex[i][0]], vCorners[CornersIndex[i][1]], vCorners[CornersIndex[i][2]],
							nextBB.Radius)) {
							//	cout << "Player의 다음 움직임에 MapObjectdml 1번 삼각형과 충돌!" << endl;
							// 슬라이딩 벡터 S = P - n(P·n)
							xmf3Shift = Vector3::Subtract(xmf3Shift,
								Vector3::ScalarProduct(Normal, dotProduct, false)
							);
						}
					}
				}
			}
		}


		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		gameObject->SetPosition(m_xmf3Position);
	}
}

void PlayerMovementComponent::updateValocity()
{
	if (dynamic_cast<Player*>(gameObject)->PresentAniType == E_PLAYER_ANIMATION_TYPE::E_WALK) {
		SetMaxVelocityXZ(1.f);
	}
	else {
		SetMaxVelocityXZ(3.5f);
	}

	auto fTimeElapsed = Timer::GetTimeElapsed();
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;
	if (fLength > m_fMaxVelocityXZ) {
		m_xmf3Velocity.x *= (m_fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (m_fMaxVelocityXZ / fLength);
	} 
	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);
//	if (Vector3::Length(m_xmf3Velocity) > 3.0f) {
		XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
		Move(xmf3Velocity, false);
//	}

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
	
	if (m_pPlayerUpdatedContext) 
		OnPlayerUpdateCallback();
}

void PlayerMovementComponent::Jump()
{
	HeightMapTerrain* pTerrain = (HeightMapTerrain*)m_pPlayerUpdatedContext;

	float fHeight = pTerrain->GetHeight(m_xmf3Position.x - pTerrain->GetPosition().x, m_xmf3Position.z - pTerrain->GetPosition().z);
	if (m_xmf3Position.y <= fHeight) {
		m_xmf3Velocity.y = 22.f;
	}
}

void PlayerMovementComponent::Dash()
{
	Dashing = true;
	CanDash = false;
	DashTimeLeft = DashDuration;
	DashCoolTimeLeft = DashCoolTime;

	XMFLOAT3 look = GetLookVector();
	float DistanceRatio = DashDistance / DashDuration;
//	SetMaxVelocityXZ(6.5f);
	m_xmf3Velocity.x += look.x * DistanceRatio;
	m_xmf3Velocity.y += look.y * DistanceRatio;
	m_xmf3Velocity.z += look.z * DistanceRatio;
}

void PlayerMovementComponent::OnPlayerUpdateCallback()
{
	HeightMapTerrain* pTerrain = (HeightMapTerrain*)m_pPlayerUpdatedContext;

	float fHeight = pTerrain->GetHeight(m_xmf3Position.x - pTerrain->GetPosition().x, m_xmf3Position.z - pTerrain->GetPosition().z);
	if (m_xmf3Position.y <= fHeight) {
		m_xmf3Velocity.y = -3.0f;
		m_xmf3Position.y = fHeight;
	}
}