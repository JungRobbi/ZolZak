#pragma once
#include "Object.h"
#include "UI.h"
enum MonsterType
{
	MONSTER_TYPE_CLOSE, 
	MONSTER_TYPE_FAR,
	MONSTER_TYPE_RUSH,
	MONSTER_TYPE_BOSS
};


class Character : public Object
{
protected:
	float m_Health = 100;
	float m_Defense = 100;
	float m_Attack = 100;
	float m_RemainHP = 100;
	Monster_HP_UI* m_pHP = NULL;
	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0,0,0);
	Shader* m_pBoundingShader = NULL;
	CubeMesh* m_pBoundMesh = NULL;
public:

	Character(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel);
	virtual ~Character();
	virtual void OnPrepareRender();
	virtual void GetHit(float damage) { m_RemainHP -= damage; }

	float GetHealth() { return m_Health; }
	float GetDefense() { return m_Defense; }
	float GetAttack() { return m_Attack; }
	float GetRemainHP() { return m_RemainHP; }

	void SetHealth(float f) { m_Health = f; }
	void SetDefense(float f) { m_Defense = f; }
	void SetAttack(float f) { m_Attack = f; }
	void SetRemainHP(float f) { m_RemainHP = f; }
};

class Goblin : public Character
{
public:

	Goblin(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel, LoadedModelInfo* pWeaponL, LoadedModelInfo* pWeaponR, MonsterType type);
	virtual ~Goblin();
};