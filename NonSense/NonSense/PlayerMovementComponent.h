#pragma once
#include "stdafx.h"
#include "Component.h"
class PlayerMovementComponent :
    public Component
{

public:
    void start();
    void update();
};