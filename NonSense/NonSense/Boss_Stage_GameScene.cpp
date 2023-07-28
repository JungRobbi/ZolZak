#include "Boss_Stage_GameScene.h"
#include "BoxCollideComponent.h"
#include "GameFramework.h"
#include "Timer.h"
class GameFramework;

void Boss_Stage_GameScene::BuildLightsAndMaterials()
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

void Boss_Stage_GameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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

	HeightMapTerrain* terrain = new HeightMapTerrain(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, _T("Terrain/terrain.raw"), 800, 800, 37, 37, xmf3Scale, xmf4Color);
	terrain->SetPosition(-400, 0, -400);
	m_pTerrain = terrain;

	Object* TempObject = NULL;
	TempObject = new Shield(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, ModelMap["Boss_Shield"], NULL, NULL, MONSTER_TYPE_BOSS);
	TempObject->SetPosition(-177.75f, m_pTerrain->GetHeight(-177.75f, 173.79f), 173.79f);
	TempObject->SetNum(106);
	TempObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	Boss = TempObject;
	DXGI_FORMAT pdxgiRtvFormats[MRT] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,  DXGI_FORMAT_R8G8B8A8_UNORM };

	m_pBlendShader = new BlendShader();
	m_pBlendShader->CreateShader(pd3dDevice, m_pGraphicsRootSignature, MRT, pdxgiRtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);

	Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Non_Blend_Object_4_Map.bin", m_pBoundingShader);
	Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Object_4_Map.bin", m_pBlendShader);

	m_pSkyBox = new DaySkyBox(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_pSkyBox->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	Sound* s;
	s = new Sound("Sound/TestMusic.mp3", true);
	AddSound(s);

	m_pd3dDevice = pd3dDevice;
	m_pd3dCommandList = pd3dCommandList;
}
void Boss_Stage_GameScene::ReleaseObjects()
{
	GameScene::ReleaseObjects();
}

void Boss_Stage_GameScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	GameScene::OnPrepareRender(pd3dCommandList, pCamera);
}

bool Boss_Stage_GameScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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
		case VK_DELETE:
			if (Boss)
				((Character*)Boss)->GetHit(20000);
		default:
			break;
		}

	default:
		break;
	}
	return(false);
}

void Boss_Stage_GameScene::update()
{
	
}
