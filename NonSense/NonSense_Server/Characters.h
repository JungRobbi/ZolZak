#pragma once
#include "Object.h"
#include <memory>
#include <atomic>
#include "../AnimationType.h"

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
	MonsterType m_type;
	float m_Health = 100;
	float m_Defense = 100;
	float m_Attack = 100;
	float m_RemainHP = 100;
	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0,0,0);

public:
	std::atomic<int> OldAniType{ E_MONSTER_ANIMATION_TYPE::E_M_IDLE };
	std::atomic<int> PresentAniType{ E_MONSTER_ANIMATION_TYPE::E_M_IDLE };

public:
	int num;
	Character();
	Character(int roomNum);
	virtual ~Character();
	virtual void GetHit(float damage) { m_RemainHP -= damage; }

	float GetHealth() { return m_Health; }
	float GetDefense() { return m_Defense; }
	float GetAttack() { return m_Attack; }
	float GetRemainHP() { return m_RemainHP; }
	int GetAniType() { return PresentAniType.load(); }
	MonsterType GetMonsterType() { return m_type; }

	void SetHealth(float f) { m_Health = f; }
	void SetDefense(float f) { m_Defense = f; }
	void SetAttack(float f) { m_Attack = f; }
	void SetRemainHP(float f) { m_RemainHP = f; }
	void SetAniType(E_MONSTER_ANIMATION_TYPE ani_type) { PresentAniType = ani_type; }
};

class Monster : public Character
{
public:
	Monster();
	Monster(int roomNum);
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

	Goblin(MonsterType type, int roomNum);
	virtual ~Goblin();
};

class Orc : public Monster
{
public:
	Orc(MonsterType type, int roomNum);
	virtual ~Orc();

	void CloseAttackEvent();
};

class Skull : public Monster
{
public:
	Skull(MonsterType type, int roomNum);
	virtual ~Skull();

	void CloseAttackEvent();
};

class Shield : public Monster
{
public:
	Shield(MonsterType type, int roomNum);
	virtual ~Shield();

	void BossAttackEvent();
	void BossStealSenseEvent();
	void BossSummonEvent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void BossDefenceEvent();
	void BossJumpAttackEvent();
	void BossTorandoEvent();

	void EndSkillEvent();
};