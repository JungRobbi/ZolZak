#include "Login_GameScene.h"

void Login_GameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 150);

	Material::PrepareShaders(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	BuildLightsAndMaterials();
	XMFLOAT3 xmf3Scale(1.0f, 0.38f, 1.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);

	Login_BackGround_UI* m_Game_Option_Dec_UI = new Login_BackGround_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Login_UI* Login = new Login_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	WhiteRect_UI* WhiteRect = new WhiteRect_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	Loading_UI* m_Loading_UI = new Loading_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
}
