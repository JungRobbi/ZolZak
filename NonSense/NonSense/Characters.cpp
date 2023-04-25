#include "Characters.h"
#include "BoxCollideComponent.h"
#include "GameScene.h"
#include "CloseTypeFSMComponent.h"
#include "AttackComponent.h"


Character::Character(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel) :
	Object(false)
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

	GameScene::MainScene->creationMonsterQueue.push((Character*)this);
}
Goblin::Goblin(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel, LoadedModelInfo* pWeaponL, LoadedModelInfo* pWeaponR, MonsterType type) :
	Character(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pModel)
{
	m_pBoundingShader = new BoundingShader();
	m_pBoundingShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	m_pBoundMesh = new CubeMesh(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);
	BoundBox* bb = new BoundBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pBoundMesh, m_pBoundingShader);
	BoundBox* bb2 = new BoundBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pBoundMesh, m_pBoundingShader);
	switch (type)
	{
	case MONSTER_TYPE_CLOSE:
		AddComponent<CloseTypeFSMComponent>();

		bb2->SetNum(5);
		AddComponent<AttackComponent>();
		GetComponent<AttackComponent>()->SetAttackSpeed(3.0f);
		GetComponent<AttackComponent>()->AttackCombo1_AnineSetNum = 4;
		GetComponent<AttackComponent>()->Type_ComboAttack = false;
		GetComponent<AttackComponent>()->SetBoundingObject(bb2);

		bb->SetNum(2);
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
		m_Health = 675;
		m_RemainHP = 675;
		m_Attack = 180;
		m_Defense = 80;
		break;
	case MONSTER_TYPE_RUSH:
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

	if (pWeaponL && pWeaponR) {
		Object* Hand = FindFrame("Weapon_Goblin_2_R_Dummy");
		if (Hand) {
			Hand->SetChild(pWeaponR->m_pRoot, true);

		}
		Hand = FindFrame("Weapon_Goblin_2_L_Dummy");
		if (Hand) {
			Hand->SetChild(pWeaponL->m_pRoot, true);

		}
	}

}

void Character::OnPrepareRender()
{
	Object::OnPrepareRender();
	m_pHP->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(0, GetComponent<BoxCollideComponent>()->GetBoundingObject()->Extents.y * 2 + 0.3, 0)));
	m_pHP->HP = m_RemainHP / m_Health;
}