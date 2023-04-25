#pragma once
#include "stdafx.h"
#include "Component.h"
#include "State.h"
#include "FSM.h"

class BoundBox;

class CloseTypeFSMComponent : public Component
{
private:
    FSM<CloseTypeFSMComponent>* m_pFSM;
    float ChangeStateDistance = 5.0f;
    Object* TargetPlayer = NULL;
    BoundBox* AttackRange = NULL;

    float IdleLeftTime = 0.0f;
    XMFLOAT3 WanderPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
    float NextAttackInputTime = 0.5;
    float AttackDuration = 1.56;
    float AttackTimeLeft = 0.0;
    bool During_Attack = false;
public:
    void start();
    void update();

    FSM<CloseTypeFSMComponent>* GetFSM();
    bool CheckDistanceFromPlayer();

    void ResetWanderPosition(float posx, float posz);
    void ResetIdleTime(float time);
    XMFLOAT3 GetOwnerPosition();
    bool Idle();
    void Stop();
    void Move_Walk(float dist);
    void Move_Run(float dist);
    void Attack();
    void Track();
    bool Wander();

    void AttackRangeupdate();
    void SetBoundingObject(BoundBox* bd) { AttackRange = bd; }

};

