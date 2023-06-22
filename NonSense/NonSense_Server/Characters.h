#pragma once
#include "Object.h"
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
	virtual ~Character();
	virtual void GetHit(float damage) { m_RemainHP -= damage; }

	float GetHealth() { return m_Health; }
	float GetDefense() { return m_Defense; }
	float GetAttack() { return m_Attack; }
	float GetRemainHP() { return m_RemainHP; }
	int GetAniType() { return PresentAniType.load(); }

	void SetHealth(float f) { m_Health = f; }
	void SetDefense(float f) { m_Defense = f; }
	void SetAttack(float f) { m_Attack = f; }
	void SetRemainHP(float f) { m_RemainHP = f; }
	void SetAniType(E_MONSTER_ANIMATION_TYPE ani_type) { PresentAniType = ani_type; }
};

class Goblin : public Character
{
public:

	Goblin(MonsterType type);
	virtual ~Goblin();
};