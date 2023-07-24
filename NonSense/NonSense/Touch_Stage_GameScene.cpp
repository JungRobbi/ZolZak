#include "Touch_Stage_GameScene.h"
#include "BoxCollideComponent.h"
class GameFramework;

void Touch_Stage_GameScene::BuildLightsAndMaterials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.3f, 0.3f, 0.5f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 50.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.5f, 1.0f);
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

void Touch_Stage_GameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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
	StartNPC->SetPosition(228.04f, m_pTerrain->GetHeight(228.04f, 48.f), 48.f);
	StartNPC->SetNum(102);
	StartNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		StartNPC->script.emplace_back("�ȳ�");
		StartNPC->script.emplace_back("���� �������� ����");
		StartNPC->script.emplace_back("���� ���� ������");
		StartNPC->script.emplace_back("���̷�");
	}

	EndNPC = new NPC(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Ent"]);
	EndNPC->SetPosition(-29.25f, m_pTerrain->GetHeight(-29.25f , 146.89f), 146.89f);
	EndNPC->SetNum(102);
	EndNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		EndNPC->script.emplace_back("2�ȳ�2");
		EndNPC->script.emplace_back("2���� ����2");
		EndNPC->script.emplace_back("2���̷�2");
	}

	Object* TempObject = NULL;
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Close"], ModelMap["Skull_Close_Weapon_L"], ModelMap["Skull_Close_Weapon_R"], MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(206.1f, m_pTerrain->GetHeight(206.1f, 82.9f), 82.9f);
	TempObject->SetNum(10001);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Far"], ModelMap["Skull_Far_Weapon_L"], ModelMap["Skull_Far_Weapon_R"], MONSTER_TYPE_FAR);
	TempObject->SetPosition(172.9f, m_pTerrain->GetHeight(172.9f, 71.6f), 71.6f);
	TempObject->SetNum(10002);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(190.1f, m_pTerrain->GetHeight(190.1f, 128.8f), 128.8f);
	TempObject->SetNum(10003);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(166.62f, m_pTerrain->GetHeight(166.62f, 142.3f), 142.3f);
	TempObject->SetNum(10004);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(141.4f, m_pTerrain->GetHeight(141.4f, 156.7f), 156.7f);
	TempObject->SetNum(10005);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(126.5f, m_pTerrain->GetHeight(126.5f, 184.9f), 184.9f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(99.f, m_pTerrain->GetHeight(99.f, 171.5f), 171.5f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(74.1f, m_pTerrain->GetHeight(74.1f, 162.1f), 162.1f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(45.9f, m_pTerrain->GetHeight(45.9f, 153.3f), 153.3f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Far"], NULL, NULL, MONSTER_TYPE_FAR);
	TempObject->SetPosition(153.3f, m_pTerrain->GetHeight(153.3f, 146.6f), 146.6f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(154.9f, m_pTerrain->GetHeight(154.9f, 151.8f), 151.8f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Skull(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Skull_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
	TempObject->SetPosition(161.9f, m_pTerrain->GetHeight(161.9f, 128.8f), 128.8f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);

	TempObject = new Water(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, 40, 40);
	TempObject->SetPosition(91.f, 2.98f, 151.f);
	TempObject->SetNum(10000);


	DXGI_FORMAT pdxgiRtvFormats[MRT] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,  DXGI_FORMAT_R8G8B8A8_UNORM };

	m_pBlendShader = new BlendShader();
	m_pBlendShader->CreateShader(pd3dDevice, m_pGraphicsRootSignature, MRT, pdxgiRtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);

	Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Non_Blend_Object_3_Map.bin", m_pBoundingShader);
	Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Object_3_Map.bin", m_pBlendShader);

	m_pSkyBox = new NightSkyBox(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_pSkyBox->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	Sound* s;
	s = new Sound("Sound/TestMusic.mp3", true);
	AddSound(s);

	m_pd3dDevice = pd3dDevice;
	m_pd3dCommandList = pd3dCommandList;
}
void Touch_Stage_GameScene::ReleaseObjects()
{
	GameScene::ReleaseObjects();
}

void Touch_Stage_GameScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	GameScene::OnPrepareRender(pd3dCommandList, pCamera);
}

bool Touch_Stage_GameScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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
