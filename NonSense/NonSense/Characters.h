#pragma once
#include <vector>
#include "Object.h"
#include "UI.h"
enum MonsterType
{
	MONSTER_TYPE_CLOSE, 
	MONSTER_TYPE_FAR,
	MONSTER_TYPE_RUSH,
	MONSTER_TYPE_BOSS
};

class WeaponObject : public Object
{
public:
	WeaponObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel);
	virtual ~WeaponObject();
	void Fire(XMFLOAT3& look, XMFLOAT3& pos);
};

class Character : public Object
{
protected:
	float m_Health = 100;
	float m_Defense = 100;
	float m_Attack = 100;
	float m_RemainHP = 100;
	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0,0,0);
	Shader* m_pBoundingShader = NULL;

public:
	Character(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel);
	virtual ~Character();
	virtual void GetHit(float damage) { m_RemainHP -= damage; }

	float GetHealth() { return m_Health; }
	float GetDefense() { return m_Defense; }
	float GetAttack() { return m_Attack; }
	float GetRemainHP() { return m_RemainHP; }

	void SetHealth(float f) { m_Health = f; }
	void SetDefense(float f) { m_Defense = f; }
	void SetAttack(float f) { m_Attack = f; }
	void SetRemainHP(float f) { m_RemainHP = f; }
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
};

class NPC : public Character
{
public:
	NPC(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel);
	std::vector<std::string>script;
};

class Monster : public Character
{
public:
	Monster(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel);
	virtual void OnPrepareRender();
	Monster_HP_UI* m_pHP = NULL;
	
	bool MageDamage = false;
	Object* HandFrame = NULL;
	Object* WeaponFrame = NULL;
public:
	void FarTypeAttack();
	void RushTypeAttack();
};

class Goblin : public Monster
{
public:

	Goblin(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel, LoadedModelInfo* pWeaponL, LoadedModelInfo* pWeaponR, MonsterType type);
	virtual ~Goblin();

	void CloseAttackEvent();
};

class Orc : public Monster
{
public:
	Orc(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel, LoadedModelInfo* pWeaponL, LoadedModelInfo* pWeaponR, MonsterType type);
	virtual ~Orc();

	void CloseAttackEvent();
};

class Skull : public Monster
{
public:
	Skull(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel, LoadedModelInfo* pWeaponL, LoadedModelInfo* pWeaponR, MonsterType type);
	virtual ~Skull();

	void CloseAttackEvent();
};

class Shield : public Monster
{
public:
	Shield(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel, LoadedModelInfo* pWeaponL, LoadedModelInfo* pWeaponR, MonsterType type);
	virtual ~Shield();

	void BossAttackEvent();
	void BossStealSenseEvent();
	void BossSummonEvent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void BossDefenceEvent();
	void BossJumpAttackEvent();
	void BossTorandoEvent();

	void EndSkillEvent();
};