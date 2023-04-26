#pragma once
#include "Component.h"
#include "../Object.h"

class BoundBox;

class BoxCollideComponent : public Component
{
    BoundBox* m_BoundingObject = NULL;
    bool MoveAble = false;
    XMFLOAT3 Center = { 0,0,0 };
    XMFLOAT3 Extents = { 0,0,0 };
public:
    void start();
    void update();

    BoundBox* GetBoundingObject() { if (m_BoundingObject) return m_BoundingObject; }
    void SetBoundingObject(BoundBox* bd) { m_BoundingObject = bd; }
    void SetCenterExtents(XMFLOAT3 ct, XMFLOAT3 ex);
    void SetMoveAble(bool ma) { MoveAble = ma; }
};

