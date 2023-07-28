#include <functional>
#include "Characters.h"
#include "Scene.h"
#include "Room.h"
#include "Components/BoxCollideComponent.h"
#include "Components/CloseTypeFSMComponent.h"
#include "Components/AttackComponent.h"
#include "Components/FarTypeFSMComponent.h"
#include "Components/RushTypeFSMComponent.h"
#include "Components/MonsterAttackComponent.h"
#include "Components/AttackComponent.h"
#include "Components/MoveForwardComponent.h"
#include "Components/RotateComponent.h"
#include "Components/PlayerMovementComponent.h"
#include "Components/SphereCollideComponent.h"
#include "Components/BossAttackComponent.h"
#include "Components/BossFSMComponent.h"
#include "Timer.h"
Character::Character() :
	Object(false)
{
	Scene::scene->creationMonsterQueue.push((Character*)this);
}

Character::Character(int roomNum) :
	Object(false)
{
	Room::roomlist.at(roomNum)->GetScene()->creationMonsterQueue.push((Character*)this);
}

Character::Character(std::shared_ptr<Scene> pScene)
{
	pScene->creationMonsterQueue.push((Character*)this);
}


Character::~Character() 
{
	for (auto& p : components)
		delete p;
	components.clear();
}


Monster::Monster() : Character()
{
}

Monster::Monster(int roomNum) : Character(roomNum)
{
}

Monster::Monster(std::shared_ptr<Scene> pScene) : Character(pScene)
{
}

void Monster::FarTypeAttack()
{
	XMFLOAT3 Pos = GetPosition();
	Pos.y += 0.5f;
//	((WeaponObject*)WeaponFrame)->Fire(GetLook(), Pos);
}
void Monster::RushTypeAttack()
{
}


Goblin::Goblin(MonsterType type, int roomNum) :
	Monster(roomNum)
{
	m_type = type;
	BoundBox* bb = new BoundBox();
	BoundBox* bb2 = new BoundBox();
	BoundSphere* bs = new BoundSphere();
	switch (type)
	{
	case MONSTER_TYPE_CLOSE:
		AddComponent<CloseTypeFSMComponent>();
		AddComponent<AttackComponent>();
		GetComponent<AttackComponent>()->SetAttackSpeed(3.0f);
		GetComponent<AttackComponent>()->AttackCombo1_AnineSetNum = 4;
		GetComponent<AttackComponent>()->Type_ComboAttack = false;
		GetComponent<AttackComponent>()->SetBoundingObject(bb2);

		AddComponent<BoxCollideComponent>();
		GetComponent<BoxCollideComponent>()->SetBoundingObject(bb);
		GetComponent<BoxCollideComponent>()->SetCenterExtents(XMFLOAT3(0.0, 0.5, 0.0), XMFLOAT3(0.3, 0.5, 0.3));
		GetComponent<BoxCollideComponent>()->SetMoveAble(true);

		m_Health = 965;
		m_RemainHP = 965;
		m_Attack = 200;
		m_Defense = 90;
		break;
	case MONSTER_TYPE_FAR:

		/*if (pWeaponL && pWeaponR) {
			Hand = FindFrame("Weapon_Goblin_3_R_Dummy");
			if (Hand) {
				Hand->SetChild(pWeaponR->m_pRoot, true);
				HandFrame = Hand;
				LoadedModelInfo* WeaponModel = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Goblin_Far_Weapon_R.bin", NULL);
				WeaponFrame = new WeaponObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, WeaponModel);
				WeaponFrame->SetScale(0.1f, 0.1f, 0.1f);
				BoundingShader* m_pBoundingShader = new BoundingShader();
				m_pBoundingShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
				SphereMesh* SphereMes = new SphereMesh(pd3dDevice, pd3dCommandList, 1.0f, 10, 10);
				BoundSphere* bs2 = new BoundSphere(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, SphereMes, m_pBoundingShader);
				bs2->SetNum(5);
				WeaponFrame->AddComponent<SphereCollideComponent>();
				WeaponFrame->GetComponent<SphereCollideComponent>()->SetBoundingObject(bs2);
				WeaponFrame->GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.0, -0.05), 10);
			}
			Hand = FindFrame("Weapon_Goblin_3_L_Dummy");
			if (Hand) {
				Hand->SetChild(pWeaponL->m_pRoot, true);
			}
		}*/
		AddComponent<MonsterAttackComponent>();
		GetComponent<MonsterAttackComponent>()->SetAttackSpeed(3.0f);

		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		AddComponent<FarTypeFSMComponent>();

		m_Health = 675;
		m_RemainHP = 675;
		m_Attack = 180;
		m_Defense = 80;
		break;
	case MONSTER_TYPE_RUSH:
		AddComponent<RushTypeFSMComponent>();
		AddComponent<MonsterAttackComponent>();
		GetComponent<MonsterAttackComponent>()->SetAttackSpeed(5.0f);
		GetComponent<MonsterAttackComponent>()->AttackAnimationNumber = 3;

		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		m_Health = 1130;
		m_RemainHP = 1130;
		m_Attack = 460;
		m_Defense = 110;
		break;
	case MONSTER_TYPE_BOSS:
		m_Health = 20000;
		m_RemainHP = 20000;
		m_Attack = 200;
		m_Defense = 90;
		break;
	default:
		break;
	}
}

Goblin::Goblin(MonsterType type, std::shared_ptr<Scene> pScene) :
	Monster(pScene)
{
	m_type = type;
	BoundBox* bb = new BoundBox();
	BoundBox* bb2 = new BoundBox();
	BoundSphere* bs = new BoundSphere();
	switch (type)
	{
	case MONSTER_TYPE_CLOSE:
		AddComponent<CloseTypeFSMComponent>();
		AddComponent<AttackComponent>();
		GetComponent<AttackComponent>()->SetAttackSpeed(3.0f);
		GetComponent<AttackComponent>()->AttackCombo1_AnineSetNum = 4;
		GetComponent<AttackComponent>()->Type_ComboAttack = false;
		GetComponent<AttackComponent>()->SetBoundingObject(bb2);

		AddComponent<BoxCollideComponent>();
		GetComponent<BoxCollideComponent>()->SetBoundingObject(bb);
		GetComponent<BoxCollideComponent>()->SetCenterExtents(XMFLOAT3(0.0, 0.5, 0.0), XMFLOAT3(0.3, 0.5, 0.3));
		GetComponent<BoxCollideComponent>()->SetMoveAble(true);

		m_Health = 965;
		m_RemainHP = 965;
		m_Attack = 200;
		m_Defense = 90;
		break;
	case MONSTER_TYPE_FAR:

		/*if (pWeaponL && pWeaponR) {
			Hand = FindFrame("Weapon_Goblin_3_R_Dummy");
			if (Hand) {
				Hand->SetChild(pWeaponR->m_pRoot, true);
				HandFrame = Hand;
				LoadedModelInfo* WeaponModel = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Goblin_Far_Weapon_R.bin", NULL);
				WeaponFrame = new WeaponObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, WeaponModel);
				WeaponFrame->SetScale(0.1f, 0.1f, 0.1f);
				BoundingShader* m_pBoundingShader = new BoundingShader();
				m_pBoundingShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
				SphereMesh* SphereMes = new SphereMesh(pd3dDevice, pd3dCommandList, 1.0f, 10, 10);
				BoundSphere* bs2 = new BoundSphere(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, SphereMes, m_pBoundingShader);
				bs2->SetNum(5);
				WeaponFrame->AddComponent<SphereCollideComponent>();
				WeaponFrame->GetComponent<SphereCollideComponent>()->SetBoundingObject(bs2);
				WeaponFrame->GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.0, -0.05), 10);
			}
			Hand = FindFrame("Weapon_Goblin_3_L_Dummy");
			if (Hand) {
				Hand->SetChild(pWeaponL->m_pRoot, true);
			}
		}*/
		AddComponent<MonsterAttackComponent>();
		GetComponent<MonsterAttackComponent>()->SetAttackSpeed(3.0f);

		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		AddComponent<FarTypeFSMComponent>();

		m_Health = 675;
		m_RemainHP = 675;
		m_Attack = 180;
		m_Defense = 80;
		break;
	case MONSTER_TYPE_RUSH:
		AddComponent<RushTypeFSMComponent>();
		AddComponent<MonsterAttackComponent>();
		GetComponent<MonsterAttackComponent>()->SetAttackSpeed(5.0f);
		GetComponent<MonsterAttackComponent>()->AttackAnimationNumber = 3;

		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		m_Health = 1130;
		m_RemainHP = 1130;
		m_Attack = 460;
		m_Defense = 110;
		break;
	case MONSTER_TYPE_BOSS:
		m_Health = 20000;
		m_RemainHP = 20000;
		m_Attack = 200;
		m_Defense = 90;
		break;
	default:
		break;
	}
}

Goblin::~Goblin()
{
	for (auto& p : components)
		delete p;
	components.clear();
}


Orc::Orc(MonsterType type, int roomNum) : Monster(roomNum)
{
	m_type = type;
	BoundBox* bb = new BoundBox();
	BoundSphere* bs = new BoundSphere();
	switch (type)
	{
	case MONSTER_TYPE_CLOSE:

		AddComponent<CloseTypeFSMComponent>();
		bb->SetNum(5);
		AddComponent<AttackComponent>();
		GetComponent<AttackComponent>()->SetAttackSpeed(3.0f);
		GetComponent<AttackComponent>()->AttackCombo1_AnineSetNum = 4;
		GetComponent<AttackComponent>()->Type_ComboAttack = false;
		GetComponent<AttackComponent>()->SetBoundingObject(bb);

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		m_Health = 965;
		m_RemainHP = 965;
		m_Attack = 200;
		m_Defense = 90;
		
		break;
	case MONSTER_TYPE_FAR:
	{
		/*BoundSphere* bs2 = new BoundSphere();
		bs2->SetNum(5);
		WeaponFrame->AddComponent<SphereCollideComponent>();
		WeaponFrame->GetComponent<SphereCollideComponent>()->SetBoundingObject(bs2);
		WeaponFrame->GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.0, -0.05), 7);*/

		AddComponent<MonsterAttackComponent>();
		GetComponent<MonsterAttackComponent>()->SetAttackSpeed(3.0f);

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		AddComponent<FarTypeFSMComponent>();

		m_Health = 675;
		m_RemainHP = 675;
		m_Attack = 180;
		m_Defense = 80;
		break;
	}
	case MONSTER_TYPE_RUSH:
		AddComponent<RushTypeFSMComponent>();
		AddComponent<MonsterAttackComponent>();
		GetComponent<MonsterAttackComponent>()->SetAttackSpeed(5.0f);
		GetComponent<MonsterAttackComponent>()->AttackAnimationNumber = 3;

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		m_Health = 1130;
		m_RemainHP = 1130;
		m_Attack = 460;
		m_Defense = 110;
		break;
	case MONSTER_TYPE_BOSS:
		m_Health = 20000;
		m_RemainHP = 20000;
		m_Attack = 200;
		m_Defense = 90;
		break;
	default:
		break;
	}
}

Orc::Orc(MonsterType type, std::shared_ptr<Scene> pScene) : Monster(pScene)
{
	m_type = type;
	BoundBox* bb = new BoundBox();
	BoundSphere* bs = new BoundSphere();
	switch (type)
	{
	case MONSTER_TYPE_CLOSE:

		AddComponent<CloseTypeFSMComponent>();
		bb->SetNum(5);
		AddComponent<AttackComponent>();
		GetComponent<AttackComponent>()->SetAttackSpeed(3.0f);
		GetComponent<AttackComponent>()->AttackCombo1_AnineSetNum = 4;
		GetComponent<AttackComponent>()->Type_ComboAttack = false;
		GetComponent<AttackComponent>()->SetBoundingObject(bb);

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		m_Health = 965;
		m_RemainHP = 965;
		m_Attack = 200;
		m_Defense = 90;

		break;
	case MONSTER_TYPE_FAR:
	{
		/*BoundSphere* bs2 = new BoundSphere();
		bs2->SetNum(5);
		WeaponFrame->AddComponent<SphereCollideComponent>();
		WeaponFrame->GetComponent<SphereCollideComponent>()->SetBoundingObject(bs2);
		WeaponFrame->GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.0, -0.05), 7);*/

		AddComponent<MonsterAttackComponent>();
		GetComponent<MonsterAttackComponent>()->SetAttackSpeed(3.0f);

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		AddComponent<FarTypeFSMComponent>();

		m_Health = 675;
		m_RemainHP = 675;
		m_Attack = 180;
		m_Defense = 80;
		break;
	}
	case MONSTER_TYPE_RUSH:
		AddComponent<RushTypeFSMComponent>();
		AddComponent<MonsterAttackComponent>();
		GetComponent<MonsterAttackComponent>()->SetAttackSpeed(5.0f);
		GetComponent<MonsterAttackComponent>()->AttackAnimationNumber = 3;

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		m_Health = 1130;
		m_RemainHP = 1130;
		m_Attack = 460;
		m_Defense = 110;
		break;
	case MONSTER_TYPE_BOSS:
		m_Health = 20000;
		m_RemainHP = 20000;
		m_Attack = 200;
		m_Defense = 90;
		break;
	default:
		break;
	}
}

Orc::~Orc()
{
}

void Orc::CloseAttackEvent()
{
	GetComponent<AttackComponent>()->CheckMonsterAttackRange();
}
Skull::Skull(MonsterType type, int roomNum) : Monster(roomNum)
{
	Object* Hand = NULL;
	m_type = type;
	BoundSphere* bs = new BoundSphere();
	BoundBox* bb = new BoundBox();

	switch (type)
	{
	case MONSTER_TYPE_CLOSE:

		AddComponent<CloseTypeFSMComponent>();
		bb->SetNum(5);
		AddComponent<AttackComponent>();
		GetComponent<AttackComponent>()->SetAttackSpeed(3.0f);
		GetComponent<AttackComponent>()->AttackCombo1_AnineSetNum = 4;
		GetComponent<AttackComponent>()->Type_ComboAttack = false;
		GetComponent<AttackComponent>()->SetBoundingObject(bb);

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);


		m_Health = 965;
		m_RemainHP = 965;
		m_Attack = 200;
		m_Defense = 90;
	
		break;
	case MONSTER_TYPE_FAR:
	{
		
		BoundSphere* bs2 = new BoundSphere();
		bs2->SetNum(5);
		WeaponFrame->AddComponent<SphereCollideComponent>();
		WeaponFrame->GetComponent<SphereCollideComponent>()->SetBoundingObject(bs2);
		WeaponFrame->GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.0, -0.05), 7);
	}
	AddComponent<MonsterAttackComponent>();
	GetComponent<MonsterAttackComponent>()->SetAttackSpeed(3.0f);

	bs->SetNum(2);
	AddComponent<SphereCollideComponent>();
	GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
	GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

	AddComponent<FarTypeFSMComponent>();

	m_Health = 675;
	m_RemainHP = 675;
	m_Attack = 180;
	m_Defense = 80;
	break;
	case MONSTER_TYPE_RUSH:
		AddComponent<RushTypeFSMComponent>();
		AddComponent<MonsterAttackComponent>();
		GetComponent<MonsterAttackComponent>()->SetAttackSpeed(5.0f);
		GetComponent<MonsterAttackComponent>()->AttackAnimationNumber = 3;

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		m_Health = 1130;
		m_RemainHP = 1130;
		m_Attack = 460;
		m_Defense = 110;
		break;
	case MONSTER_TYPE_BOSS:
		m_Health = 20000;
		m_RemainHP = 20000;
		m_Attack = 200;
		m_Defense = 90;
		break;
	default:
		break;
	}
}

Skull::Skull(MonsterType type, std::shared_ptr<Scene> pScene) :
	Monster(pScene)
{
	Object* Hand = NULL;
	m_type = type;
	BoundSphere* bs = new BoundSphere();
	BoundBox* bb = new BoundBox();

	switch (type)
	{
	case MONSTER_TYPE_CLOSE:

		AddComponent<CloseTypeFSMComponent>();
		bb->SetNum(5);
		AddComponent<AttackComponent>();
		GetComponent<AttackComponent>()->SetAttackSpeed(3.0f);
		GetComponent<AttackComponent>()->AttackCombo1_AnineSetNum = 4;
		GetComponent<AttackComponent>()->Type_ComboAttack = false;
		GetComponent<AttackComponent>()->SetBoundingObject(bb);

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);


		m_Health = 965;
		m_RemainHP = 965;
		m_Attack = 200;
		m_Defense = 90;

		break;
	case MONSTER_TYPE_FAR:
	{

		BoundSphere* bs2 = new BoundSphere();
		bs2->SetNum(5);
		WeaponFrame->AddComponent<SphereCollideComponent>();
		WeaponFrame->GetComponent<SphereCollideComponent>()->SetBoundingObject(bs2);
		WeaponFrame->GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.0, -0.05), 7);
	}
	AddComponent<MonsterAttackComponent>();
	GetComponent<MonsterAttackComponent>()->SetAttackSpeed(3.0f);

	bs->SetNum(2);
	AddComponent<SphereCollideComponent>();
	GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
	GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

	AddComponent<FarTypeFSMComponent>();

	m_Health = 675;
	m_RemainHP = 675;
	m_Attack = 180;
	m_Defense = 80;
	break;
	case MONSTER_TYPE_RUSH:
		AddComponent<RushTypeFSMComponent>();
		AddComponent<MonsterAttackComponent>();
		GetComponent<MonsterAttackComponent>()->SetAttackSpeed(5.0f);
		GetComponent<MonsterAttackComponent>()->AttackAnimationNumber = 3;

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		m_Health = 1130;
		m_RemainHP = 1130;
		m_Attack = 460;
		m_Defense = 110;
		break;
	case MONSTER_TYPE_BOSS:
		m_Health = 20000;
		m_RemainHP = 20000;
		m_Attack = 200;
		m_Defense = 90;
		break;
	default:
		break;
	}
}

Skull::~Skull()
{
}
void Skull::CloseAttackEvent()
{
	GetComponent<AttackComponent>()->CheckMonsterAttackRange();
}

Shield::Shield(MonsterType type, int roomNum) : Monster(roomNum)
{
	m_type = type;
	BoundSphere* bs = new BoundSphere();
	BoundBox* bb = new BoundBox();
	switch (type)
	{
	case MONSTER_TYPE_CLOSE:
		AddComponent<CloseTypeFSMComponent>();
		bb->SetNum(5);
		AddComponent<AttackComponent>();
		GetComponent<AttackComponent>()->SetAttackSpeed(3.0f);
		GetComponent<AttackComponent>()->AttackCombo1_AnineSetNum = 4;
		GetComponent<AttackComponent>()->Type_ComboAttack = false;
		GetComponent<AttackComponent>()->SetBoundingObject(bb);

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		m_Health = 965;
		m_RemainHP = 965;
		m_Attack = 200;
		m_Defense = 90;

		break;
	case MONSTER_TYPE_FAR:
		AddComponent<MonsterAttackComponent>();
		GetComponent<MonsterAttackComponent>()->SetAttackSpeed(3.0f);

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		AddComponent<FarTypeFSMComponent>();

		m_Health = 675;
		m_RemainHP = 675;
		m_Attack = 180;
		m_Defense = 80;
		break;
	case MONSTER_TYPE_RUSH:
		AddComponent<RushTypeFSMComponent>();
		AddComponent<MonsterAttackComponent>();
		GetComponent<MonsterAttackComponent>()->SetAttackSpeed(5.0f);
		GetComponent<MonsterAttackComponent>()->AttackAnimationNumber = 3;

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		m_Health = 1130;
		m_RemainHP = 1130;
		m_Attack = 460;
		m_Defense = 110;
		break;
	case MONSTER_TYPE_BOSS:
		AddComponent<BossFSMComponent>();
		AddComponent<BossAttackComponent>();
		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		m_Health = 20000;
		m_RemainHP = 20000;
		m_Attack = 200;
		m_Defense = 90;
		//{
		//	std::function<void()>EndEvent = [this]() {
		//		this->EndSkillEvent();
		//	};
		//	{
		//		std::function<void()> AttackEvent = [this]() {
		//			this->BossAttackEvent();
		//		};
		//		m_pSkinnedAnimationController->AddAnimationEvent("AttackEvent", E_B_ATTACK, 0.6, AttackEvent);
		//	}
		//	{
		//		std::function<void()> StealSenseEvent = [this]() {
		//			this->BossStealSenseEvent();
		//		};
		//		m_pSkinnedAnimationController->AddAnimationEvent("StealSenseEvent", E_B_ROAR, 1.5, StealSenseEvent);

		//		float len = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[E_B_ROAR]->m_Length - 0.1;
		//		m_pSkinnedAnimationController->AddAnimationEvent("EndEvent", E_B_ROAR, len, EndEvent);
		//	}
		//	{
		//		std::function<void()> SummonEvent = [this, pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature]() {
		//			this->BossSummonEvent(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		//		};
		//		m_pSkinnedAnimationController->AddAnimationEvent("SummonEvent", E_B_SUMMON, 1.5, SummonEvent);

		//		float len = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[E_B_SUMMON]->m_Length - 0.1;
		//		m_pSkinnedAnimationController->AddAnimationEvent("EndEvent", E_B_SUMMON, len, EndEvent);
		//	}
		//	{
		//		std::function<void()> DefenceEvent = [this]() {
		//			this->BossDefenceEvent();
		//		};
		//		m_pSkinnedAnimationController->AddAnimationEvent("DefenceEvent", E_B_DEFENCE, 0.6, DefenceEvent);

		//		float len = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[E_B_DEFENCE]->m_Length - 0.1;
		//		m_pSkinnedAnimationController->AddAnimationEvent("EndEvent", E_B_DEFENCE, len, EndEvent);
		//	}
		//	{
		//		std::function<void()> JumpAttackEvent = [this]() {
		//			this->BossJumpAttackEvent();
		//		};
		//		m_pSkinnedAnimationController->AddAnimationEvent("JumpAttackEvent", E_B_JUMPATTACK, 2.3, JumpAttackEvent);

		//		float len = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[E_B_JUMPATTACK]->m_Length - 0.1;
		//		m_pSkinnedAnimationController->AddAnimationEvent("EndEvent", E_B_JUMPATTACK, len, EndEvent);
		//	}
		//	{
		//		std::function<void()> ToranodoEvent = [this]() {
		//			this->BossTorandoEvent();
		//		};
		//		m_pSkinnedAnimationController->AddAnimationEvent("ToranodoEvent", E_B_TORNADO, 0.6, ToranodoEvent);
		//		m_pSkinnedAnimationController->AddAnimationEvent("ToranodoEvent", E_B_TORNADO, 1.2, ToranodoEvent);
		//		m_pSkinnedAnimationController->AddAnimationEvent("ToranodoEvent", E_B_TORNADO, 1.8, ToranodoEvent);
		//		m_pSkinnedAnimationController->AddAnimationEvent("ToranodoEvent", E_B_TORNADO, 2.4, ToranodoEvent);
		//		m_pSkinnedAnimationController->AddAnimationEvent("ToranodoEvent", E_B_TORNADO, 3.0, ToranodoEvent);
		//		m_pSkinnedAnimationController->AddAnimationEvent("ToranodoEvent", E_B_TORNADO, 3.6, ToranodoEvent);
		//		m_pSkinnedAnimationController->AddAnimationEvent("ToranodoEvent", E_B_TORNADO, 4.2, ToranodoEvent);
		//		m_pSkinnedAnimationController->AddAnimationEvent("ToranodoEvent", E_B_TORNADO, 4.8, ToranodoEvent);
		//		m_pSkinnedAnimationController->AddAnimationEvent("ToranodoEvent", E_B_TORNADO, 5.4, ToranodoEvent);
		//		m_pSkinnedAnimationController->AddAnimationEvent("ToranodoEvent", E_B_TORNADO, 6.0, ToranodoEvent);
		//		float len = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[E_B_TORNADO]->m_Length - 0.1;
		//		m_pSkinnedAnimationController->AddAnimationEvent("EndEvent", E_B_TORNADO, len, EndEvent);
		//	}
		//} // 스킬 이벤트 // Boss Skill Event

		break;
	default:
		break;
	}
}

Shield::Shield(MonsterType type, std::shared_ptr<Scene> pScene) :
	Monster(pScene)
{
	m_type = type;
	BoundSphere* bs = new BoundSphere();
	BoundBox* bb = new BoundBox();
	switch (type)
	{
	case MONSTER_TYPE_CLOSE:
		AddComponent<CloseTypeFSMComponent>();
		bb->SetNum(5);
		AddComponent<AttackComponent>();
		GetComponent<AttackComponent>()->SetAttackSpeed(3.0f);
		GetComponent<AttackComponent>()->AttackCombo1_AnineSetNum = 4;
		GetComponent<AttackComponent>()->Type_ComboAttack = false;
		GetComponent<AttackComponent>()->SetBoundingObject(bb);

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		m_Health = 965;
		m_RemainHP = 965;
		m_Attack = 200;
		m_Defense = 90;

		break;
	case MONSTER_TYPE_FAR:
		AddComponent<MonsterAttackComponent>();
		GetComponent<MonsterAttackComponent>()->SetAttackSpeed(3.0f);

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		AddComponent<FarTypeFSMComponent>();

		m_Health = 675;
		m_RemainHP = 675;
		m_Attack = 180;
		m_Defense = 80;
		break;
	case MONSTER_TYPE_RUSH:
		AddComponent<RushTypeFSMComponent>();
		AddComponent<MonsterAttackComponent>();
		GetComponent<MonsterAttackComponent>()->SetAttackSpeed(5.0f);
		GetComponent<MonsterAttackComponent>()->AttackAnimationNumber = 3;

		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		m_Health = 1130;
		m_RemainHP = 1130;
		m_Attack = 460;
		m_Defense = 110;
		break;
	case MONSTER_TYPE_BOSS:
		AddComponent<BossFSMComponent>();
		AddComponent<BossAttackComponent>();
		bs->SetNum(2);
		AddComponent<SphereCollideComponent>();
		GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
		GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.5);

		m_Health = 20000;
		m_RemainHP = 20000;
		m_Attack = 200;
		m_Defense = 90;

		break;
	default:
		break;
	}
}

Shield::~Shield()
{
}

void Shield::BossAttackEvent()
{
	PresentAniType = E_BOSS_ANIMATION_TYPE::E_B_ATTACK;
//	GetComponent<BossAttackComponent>()->Attack();
	EndSkillEvent();
}

void Shield::BossStealSenseEvent()
{
	PresentAniType = E_BOSS_ANIMATION_TYPE::E_B_ROAR;
//	GetComponent<BossAttackComponent>()->StealSense();
	EndSkillEvent();
}

void Shield::BossSummonEvent()
{
	PresentAniType = E_BOSS_ANIMATION_TYPE::E_B_SUMMON;
//	GetComponent<BossAttackComponent>()->Summon(GetPosition());
	EndSkillEvent();
}

void Shield::BossDefenceEvent()
{
	PresentAniType = E_BOSS_ANIMATION_TYPE::E_B_DEFENCE;
//	GetComponent<BossAttackComponent>()->Defence();
	EndSkillEvent();
}

void Shield::BossJumpAttackEvent()
{
	PresentAniType = E_BOSS_ANIMATION_TYPE::E_B_JUMPATTACK;
//	GetComponent<BossAttackComponent>()->JumpAttack();
	EndSkillEvent();
}

void Shield::BossTorandoEvent()
{
	PresentAniType = E_BOSS_ANIMATION_TYPE::E_B_TORNADO;
//	GetComponent<BossAttackComponent>()->Tornado();
	EndSkillEvent();
}

void Shield::EndSkillEvent()
{
	std::cout << "EndSkillEvent" << std::endl;
	GetComponent<BossAttackComponent>()->End_Skill = true;
}