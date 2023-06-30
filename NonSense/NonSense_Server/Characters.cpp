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