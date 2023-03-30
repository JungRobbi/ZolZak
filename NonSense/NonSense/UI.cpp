//#include "stdafx.h"
//#include "GameScene.h"
//#include "UI.h"
//
//UI::UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
//{
//	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
//	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Player_State.dds", RESOURCE_TEXTURE2D, 0);
//
//	UIShader* pUIShader = new UIShader();
//	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
//	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 18, false);
//
//	Material* pUIMaterial = new Material();
//	pUIMaterial->SetTexture(pUITexture);
//	pUIMaterial->SetShader(pUIShader);
//	SetMaterial(pUIMaterial);
//
//	CreateShaderVariables(pd3dDevice, pd3dCommandList);
//}
//
//UI::~UI()
//{
//}
//
//void UI::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
//{
//	UINT ncbElementBytes = ((sizeof(CB_PLAYER_INFO) + 255) & ~255); //256ÀÇ ¹è¼ö
//
//	m_pd3dcbUI = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
//	m_pd3dcbUI->Map(0, NULL, (void**)&m_pcbMappedUI);
//}
//
//void UI::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
//{
//	XMFLOAT4X4 xmf4x4World;
//	D3D12_GPU_VIRTUAL_ADDRESS d3dcbUIGpuVirtualAddress = m_pd3dcbUI->GetGPUVirtualAddress();
//	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&GetWorld())));
//	CB_PLAYER_INFO* pbMappedcbUI = (CB_PLAYER_INFO*)((UINT8*)m_pcbMappedUI);
//	::memcpy(&pbMappedcbUI->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
//
//	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dcbUIGpuVirtualAddress);
//}
//
//void UI::ReleaseShaderVariables()
//{
//}
//
//void UI::SetPosition(float x, float y, float w, float h)
//{
//	m_xmf4x4World._11 = w;
//	m_xmf4x4World._22 = h;
//	m_xmf4x4World._41 = x;
//	m_xmf4x4World._42 = y;
//}
//void UI::SetPos(float x, float y, float w, float h)
//
//{
//	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
//	m_xmf4x4World._11 = w; // 0 ~ 1 -> 0 ~ 2
//	m_xmf4x4World._22 = h;
//	m_xmf4x4World._41 = x; // 0 ~ 1 -> -1 ~ 1
//	m_xmf4x4World._42 = y;
//
//	if (ParentUI) {
//		XMFLOAT4X4 ParentWorld;
//		XMStoreFloat4x4(&ParentWorld, XMMatrixIdentity());
//		ParentWorld._11 = ParentUI->GetWorld()._11 / 2; // 0 ~ 1 -> 0 ~ 2
//		ParentWorld._22 = ParentUI->GetWorld()._22 / 2;
//		ParentWorld._41 = (ParentUI->GetWorld()._41 + 1) / 2; // 0 ~ 1 -> -1 ~ 1
//		ParentWorld._42 = (ParentUI->GetWorld()._42 + 1) / 2;
//		m_xmf4x4World = Matrix4x4::Multiply(m_xmf4x4World, ParentWorld);
//	}
//
//	m_xmf4x4World._11 = m_xmf4x4World._11 * 2; // 0 ~ 1 -> 0 ~ 2
//	m_xmf4x4World._22 = m_xmf4x4World._22 * 2;
//	m_xmf4x4World._41 = m_xmf4x4World._41 * 2 - 1; // 0 ~ 1 -> -1 ~ 1
//	m_xmf4x4World._42 = m_xmf4x4World._42 * 2 - 1;
//};
//
//void UI::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
//{
//	SetPos(0, 0, 1, 1);
//	UpdateShaderVariables(pd3dCommandList);
//
//	if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
//	if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);
//
//	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	pd3dCommandList->DrawInstanced(6, 1, 0, 0);
//
//}
//
//Player_State_UI::Player_State_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature):UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
//{
//	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
//	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Player_State.dds", RESOURCE_TEXTURE2D, 0);
//
//	UIShader* pUIShader = new UIShader();
//	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
//	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 18, false);
//
//	Material* pUIMaterial = new Material();
//	pUIMaterial->SetTexture(pUITexture);
//	pUIMaterial->SetShader(pUIShader);
//	SetMaterial(pUIMaterial);
//
//	CreateShaderVariables(pd3dDevice, pd3dCommandList);
//}