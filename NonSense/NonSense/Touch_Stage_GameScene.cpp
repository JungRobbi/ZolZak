#include "Touch_Stage_GameScene.h"
#include "BoxCollideComponent.h"
class GameFramework;

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
	StartNPC->SetPosition(-16.7, m_pTerrain->GetHeight(-16.7, 96.5), 96.5);
	StartNPC->SetNum(102);
	StartNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		StartNPC->script.emplace_back("�ȳ�");
		StartNPC->script.emplace_back("���� �������� ����");
		StartNPC->script.emplace_back("���� ���� ������");
		StartNPC->script.emplace_back("���̷�");
	}

	EndNPC = new NPC(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Ent"]);
	EndNPC->SetPosition(-5.7, m_pTerrain->GetHeight(-16.7, 96.5), 96.5);
	EndNPC->SetNum(102);
	EndNPC->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	{
		EndNPC->script.emplace_back("2�ȳ�2");
		EndNPC->script.emplace_back("2���� ����2");
		EndNPC->script.emplace_back("2���̷�2");
	}

	Object* TempObject = NULL;
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], ModelMap["Goblin_Close_Weapon_L"], ModelMap["Goblin_Close_Weapon_R"], MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(-9.0f, m_pTerrain->GetHeight(-9.0f, 9.0f), 87);
	TempObject->SetNum(10001);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);

	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Far"], ModelMap["Goblin_Far_Weapon_L"], ModelMap["Goblin_Far_Weapon_R"], MONSTER_TYPE_FAR);
	TempObject->SetPosition(-1.0f, m_pTerrain->GetHeight(-1.0f, 42.0f), 42.0f);
	TempObject->SetNum(10002);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(16.0f, m_pTerrain->GetHeight(16.0f, 34.0f), 34.0f);
	TempObject->SetNum(10003);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(53.0f, m_pTerrain->GetHeight(53.0f, 43.0f), 43.0f);
	TempObject->SetNum(10004);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(89.0f, m_pTerrain->GetHeight(89.0f, 33.0f), 33.0f);
	TempObject->SetNum(10005);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	TempObject = new Goblin(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
	TempObject->SetPosition(113.0f, m_pTerrain->GetHeight(113.0f, 20.0f), 20.0f);
	TempObject->SetNum(10006);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);

	TempObject = new Shield(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Boss_Shield"], NULL, NULL, MONSTER_TYPE_BOSS);
	TempObject->SetPosition(-16.7, m_pTerrain->GetHeight(-16.7, 96.5), 96.5);
	TempObject->SetNum(106);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);

	DXGI_FORMAT pdxgiRtvFormats[MRT] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,  DXGI_FORMAT_R8G8B8A8_UNORM };

	m_pBlendShader = new BlendShader();
	m_pBlendShader->CreateShader(pd3dDevice, m_pGraphicsRootSignature, MRT, pdxgiRtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);

	//Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/NonBlend_Props_Map.bin", m_pBoundingShader);
	//Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Objects_Map.bin", m_pBlendShader);
	Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Non_Blend_Object_2_Map.bin", m_pBoundingShader);
	Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Object_2_Map.bin", m_pBlendShader);
	//Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Non_Blend_Object_3_Map.bin", m_pBoundingShader);
	//Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Object_3_Map.bin", m_pBlendShader);

	m_pSkyBox = new SkyBox(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
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
