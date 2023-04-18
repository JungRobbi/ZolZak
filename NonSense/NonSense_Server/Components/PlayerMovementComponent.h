#pragma once
#include "../stdafx.h"
#include "Component.h"

class PlayerMovementComponent :
    public Component
{
    class Scene* scene;
public:
    void start();
    void update();
};

