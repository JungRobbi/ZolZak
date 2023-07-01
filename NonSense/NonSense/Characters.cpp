#include "Characters.h"
#include "BoxCollideComponent.h"
#include "GameScene.h"
#include "CloseTypeFSMComponent.h"
#include "FarTypeFSMComponent.h"
#include "RushTypeFSMComponent.h"
#include "MonsterAttackComponent.h"
#include "AttackComponent.h"
#include "MoveForwardComponent.h"
#include "RotateComponent.h"
#include "PlayerMovementComponent.h"
#include "SphereCollideComponent.h"

WeaponObject::WeaponObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel) : Object(DEFAULT_OBJECT)
{
	LoadedModelInfo* pLoadedModel = pModel;
	if (pLoadedModel)
	{
		SetChild(pLoadedModel->m_pRoot, true);
	}

	SetDo_Render(false);
	AddComponent<MoveForwardComponent>();
	AddComponent<RotateComponent>();
	GetComponent<RotateComponent>()->SetRotationSpeed(360.0f);
	GetComponent<RotateComponent>()->SetRotationAxis(XMFLOAT3(1.0f, 0.0f, 0.0f));

}

WeaponObject::~WeaponObject()
{
}

void WeaponObject::Fire(XMFLOAT3& look, XMFLOAT3& pos)
{
	SetPosition(pos);
	GetComponent<RotateComponent>()->RotateOn = true;
	GetComponent<MoveForwardComponent>()->ReadyToMove(look);
	SetDo_Render(true);
}



Character::Character(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel) :Object(false)
{
	LoadedModelInfo* pLoadedModel = pModel;
	if (pLoadedModel)
	{
		SetChild(pLoadedModel->m_pRoot, true);
	}
	m_pSkinnedAnimationController = new AnimationController(pd3dDevice, pd3dCommandList, 3, pModel);
	m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_pSkinnedAnimationController->SetTrackAnimationSet(1, 0);
	m_pSkinnedAnimationController->SetTrackAnimationSet(2, 0);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);

	m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[3]->m_nType = ANIMATION_TYPE_ONCE;
	m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[4]->m_nType = ANIMATION_TYPE_ONCE;
	m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[5]->m_nType = ANIMATION_TYPE_ONCE;
	m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[6]->m_nType = ANIMATION_TYPE_ONCE;

}

Character::~Character()
{
}

void Character::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	Object::Render(pd3dCommandList, pCamera);
}

Monster::Monster(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel) : Character(pd3dDevice,pd3dCommandList, pd3dGraphicsRootSignature, pModel)
{
	GameScene::MainScene->creationMonsterQueue.push((Monster*)this);
}

void Monster::OnPrepareRender()
{
	Object::OnPrepareRender();
	m_pHP->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(0, GetComponent<SphereCollideComponent>()->GetBoundingObject()->Radius * 2 + 0.3, 0)));
	m_pHP->HP = m_RemainHP / m_Health;
}
void Monster::FarTypeAttack()
{
	XMFLOAT3 Pos = GetPosition();
	Pos.y += 0.5f;
	((WeaponObject*)WeaponFrame)->Fire(GetLook(), Pos);
}
void Monster::RushTypeAttack()
{
}
Goblin::Goblin(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel, LoadedModelInfo* pWeaponL, LoadedModelInfo* pWeaponR, MonsterType type) : Monster(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pModel)
{
	Object* Hand = NULL;
	m_pBoundingShader = new BoundingShader();
	m_pBoundingShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);

	CubeMesh* m_pBoundMesh = new CubeMesh(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);
	SphereMesh* SphereMes = new SphereMesh(pd3dDevice, pd3dCommandList, 1.0f, 10, 10);
	BoundSphere* bs = new BoundSphere(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, SphereMes, m_pBoundingShader);
	BoundBox* bb = new BoundBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pBoundMesh, m_pBoundingShader);

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

		if (pWeaponL && pWeaponR) {
			Hand = FindFrame("Weapon_Goblin_2_R_Dummy");
			if (Hand) {
				Hand->SetChild(pWeaponR->m_pRoot, true);

			}
			Hand = FindFrame("Weapon_Goblin_2_L_Dummy");
			if (Hand) {
				Hand->SetChild(pWeaponL->m_pRoot, true);

			}
		}
		m_Health = 965;
		m_RemainHP = 965;
		m_Attack = 200;
		m_Defense = 90;
		{
			std::function<void()> AttackEvent = [this]() {
				this->CloseAttackEvent();
			};
			m_pSkinnedAnimationController->AddAnimationEvent("AttackEvent", E_M_ATTACK, 0.6, AttackEvent);
		}
		break;
	case MONSTER_TYPE_FAR:

		if (pWeaponL && pWeaponR) {
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
				WeaponFrame->GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.0, -0.05), 7);
			}
			Hand = FindFrame("Weapon_Goblin_3_L_Dummy");
			if (Hand) {
				Hand->SetChild(pWeaponL->m_pRoot, true);
			}
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

	Monster_HP_UI* m_HP_UI = new Monster_HP_UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pHP = m_HP_UI;

	
	
}

Goblin::~Goblin()
{
}


void Goblin::CloseAttackEvent()
{
	GetComponent<AttackComponent>()->CheckMonsterAttackRange();
}

NPC::NPC(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel) : Character(pd3dDevice,  pd3dCommandList, pd3dGraphicsRootSignature, pModel)
{
	GameScene::MainScene->creationQueue.push((Object*)this);

	m_pBoundingShader = new BoundingShader();
	m_pBoundingShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);

	SphereMesh* SphereMes = new SphereMesh(pd3dDevice, pd3dCommandList, 1.0f, 10, 10);
	BoundSphere* bs = new BoundSphere(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, SphereMes, m_pBoundingShader);

	bs->SetNum(2);
	AddComponent<SphereCollideComponent>();
	GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
	GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 3);
	
}
