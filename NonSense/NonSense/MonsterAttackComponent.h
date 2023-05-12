#pragma once
#include "stdafx.h"
#include "Component.h"
#include "Object.h"
#include "Characters.h";
#include "Timer.h"
class MonsterAttackComponent : public Component
{
public:

    bool During_Attack = false;
    bool WeaponFire = false;
    XMFLOAT3 Direction = { 0.0f,0.0f,0.0f };
    BoundBox* AttackRange = NULL;
    int AttackAnimationNumber = 4;
    float AttackDuration = 3.0f;
    float AttackTimeLeft = 0.0;


    void FarTypeAttack();
    void RushTypeAttack();
    void SetAttackSpeed(float speed);
    void ResetWeapon();

    void start();
    void update();
    void SetBoundingObject(BoundBox* bd) { AttackRange = bd; }
};

