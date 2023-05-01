#pragma once
#include "stdafx.h"
#include "Component.h"
class BillboardComponent : public Component
{
public:
    ~BillboardComponent() {}
    void start();
    void update();
};

