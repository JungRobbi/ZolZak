﻿#include "Characters.h"
#include "BoxCollideComponent.h"
#include "GameScene.h"


Character::Character(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel) : Object(false)
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
	GameScene::MainScene->creationMonsterQueue.push((Character*)this);
}

Goblin::Goblin(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel, LoadedModelInfo* pWeaponL, LoadedModelInfo* pWeaponR, MonsterType type) :
	Character(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pModel)
{
	switch (type)
	{
	case MONSTER_TYPE_CLOSE:
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
	m_pHP->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(0, GetComponent<BoxCollideComponent>()->GetBoundingObject()->Extents.y * 2 + 0.3, 0)));
	m_pHP->HP = m_RemainHP / m_Health;
}

void Character::update()
{
	//m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(XMFLOAT3(0.0f, -60.0f, 0.0f), fTimeElapsed, false));
	//SetPosition(Vector3::Add(GetPosition(), m_xmf3Velocity));
}

void Character::OnUpdateCallback(float fTimeElapsed)
{
	//XMFLOAT3 xmf3PlayerPosition = GetPosition();
	//HeightMapTerrain* pTerrain = (HeightMapTerrain*)m_pPlayerUpdatedContext;

	//float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x + 400.0f, xmf3PlayerPosition.z + 400.0f);
	//if (xmf3PlayerPosition.y < fHeight)
	//{
	//	XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
	//	xmf3PlayerVelocity.y = 0.0f;
	//	SetVelocity(xmf3PlayerVelocity);
	//	xmf3PlayerPosition.y = fHeight;
	//	SetPosition(xmf3PlayerPosition);
	//}
}