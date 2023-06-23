#include "Stage_GameScene.h"
#include "BoxCollideComponent.h"
class GameFramework;
void Stage_GameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 170);

	Material::PrepareShaders(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	BuildLightsAndMaterials();
	XMFLOAT3 xmf3Scale(1.0f, 0.38f, 1.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);

	Aim* aim = new Aim(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Option_UI* m_Option_Dec_UI = new Option_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Sound_Option_UI* m_Sound_Option_Dec_UI = new Sound_Option_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Graphic_Option_UI* m_Graphic_Option_Dec_UI = new Graphic_Option_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Game_Option_UI* m_Game_Option_Dec_UI = new Game_Option_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);

	m_Game_Option_Dec_UI->SetParentUI(m_Option_Dec_UI);
	m_Graphic_Option_Dec_UI->SetParentUI(m_Option_Dec_UI);
	m_Sound_Option_Dec_UI->SetParentUI(m_Option_Dec_UI);

	LoadedModelInfo* pModel = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Far.bin", NULL);
	HeightMapTerrain* terrain = new HeightMapTerrain(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, _T("Terrain/terrain.raw"), 800, 800, xmf3Scale, xmf4Color);
	terrain->SetPosition(-400, 0, -400);
	m_pTerrain = terrain;



	Object* TempObject = NULL;
	LoadedModelInfo* pRW = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Close_Weapon_R.bin", NULL);
	LoadedModelInfo* pLW = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Close_Weapon_L.bin", NULL);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, pModel, pLW, pRW, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(-9.0f, m_pTerrain->GetHeight(-9.0f, 9.0f), 87);
	TempObject->SetNum(101);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	pModel = Object::LoadAnimationModel(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Goblin_Close.bin", NULL);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, pModel, pLW, pRW, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(-1.0f, m_pTerrain->GetHeight(-1.0f, 42.0f), 42.0f);
	TempObject->SetNum(102);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, pModel, NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(16.0f, m_pTerrain->GetHeight(16.0f, 34.0f), 34.0f);
	TempObject->SetNum(103);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, pModel, NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(53.0f, m_pTerrain->GetHeight(53.0f, 43.0f), 43.0f);
	TempObject->SetNum(104);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, pModel, NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(89.0f, m_pTerrain->GetHeight(89.0f, 33.0f), 33.0f);
	TempObject->SetNum(105);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, pModel, NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(113.0f, m_pTerrain->GetHeight(113.0f, 20.0f), 20.0f);
	TempObject->SetNum(106);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);



	DXGI_FORMAT pdxgiRtvFormats[MRT] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,  DXGI_FORMAT_R8G8B8A8_UNORM };

	m_pBlendShader = new BlendShader();
	m_pBlendShader->CreateShader(pd3dDevice, m_pGraphicsRootSignature, MRT, pdxgiRtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);

	Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/NonBlend_Props_Map.bin", m_pBoundingShader);
	Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Objects_Map.bin", m_pBlendShader);

	m_pSkyBox = new SkyBox(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_pSkyBox->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	bgm = new Sound("Sound/TestMusic.mp3", true);
	bgm->Play();
}


void Stage_GameScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	GameScene::OnPrepareRender(pd3dCommandList, pCamera);
}

bool Stage_GameScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_SPACE:
			
			break;
		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		default:
			break;
		}

	default:
		break;
	}
	return(false);
}