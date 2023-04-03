#include "Login_GameScene.h"

void Login_GameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 16, 25);

	Material::PrepareShaders(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	BuildLightsAndMaterials();

	m_nObjects = 2;
	m_GameObjects = new Object * [m_nObjects];

	LoadedModelInfo* pModel = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/F05.bin", NULL);
	LoadedModelInfo* pWeaponModel = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Wand.bin", NULL);

	m_GameObjects[0] = new TestModelObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, pModel, pWeaponModel, 1);
	m_GameObjects[0]->SetNum(0);
	m_GameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 4);
	m_GameObjects[0]->SetPosition(0.0f, 0.0f, 0.0f);

	pModel = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/goblin_Far.bin", NULL);
	m_GameObjects[1] = new TestModelObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, pModel, NULL, 1);
	m_GameObjects[1]->SetNum(1);
	m_GameObjects[1]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 1);
	m_GameObjects[1]->SetPosition(1.0f, 0.0f, 0.0f);

	/*pModel = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/goblin_Close.bin", NULL);

	m_GameObjects[2] = new TestModelObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, pModel, NULL, 1);
	m_GameObjects[2]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 6);
	m_GameObjects[2]->SetNum(2);
	m_GameObjects[2]->SetPosition(2.0f, 0.0f, 0.0f);

	pModel = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/goblin_Rush.bin", NULL);

	m_GameObjects[3] = new TestModelObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, pModel, NULL, 1);
	m_GameObjects[3]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 5);
	m_GameObjects[3]->SetNum(3);
	m_GameObjects[3]->SetPosition(-1.0f, 0.0f, 0.0f);

	pModel = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/ent.bin", NULL);

	m_GameObjects[4] = new TestModelObject(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, pModel, NULL, 1);
	m_GameObjects[4]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 3);
	m_GameObjects[4]->SetNum(4);
	m_GameObjects[4]->SetPosition(-3.0f, 0.0f, 0.0f);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);*/

	m_pSkyBox = new SkyBox(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_pSkyBox->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
