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
public:

	Character(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel);
	virtual ~Character() {}
	virtual void OnPrepareRender();
	virtual void update();
	virtual void OnUpdateCallback(float fTimeElapsed);
	virtual void GetHit(float damage) { m_RemainHP -= damage; }
};

class Goblin : public Character
{
public:

	Goblin(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel, LoadedModelInfo* pWeaponL, LoadedModelInfo* pWeaponR, MonsterType type);
	virtual ~Goblin() {}
};