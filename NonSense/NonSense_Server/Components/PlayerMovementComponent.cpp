#include "../stdafx.h"
#include <directxcollision.h>
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
	m_fMaxVelocityXZ = 6.5f;
	m_fMaxVelocityY = 400.0f;
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
			
					if (DirectX::TriangleTests::Intersects(vOrigin, vNorDirection,
						vCorners[0], vCorners[1], vCorners[2],
						nextBB.Radius)) {
					//	cout << "Player의 다음 움직임에 MapObjectdml 1번 삼각형과 충돌!" << endl;
						auto Normal = Vector3::CrossProduct(
							Vector3::Subtract(Corners[1], Corners[0]),
							Vector3::Subtract(Corners[2], Corners[0]), true);
						
						auto dotProduct = Vector3::DotProduct(xmf3Shift, Normal);

						// Normal과 평행하면 충돌 처리 금지
						if (dotProduct <= 0) {
							// 슬라이딩 벡터 S = P - n(P·n)
							xmf3Shift = Vector3::Subtract(xmf3Shift,
								Vector3::ScalarProduct(Normal, dotProduct, false)
							);
						}
					}
					else if (DirectX::TriangleTests::Intersects(vOrigin, vNorDirection,
						vCorners[0], vCorners[2], vCorners[3],
						nextBB.Radius)) {
					//	cout << "Player의 다음 움직임에 MapObjectdml 2번 삼각형과 충돌!" << endl;
						auto Normal = Vector3::CrossProduct(
							Vector3::Subtract(Corners[2], Corners[0]),
							Vector3::Subtract(Corners[3], Corners[0]), true);

						auto dotProduct = Vector3::DotProduct(xmf3Shift, Normal);

						// Normal과 평행하면 충돌 처리 금지
						if (dotProduct <= 0) {
							// 슬라이딩 벡터 S = P - n(P·n)
							xmf3Shift = Vector3::Subtract(xmf3Shift,
								Vector3::ScalarProduct(Normal, dotProduct, false)
							);
						}
					}
					else if (DirectX::TriangleTests::Intersects(vOrigin, vNorDirection,
						vCorners[0], vCorners[3], vCorners[7],
						nextBB.Radius)) {
					//	cout << "Player의 다음 움직임에 MapObjectdml 3번 삼각형과 충돌!" << endl;
						auto Normal = Vector3::CrossProduct(
							Vector3::Subtract(Corners[3], Corners[0]),
							Vector3::Subtract(Corners[7], Corners[0]), true);

						auto dotProduct = Vector3::DotProduct(xmf3Shift, Normal);

						// Normal과 평행하면 충돌 처리 금지
						if (dotProduct <= 0) {
							// 슬라이딩 벡터 S = P - n(P·n)
							xmf3Shift = Vector3::Subtract(xmf3Shift,
								Vector3::ScalarProduct(Normal, dotProduct, false)
							);
						}
					}
					else if (DirectX::TriangleTests::Intersects(vOrigin, vNorDirection,
						vCorners[0], vCorners[7], vCorners[4],
						nextBB.Radius)) {
					//	cout << "Player의 다음 움직임에 MapObjectdml 4번 삼각형과 충돌!" << endl;
						auto Normal = Vector3::CrossProduct(
							Vector3::Subtract(Corners[7], Corners[0]),
							Vector3::Subtract(Corners[4], Corners[0]), true);

						auto dotProduct = Vector3::DotProduct(xmf3Shift, Normal);

						// Normal과 평행하면 충돌 처리 금지
						if (dotProduct <= 0) {
							// 슬라이딩 벡터 S = P - n(P·n)
							xmf3Shift = Vector3::Subtract(xmf3Shift,
								Vector3::ScalarProduct(Normal, dotProduct, false)
							);
						}
					}
					else if (DirectX::TriangleTests::Intersects(vOrigin, vNorDirection,
						vCorners[4], vCorners[7], vCorners[5],
						nextBB.Radius)) {
					//	cout << "Player의 다음 움직임에 MapObjectdml 5번 삼각형과 충돌!" << endl;
						auto Normal = Vector3::CrossProduct(
							Vector3::Subtract(Corners[7], Corners[4]),
							Vector3::Subtract(Corners[5], Corners[4]), true);

						auto dotProduct = Vector3::DotProduct(xmf3Shift, Normal);

						// Normal과 평행하면 충돌 처리 금지
						if (dotProduct <= 0) {
							// 슬라이딩 벡터 S = P - n(P·n)
							xmf3Shift = Vector3::Subtract(xmf3Shift,
								Vector3::ScalarProduct(Normal, dotProduct, false)
							);
						}
					}
					else if (DirectX::TriangleTests::Intersects(vOrigin, vNorDirection,
						vCorners[5], vCorners[7], vCorners[6],
						nextBB.Radius)) {
					//	cout << "Player의 다음 움직임에 MapObjectdml 6번 삼각형과 충돌!" << endl;
						auto Normal = Vector3::CrossProduct(
							Vector3::Subtract(Corners[7], Corners[5]),
							Vector3::Subtract(Corners[6], Corners[5]), true);

						auto dotProduct = Vector3::DotProduct(xmf3Shift, Normal);

						// Normal과 평행하면 충돌 처리 금지
						if (dotProduct <= 0) {
							// 슬라이딩 벡터 S = P - n(P·n)
							xmf3Shift = Vector3::Subtract(xmf3Shift,
								Vector3::ScalarProduct(Normal, dotProduct, false)
							);
						}
					}
					else if(DirectX::TriangleTests::Intersects(vOrigin, vNorDirection,
						vCorners[1], vCorners[5], vCorners[6],
						nextBB.Radius)) {
					//	cout << "Player의 다음 움직임에 MapObjectdml 7번 삼각형과 충돌!" << endl;
						auto Normal = Vector3::CrossProduct(
							Vector3::Subtract(Corners[5], Corners[1]),
							Vector3::Subtract(Corners[6], Corners[1]), true);

						auto dotProduct = Vector3::DotProduct(xmf3Shift, Normal);

						// Normal과 평행하면 충돌 처리 금지
						if (dotProduct <= 0) {
							// 슬라이딩 벡터 S = P - n(P·n)
							xmf3Shift = Vector3::Subtract(xmf3Shift,
								Vector3::ScalarProduct(Normal, dotProduct, false)
							);
						}
					}
					else if (DirectX::TriangleTests::Intersects(vOrigin, vNorDirection,
						vCorners[1], vCorners[6], vCorners[2],
						nextBB.Radius)) {
					//	cout << "Player의 다음 움직임에 MapObjectdml 8번 삼각형과 충돌!" << endl;
						auto Normal = Vector3::CrossProduct(
							Vector3::Subtract(Corners[6], Corners[1]),
							Vector3::Subtract(Corners[2], Corners[1]), true);

						auto dotProduct = Vector3::DotProduct(xmf3Shift, Normal);

						// Normal과 평행하면 충돌 처리 금지
						if (dotProduct <= 0) {
							// 슬라이딩 벡터 S = P - n(P·n)
							xmf3Shift = Vector3::Subtract(xmf3Shift,
								Vector3::ScalarProduct(Normal, dotProduct, false)
							);
						}
					}
					else if (DirectX::TriangleTests::Intersects(vOrigin, vNorDirection,
						vCorners[6], vCorners[3], vCorners[2],
						nextBB.Radius)) {
					//	cout << "Player의 다음 움직임에 MapObjectdml 9번 삼각형과 충돌!" << endl;
						auto Normal = Vector3::CrossProduct(
							Vector3::Subtract(Corners[3], Corners[6]),
							Vector3::Subtract(Corners[2], Corners[6]), true);

						auto dotProduct = Vector3::DotProduct(xmf3Shift, Normal);

						// Normal과 평행하면 충돌 처리 금지
						if (dotProduct <= 0) {
							// 슬라이딩 벡터 S = P - n(P·n)
							xmf3Shift = Vector3::Subtract(xmf3Shift,
								Vector3::ScalarProduct(Normal, dotProduct, false)
							);
						}
					}
					else if (DirectX::TriangleTests::Intersects(vOrigin, vNorDirection,
						vCorners[7], vCorners[3], vCorners[6],
						nextBB.Radius)) {
					//	cout << "Player의 다음 움직임에 MapObjectdml 10번 삼각형과 충돌!" << endl;
						auto Normal = Vector3::CrossProduct(
							Vector3::Subtract(Corners[3], Corners[7]),
							Vector3::Subtract(Corners[6], Corners[7]), true);

						auto dotProduct = Vector3::DotProduct(xmf3Shift, Normal);

						// Normal과 평행하면 충돌 처리 금지
						if (dotProduct <= 0) {
							// 슬라이딩 벡터 S = P - n(P·n)
							xmf3Shift = Vector3::Subtract(xmf3Shift,
								Vector3::ScalarProduct(Normal, dotProduct, false)
							);
						}
					}
					else if (DirectX::TriangleTests::Intersects(vOrigin, vNorDirection,
						vCorners[5], vCorners[1], vCorners[0],
						nextBB.Radius)) {
					//	cout << "Player의 다음 움직임에 MapObjectdml 11번 삼각형과 충돌!" << endl;
						auto Normal = Vector3::CrossProduct(
							Vector3::Subtract(Corners[1], Corners[5]),
							Vector3::Subtract(Corners[0], Corners[5]), true);

						auto dotProduct = Vector3::DotProduct(xmf3Shift, Normal);

						// Normal과 평행하면 충돌 처리 금지
						if (dotProduct <= 0) {
							// 슬라이딩 벡터 S = P - n(P·n)
							xmf3Shift = Vector3::Subtract(xmf3Shift,
								Vector3::ScalarProduct(Normal, dotProduct, false)
							);
						}
					}
					else if (DirectX::TriangleTests::Intersects(vOrigin, vNorDirection,
						vCorners[5], vCorners[0], vCorners[4],
						nextBB.Radius)) {
					//	cout << "Player의 다음 움직임에 MapObjectdml 12번 삼각형과 충돌!" << endl;
						auto Normal = Vector3::CrossProduct(
							Vector3::Subtract(Corners[0], Corners[5]),
							Vector3::Subtract(Corners[4], Corners[5]), true);

						auto dotProduct = Vector3::DotProduct(xmf3Shift, Normal);

						// Normal과 평행하면 충돌 처리 금지
						if (dotProduct <= 0) {
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
		SetMaxVelocityXZ(1.8f);
	}
	else {
		SetMaxVelocityXZ(6.5f);
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
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	Move(xmf3Velocity, false);
	
	if (m_pPlayerUpdatedContext) 
		OnPlayerUpdateCallback();

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
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
		m_xmf3Velocity.y = -5.0f;
		m_xmf3Position.y = fHeight;
	}
}