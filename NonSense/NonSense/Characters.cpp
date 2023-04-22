#include "Characters.h"
#include "CloseTypeFSMComponent.h"



Character::Character(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel) :
	ModelObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pModel)
{

	m_pSkinnedAnimationController = new AnimationController(pd3dDevice, pd3dCommandList, 3, pModel);
	m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_pSkinnedAnimationController->SetTrackAnimationSet(1, 0);
	m_pSkinnedAnimationController->SetTrackAnimationSet(2, 0);
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);

}
Goblin::Goblin(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel, LoadedModelInfo* pWeaponL, LoadedModelInfo* pWeaponR, MonsterType type) :
	Character(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pModel)
{
	switch (type)
	{
	case MONSTER_TYPE_CLOSE:
		AddComponent<CloseTypeFSMComponent>();
		m_Health = 965;
		m_HP = 965;
		m_Attack = 200;
		m_Defense = 90;
		break;
	case MONSTER_TYPE_FAR:
		m_Health = 675;
		m_HP = 675;
		m_Attack = 180;
		m_Defense = 80;
		break;
	case MONSTER_TYPE_RUSH:
		m_Health = 1130;
		m_HP = 1130;
		m_Attack = 460;
		m_Defense = 110;
		break;
	case MONSTER_TYPE_BOSS:
		m_Health = 20000;
		m_HP = 20000;
		m_Attack = 200;
		m_Defense = 90;
		break;
	default:
		break;
	}

	Monster_HP_UI* m_HP_UI = new Monster_HP_UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pHP = m_HP_UI;

	if (pWeaponL && pWeaponR) {
		Object* Hand = FindFrame("Weapon_Goblin_3_R_Dummy");
		if (Hand) {
			Hand->SetChild(pWeaponR->m_pRoot, true);

		}
		Hand = FindFrame("Weapon_Goblin_3_L_Dummy");
		if (Hand) {
			Hand->SetChild(pWeaponL->m_pRoot, true);

		}
	}
}

void Character::OnPrepareRender()
{
	Object::OnPrepareRender();
	m_pHP->HP = m_HP / m_Health;
}
