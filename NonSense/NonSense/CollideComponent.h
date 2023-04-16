#pragma once
#include "Component.h"
#include "Object.h"

class BoundBox;
class CollideComponent : public Component
{
    BoundingOrientedBox m_BoundingBox;
    BoundBox* m_BoundingObject;
    bool MoveAble = false;
public:
    void start();
    void update();
    BoundBox* GetBoundingObject() { return m_BoundingObject; }
    BoundingOrientedBox GetBoundingBox() { return m_BoundingBox; }
    void SetBoundingObject(BoundBox* bd);
    void SetCenterExtents(XMFLOAT3 ct, XMFLOAT3 ex);
    void SetMoveAble(bool ma) { MoveAble = ma; }
};

