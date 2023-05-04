#pragma once
#include "stdafx.h"
#include "Timer.h"
#include "Component.h"
#include "../AnimationType.h"
#include "NetworkMGR.h"
class PlayerMovementComponent : public Component
{
public:
    bool CursorExpose;
    E_PLAYER_ANIMATION_TYPE Animation_type = E_PLAYER_ANIMATION_TYPE::E_IDLE;
private:
    POINT ptCursorPos;
    POINT CenterOfWindow;
    RECT WindowPos;

    float CursorCoolTime = 1.0f;
    float MaxCursorCoolTime = 1.0f;

    float DashDistance = 3.5f;
    float DashDuration = 0.5f;
    float DashTimeLeft = 0.0f;
    float DashCoolTime = 5.0f;
    float DashCoolTimeLeft = 0.0f;
    bool Dashing = false;
    bool CanDash = true;

    bool b_Jump = false;
    bool b_Dash = false;

private:
    void Jump();
    void Dash();
public:
    ~PlayerMovementComponent() {}
    void SetWindowPos(RECT pos);

    void start();
    void update();

    friend NetworkMGR;
};