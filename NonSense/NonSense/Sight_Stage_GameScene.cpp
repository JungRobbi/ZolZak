#include "Sight_Stage_GameScene.h"
#include "BoxCollideComponent.h"
class GameFramework;

void Sight_Stage_GameScene::BuildLightsAndMaterials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 50.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(0.0f, 1000.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[0].m_fFalloff = 8.0f;
	m_pLights->m_pLights[0].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[0].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	m_pLights->m_pLights[1].m_bEnable = false;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 50.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.1f, 0.1f, 0.2f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

}

void Sight_Stage_GameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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
	Shadow_Left_UI* m_Shadow_Left_UI = new Shadow_Left_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Shadow_Right_UI* m_Shadow_Right_UI = new Shadow_Right_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Loading_UI* m_Loading_UI = new Loading_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);

	ScriptUI = new NPCScript(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);

	HeightMapTerrain* terrain = new HeightMapTerrain(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, _T("Terrain/terrain.raw"), 800, 800, 37, 37, xmf3Scale, xmf4Color);
	terrain->SetPosition(-400, 0, -400);
	m_pTerrain = terrain;
	StartNPC = new NPC(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Ent"]);
	StartNPC->SetPosition(-16.7, m_pTerrain->GetHeight(-16.7, 96.5), 96.5);
	StartNPC->SetNum(102);
	StartNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		StartNPC->script.emplace_back("asdf");
		StartNPC->script.emplace_back("�dffd�");
		StartNPC->script.emplace_back("�ffff���");
		StartNPC->script.emplace_back("���̷�");
	}

	EndNPC = new NPC(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Ent"]);
	EndNPC->SetPosition(139.85f, m_pTerrain->GetHeight(139.85f, 13.24f), 13.24f);
	EndNPC->SetNum(102);
	EndNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		EndNPC->script.emplace_back("2�ȳ�2");
		EndNPC->script.emplace_back("2���� ����2");
		EndNPC->script.emplace_back("2���̷�2");
	}

	Object* TempObject = NULL;
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], ModelMap["Goblin_Close_Weapon_L"], ModelMap["Goblin_Close_Weapon_R"], MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(-8.1f, m_pTerrain->GetHeight(-8.1f, 91.15f), 91.15f);
	TempObject->SetNum(10001);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], ModelMap["Goblin_Far_Weapon_L"], ModelMap["Goblin_Far_Weapon_R"], MONSTER_TYPE_FAR);
	TempObject->SetPosition(-2.23f, m_pTerrain->GetHeight(2.23f, 85.3f), 85.3f);
	TempObject->SetNum(10002);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(19.42f, m_pTerrain->GetHeight(19.42f, 80.73f), 80.73f);
	TempObject->SetNum(10003);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(-1.27f, m_pTerrain->GetHeight(-1.27f, 60.32f), 60.32f);
	TempObject->SetNum(10004);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(-0.14f, m_pTerrain->GetHeight(-0.14f, 36.66f), 36.66f);
	TempObject->SetNum(10005);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(15.05f, m_pTerrain->GetHeight(15.05f, 26.07f), 26.07f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(25.01f, m_pTerrain->GetHeight(25.01f, 42.33f), 42.33f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(53.29f, m_pTerrain->GetHeight(53.29f, 45.72f), 45.72f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(93.69f, m_pTerrain->GetHeight(93.69f, 34.69f), 34.69f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(100.43f, m_pTerrain->GetHeight(100.43f, 31.01f), 31.01f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(97.29f, m_pTerrain->GetHeight(97.29f, 30.13f), 30.13f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);

	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature,20,20);
	TempObject->SetPosition(40.59f, 5.4f, -29.63f);
	TempObject->SetNum(10000);
	TempObject->Rotate(0.f, 30.f, 0.f);
	DXGI_FORMAT pdxgiRtvFormats[MRT] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,  DXGI_FORMAT_R8G8B8A8_UNORM };



	m_pBlendShader = new BlendShader();
	m_pBlendShader->CreateShader(pd3dDevice, m_pGraphicsRootSignature, MRT, pdxgiRtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);

	Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Non_Blend_Object_4_Map.bin", m_pBoundingShader);
	Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Object_4_Map.bin", m_pBlendShader);
	Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Non_Blend_Object_2_Map.bin", m_pBoundingShader);
	Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Object_2_Map.bin", m_pBlendShader);
	Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/NonBlend_Props_Map.bin", m_pBoundingShader);
	Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Objects_Map.bin", m_pBlendShader);
	Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Non_Blend_Object_3_Map.bin", m_pBoundingShader);
	Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Object_3_Map.bin", m_pBlendShader);

	m_pSkyBox = new DaySkyBox(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_pSkyBox->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	Sound* s;
	s = new Sound("Sound/TestMusic.mp3", true);
	AddSound(s);
	m_pd3dDevice = pd3dDevice;
	m_pd3dCommandList = pd3dCommandList;
}
void Sight_Stage_GameScene::ReleaseObjects()
{
	GameScene::ReleaseObjects();
}

void Sight_Stage_GameScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	GameScene::OnPrepareRender(pd3dCommandList, pCamera);
}

bool Sight_Stage_GameScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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
