#pragma once
#include "Component.h"

class MonsterAttackComponent : public Component
{
public:

    bool During_Attack;


    void Attack_Far();


    void start();
    void update();
};

