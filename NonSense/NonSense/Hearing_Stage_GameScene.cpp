#include "Hearing_Stage_GameScene.h"
#include "BoxCollideComponent.h"
class GameFramework;

void Hearing_Stage_GameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 500);
	LoadAllModels(pd3dDevice, pd3dCommandList);
	Material::PrepareShaders(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	BuildLightsAndMaterials();
	XMFLOAT3 xmf3Scale(1.0f, 0.38f, 1.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);

	Aim* aim = new Aim(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Option_UI* m_Option_Dec_UI = new Option_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Line_Left_UI* m_Line_Left_UI = new Line_Left_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Line_Right_UI* m_Line_Right_UI = new Line_Right_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Color_Left_UI* m_Color_Left_UI = new Color_Left_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Color_Right_UI* m_Color_Right_UI = new Color_Right_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Toon_Left_UI* m_Toon_Left_UI = new Toon_Left_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Toon_Right_UI* m_Toon_Right_UI = new Toon_Right_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Mouse_Left_UI* m_Mouse_Left_UI = new Mouse_Left_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Mouse_Right_UI* m_Mouse_Right_UI = new Mouse_Right_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);

	ScriptUI = new NPCScript(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);

	HeightMapTerrain* terrain = new HeightMapTerrain(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, _T("Terrain/terrain.raw"), 800, 800, 37, 37, xmf3Scale, xmf4Color);
	terrain->SetPosition(-400, 0, -400);
	m_pTerrain = terrain;
	StartNPC = new NPC(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Ent"]);
	StartNPC->SetPosition(139.85f, m_pTerrain->GetHeight(139.85f, 13.24f), 13.24f);
	StartNPC->SetNum(102);
	StartNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		StartNPC->script.emplace_back("�ȳ�");
		StartNPC->script.emplace_back("���� �������� ����");
		StartNPC->script.emplace_back("���� ���� ������");
		StartNPC->script.emplace_back("���̷�");
	}

	EndNPC = new NPC(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Ent"]);
	EndNPC->SetPosition(285.f, m_pTerrain->GetHeight(285.f, 43.f), 43.f);
	EndNPC->SetNum(102);
	EndNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		EndNPC->script.emplace_back("2�ȳ�2");
		EndNPC->script.emplace_back("2���� ����2");
		EndNPC->script.emplace_back("2���̷�2");
	}

	Object* TempObject = NULL;
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Close"], ModelMap["Orc_Close_Weapon_L"], ModelMap["Orc_Close_Weapon_R"], MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(-149.62f, m_pTerrain->GetHeight(-149.62f, 8.02f), 8.02f);
	TempObject->SetNum(10001);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Far"], ModelMap["Orc_Far_Weapon_L"], ModelMap["Orc_Far_Weapon_R"], MONSTER_TYPE_FAR);
	TempObject->SetPosition(-159.09f, m_pTerrain->GetHeight(-159.09f, 31.32f), 31.32f);
	TempObject->SetNum(10002);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(176.4f, m_pTerrain->GetHeight(176.4f, 42.8f), 42.8f);
	TempObject->SetNum(10003);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(195.55f, m_pTerrain->GetHeight(195.55f, 50.82f), 50.82f);
	TempObject->SetNum(10004);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(226.1f, m_pTerrain->GetHeight(226.1f, 53.4f), 53.4f);
	TempObject->SetNum(10005);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(247.1f, m_pTerrain->GetHeight(247.1f, 31.1f), 31.1f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(280.7, m_pTerrain->GetHeight(280.7, 20.2), 20.2);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(273.1f, m_pTerrain->GetHeight(273.1f, -12.6f), -12.6f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(256.9f, m_pTerrain->GetHeight(256.9f, -37.3f), -37.3f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(215.f, m_pTerrain->GetHeight(215.f, -52.21f), -52.21f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(196.f, m_pTerrain->GetHeight(196.f, -46.3f), -46.3f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(176.2f, m_pTerrain->GetHeight(176.2f, -25.f), -25.f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(178.5f, m_pTerrain->GetHeight(178.5f, -48.3f), -48.3f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Orc(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Orc_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(184.9f, m_pTerrain->GetHeight(184.9f, -70.3f), -70.3f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);


	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 20, 20);
	TempObject->SetPosition(239.f, 2.0f, -67.f);
	TempObject->SetNum(10000);
	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 20, 20);
	TempObject->SetPosition(239.f, 2.0f, -107);
	TempObject->SetNum(10000);
	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 20, 20);
	TempObject->SetPosition(239.f, 2.0f, -147.f);
	TempObject->SetNum(10000);
	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 20, 20);
	TempObject->SetPosition(239.f, 2.0f, -27.f);
	TempObject->SetNum(10000);
	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 20, 20);
	TempObject->SetPosition(199.f, 2.0f, -27.f);
	TempObject->SetNum(10000);
	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 20, 20);
	TempObject->SetPosition(199.f, 2.0f, 13.f);
	TempObject->SetNum(10000);


	DXGI_FORMAT pdxgiRtvFormats[MRT] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,  DXGI_FORMAT_R8G8B8A8_UNORM };

	m_pBlendShader = new BlendShader();
	m_pBlendShader->CreateShader(pd3dDevice, m_pGraphicsRootSignature, MRT, pdxgiRtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);

	Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Non_Blend_Object_2_Map.bin", m_pBoundingShader);
	Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Object_2_Map.bin", m_pBlendShader);

	m_pSkyBox = new SkyBox(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_pSkyBox->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	Sound* s;
	s = new Sound("Sound/TestMusic.mp3", true);
	AddSound(s);

	Sound_Debuff(-10);

	m_pd3dDevice = pd3dDevice;
	m_pd3dCommandList = pd3dCommandList;
}
void Hearing_Stage_GameScene::ReleaseObjects()
{
	GameScene::ReleaseObjects();
}

void Hearing_Stage_GameScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	GameScene::OnPrepareRender(pd3dCommandList, pCamera);
}

bool Hearing_Stage_GameScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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
