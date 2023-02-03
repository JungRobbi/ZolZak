#pragma once
#include "stdafx.h"
#include "Component.h"

class RotateComponent :
    public Component
{
    XMFLOAT3 m_xmf3RotationAxis;
    float m_fRotationSpeed;

public:
    void start();
    void update();

    void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
    void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
};

