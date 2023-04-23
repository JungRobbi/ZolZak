#pragma once
#include "Component.h"
#include "Object.h"

class BoundSphere;

class SphereCollideComponent : public Component
{
    BoundSphere* m_BoundingObject = NULL;
    XMFLOAT3 Center = { 0,0,0 };
    float Radius = 1.f;
public:
    void start();
    void update();

    BoundSphere* GetBoundingObject() { if (m_BoundingObject) return m_BoundingObject; }
    void SetBoundingObject(BoundSphere* bd) { m_BoundingObject = bd; }
    void SetCenterRadius(XMFLOAT3 ct, float rs);
};

