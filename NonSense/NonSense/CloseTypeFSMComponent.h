#pragma once
#include "stdafx.h"
#include "Component.h"
#include "State.h"
#include "FSM.h"
class CloseTypeFSMComponent : public Component
{
private:
    FSM<CloseTypeFSMComponent>* m_pFSM;
    float ChangeStateDistance = 20.0f;
public:
    void start();
    void update();

    FSM<CloseTypeFSMComponent>* GetFSM();
    bool CheckDistanceFromPlayer();
};

