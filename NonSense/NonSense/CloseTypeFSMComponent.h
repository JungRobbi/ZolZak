#pragma once
#include "stdafx.h"
#include "Component.h"
#include "State.h"
#include "FSM.h"
class CloseTypeFSMComponent : public Component
{
private:
    FSM<CloseTypeFSMComponent>* m_pFSM;
    float ChangeStateDistance = 5.0f;
    Object* TargetPlayer = NULL;
public:
    void start();
    void update();

    FSM<CloseTypeFSMComponent>* GetFSM();
    bool CheckDistanceFromPlayer();

    void Stop();
    void Move_Walk(float dist);
    void Move_Run(float dist);
    void Attack();
    void Track();
    void Wander();

};

