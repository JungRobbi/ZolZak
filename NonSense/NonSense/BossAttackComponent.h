#pragma once
#include "Component.h"
#include "Timer.h"
#include "Object.h"

class BossAttackComponent : public Component
{
public:
	bool During_Attack = false;
	BoundBox* AttackRange = NULL;
	bool During_Skill = false;
	bool End_Skill = false;
private:
	float AttackDuration = 2.0;
	float AttackTimeLeft = 0.0;
	float DefenceTimeLeft = 0;
public:
	~BossAttackComponent() {}
	void SetBoundingObject(BoundBox* bd) { AttackRange = bd; }
	void start();
	void update();
	void SetAttackSpeed(float speed);
	void SetAttackDuration(float d) { AttackDuration = d; }

	void AttackAnimation();
	void Attack();

	void StealSenseAnimation();
	void StealSense();

	void SummonAnimation();
	void Summon(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,XMFLOAT3 Pos);
	
	void DefenceAnimation();
	void Defence();

	void JumpAttackAnimation();
	void JumpAttack();

	void TornadoAnimation();
	void Tornado();
};