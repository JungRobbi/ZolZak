#pragma once
#include "../stdafx.h"
#include "Component.h"
#include "../Object.h"
#include "../Characters.h";
#include "../Timer.h"
class MonsterAttackComponent : public Component
{
public:

    bool During_Attack = false;
    bool WeaponFire = false;
    bool Rushing = false;
    XMFLOAT3 Direction = { 0.0f,0.0f,0.0f };

    int AttackAnimationNumber = 4;
    float AttackDuration = 3.0f;
    float AttackTimeLeft = 0.0;
    float RushTime = 0.0f;
    float TargetCoolTime = 1.0;
    bool Targetting = false;

    void FarTypeAttack();
    void RushTypeAttack();
    void SetAttackSpeed(float speed);
    void ResetWeapon();
    void TargetOn();
    void start();
    void update();

};

