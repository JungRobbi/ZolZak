#pragma once
#include "stdafx.h"
#include "Component.h"
#include "State.h"
#include "FSM.h"
#include "../AnimationType.h"
#include <iostream>

class BoundBox;

class BossFSMComponent : public Component
{
private:
	FSM<BossFSMComponent>* m_pFSM;
    float ChangeStateDistance = 20.0f;
    Object* TargetPlayer = NULL;
    float IdleLeftTime = 0.0f;
    float DeathCount = 3.0f;
    float SkillCoolTime = 10.0f;
public:
    XMFLOAT3 WanderPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
   

public:
	~BossFSMComponent() {}

	void start();
	void update();

    FSM<BossFSMComponent>* GetFSM();
    bool CheckDistanceFromPlayer();

    void ResetWanderPosition(float posx, float posz);
    void ResetIdleTime(float time);
    float GetSkillCoolTime();
    void SetSkillCoolTime(float time);
    XMFLOAT3 GetOwnerPosition();

    bool Idle();
    void Stop();
    void Move_Walk(float dist);
    void Move_Run(float dist);
    void Attack();
    void Track();
    bool Wander();
    void Death();

    void StealSense();
    void Summon();
    void Defence();
    void JumpAttack();
    void Torando();

    void SetTargetPlayer(Object* target) { TargetPlayer = target; };

    E_MONSTER_ANIMATION_TYPE Animation_type = E_MONSTER_ANIMATION_TYPE::E_M_IDLE;

};

