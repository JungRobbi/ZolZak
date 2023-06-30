#pragma once
#define DIR_FORWARD 0x01
#define DIR_BACKWARD 0x02
#define DIR_LEFT 0x04
#define DIR_RIGHT 0x08
#define DIR_UP 0x10
#define DIR_DOWN 0x20

#include <string>
#include "Object.h"
#include "Shader.h"
#include "UI.h"

class Camera;

class Player : public Object
{
protected:
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

	Camera* m_pCamera = NULL;

	XMFLOAT3 m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	float m_Health = 1000;
	float m_Defense = 100;
	float m_Attack = 200;
	float m_RemainHP = 1000;
public:

	bool Magical = false;
	unsigned int id = 0;
	std::string m_name;
	Player_State_UI* m_pUI = NULL;
	Player_HP_UI* m_pHP_UI = NULL;
	Player_HP_DEC_UI* m_pHP_Dec_UI = NULL;
	float last_DeBuff = 0;
	bool dark = false;
public:

	LPVOID m_pPlayerUpdatedContext;
	LPVOID m_pCameraUpdatedContext;

	Player();
	virtual ~Player();

	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }

	void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	void Move(XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetPosition(XMFLOAT3& xmf3Position) { XMFLOAT3 pos = { xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z }; Move(pos, false); }
	void SetAnimation();
	void GetHit(float damage) { m_RemainHP -= damage; }

	void SetLookVector(XMFLOAT3 xmf3Look) { m_xmf3Look = xmf3Look; }
	void SetUpVector(XMFLOAT3 xmf3Up) { m_xmf3Up = xmf3Up; }
	void SetRightVector(XMFLOAT3 xmf3Right) { m_xmf3Right = xmf3Right; }

	void Sight_DeBuff(float sec);

	float GetHealth() { return m_Health; }
	float GetDefense() { return m_Defense; }
	float GetAttack() { return m_Attack; }
	float GetRemainHP() { return m_RemainHP; }

	void SetHealth(float f) { m_Health = f; }
	void SetDefense(float f) { m_Defense = f; }
	void SetAttack(float f) { m_Attack = f; }
	void SetRemainHP(float f) { m_RemainHP = f; }

	XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }
	float GetYaw() { return(m_fYaw); }
	float GetPitch() { return(m_fPitch); }
	float GetRoll() { return(m_fRoll); }

	Camera* GetCamera() { return(m_pCamera); }
	void SetCamera(Camera* pCamera) { m_pCamera = pCamera; }
	void Rotate(float x, float y, float z);

	virtual void Update(float fTimeElapsed);
	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }
	virtual void OnCameraUpdateCallback(float fTimeElapsed);
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	Camera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	virtual Camera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
};

class MagePlayer : public Player
{
public:
	FireBall* fireball;
	Object* pWeaponObject;
	Shader* m_pBoundingShader = NULL;
	CubeMesh* m_pBoundMesh = NULL;
	SphereMesh* m_pSphereMesh = NULL;

	MagePlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext);
	virtual ~MagePlayer() {}

	virtual Camera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
};

class WarriorPlayer : public Player
{
public:
	FireBall* fireball;
	Object* pWeaponObject;
	Shader* m_pBoundingShader = NULL;
	CubeMesh* m_pBoundMesh = NULL;
	SphereMesh* m_pSphereMesh = NULL;

	WarriorPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext);
	virtual ~WarriorPlayer() {}

	virtual Camera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
};