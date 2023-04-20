#pragma once
#include "../stdafx.h"
#include "Component.h"

class PlayerMovementComponent :
    public Component
{
    class Scene* scene;

	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Right;
	XMFLOAT3 m_xmf3Up;
	XMFLOAT3 m_xmf3Look;
	float m_fPitch;
	float m_fYaw;
	float m_fRoll;
	XMFLOAT3 m_xmf3Velocity;
	XMFLOAT3 m_xmf3Gravity;
	float m_fMaxVelocityXZ;
	float m_fMaxVelocityY;
	float m_fFriction;

public:
    void start();
    void update();

	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }

	XMFLOAT3 GetVelocity() { return(m_xmf3Velocity); }
	float GetYaw() { return(m_fYaw); }
	float GetPitch() { return(m_fPitch); }
	float GetRoll() { return(m_fRoll); }

	void SetPosition(XMFLOAT3 xmf3Position) { XMFLOAT3 pos = { xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z }; Move(pos, false); }
	void SetLookVector(XMFLOAT3 xmf3Look) { m_xmf3Look = xmf3Look; }
	void SetUpVector(XMFLOAT3 xmf3Up) { m_xmf3Up = xmf3Up; }
	void SetRightVector(XMFLOAT3 xmf3Right) { m_xmf3Right = xmf3Right; }
	void SetVelocity(XMFLOAT3 xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	
	void SetGravity(XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetFriction(float fFriction) { m_fFriction = fFriction; }


	void Move(DWORD nDirection, float fDistance, bool bVelocity = false);
	void Move(XMFLOAT3& xmf3Shift, bool bVelocity = false);

	void updateValocity();
};

