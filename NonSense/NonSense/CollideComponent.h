#pragma once
#include "Component.h"
#include "Object.h"

class BoundBox;
class CollideComponent : public Component
{
    BoundingOrientedBox m_BoundingBox;
    BoundBox* m_BoundingObject;
public:
    void start();
    void update();
    BoundBox* GetBoundingObject() { return m_BoundingObject; }
    void SetBoundingObject(BoundBox* bd);
    void SetExtents(XMFLOAT3 ex);
};

