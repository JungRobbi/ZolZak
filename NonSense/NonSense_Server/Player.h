#pragma once
#include "Object.h"
#include <atomic>
#include "../AnimationType.h"

class Player :
    public Object
{
	float m_Health = 1000;
	float m_Defense = 100;
	float m_Attack = 200;
	float m_RemainHP = 1000;

public:
	class RemoteClient* remoteClient;
public:
	std::atomic<int> OldAniType{ E_PLAYER_ANIMATION_TYPE::E_IDLE };
	std::atomic<int> PresentAniType{ E_PLAYER_ANIMATION_TYPE::E_IDLE };

public:
	Player();
	virtual ~Player();

	virtual void start();
	virtual void update();

	float GetHealth() { return m_Health; }
	float GetDefense() { return m_Defense; }
	float GetAttack() { return m_Attack; }
	float GetRemainHP() { return m_RemainHP; }
	void GetHit(float damage) { m_RemainHP -= damage; }

	void SetHealth(float f) { m_Health = f; }
	void SetDefense(float f) { m_Defense = f; }
	void SetAttack(float f) { m_Attack = f; }
	void SetRemainHP(float f) { m_RemainHP = f; }
};