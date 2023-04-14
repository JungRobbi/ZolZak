#pragma once
#include "stdafx.h"
#include "Timer.h"
#include "Component.h"
class PlayerMovementComponent : public Component
{
public:
    bool CursorExpose;
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
private:
    void Jump();
    void Dash();
public:
    void SetWindowPos(RECT pos);

    void start();
    void update();
};