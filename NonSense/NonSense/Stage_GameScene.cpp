#include "Stage_GameScene.h"

void Stage_GameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 150);

	Material::PrepareShaders(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	BuildLightsAndMaterials();
	XMFLOAT3 xmf3Scale(1.0f, 0.38f, 1.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);

	Game_Option_UI* m_Game_Option_Dec_UI = new Game_Option_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Graphic_Option_UI* m_Graphic_Option_Dec_UI = new Graphic_Option_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Sound_Option_UI* m_Sound_Option_Dec_UI = new Sound_Option_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Option_UI* m_Option_Dec_UI = new Option_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);

	Player_State_UI* m_pUI = new Player_State_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Player_HP_UI* m_pHP_UI = new Player_HP_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Player_HP_DEC_UI* m_pHP_Dec_UI = new Player_HP_DEC_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);

	m_pHP_UI->SetParentUI(m_pUI);
	m_pHP_Dec_UI->SetParentUI(m_pUI);

	m_Game_Option_Dec_UI->SetParentUI(m_Option_Dec_UI);
	m_Graphic_Option_Dec_UI->SetParentUI(m_Option_Dec_UI);
	m_Sound_Option_Dec_UI->SetParentUI(m_Option_Dec_UI);

	HeightMapTerrain* terrain = new HeightMapTerrain(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, _T("Terrain/terrain.raw"), 800, 800, xmf3Scale, xmf4Color);
	terrain->SetPosition(-400, 0, -400);
	m_pTerrain = terrain;

	DXGI_FORMAT pdxgiRtvFormats[MRT] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,  DXGI_FORMAT_R8G8B8A8_UNORM };

	m_pBlendShader = new BlendShader();
	m_pBlendShader->CreateShader(pd3dDevice, m_pGraphicsRootSignature, MRT, pdxgiRtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);

	Object::LoadMapData(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/NonBlend_Props_Map.bin");
	Object::LoadMapData_Blend(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature, "Model/Blend_Objects_Map.bin", m_pBlendShader);

	m_pSkyBox = new SkyBox(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_pSkyBox->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
