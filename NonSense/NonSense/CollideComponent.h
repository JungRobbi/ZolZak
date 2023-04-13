#pragma once
#include "Component.h"
#include "Object.h"

class BoundBox;
class CollideComponent : public Component
{
    BoundingOrientedBox m_BoundingBox;
    BoundBox* m_BoundingObject;
    XMFLOAT3 m_Offset;
public:
    void start();
    void update();
    void SetExtents(XMFLOAT3 extents);
    BoundBox* GetBoundingObject() { return m_BoundingObject; }
    void SetBoundingObject(BoundBox* bd) { m_BoundingObject = bd; }
    void SetOffset(XMFLOAT3 os) { m_Offset = os; }
};

