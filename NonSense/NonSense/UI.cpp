#include "UI.h"
#include "Shader.h"
#include "GameScene.h"
#include "GameFramework.h"
#include "Lobby_GameScene.h"

UI::UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : Object(false)
{
}

UI::~UI()
{
}

void UI::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_PLAYER_INFO) + 255) & ~255); //256의 배수

	m_pd3dcbUI = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbUI->Map(0, NULL, (void**)&m_pcbMappedUI);
}

void UI::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbUIGpuVirtualAddress = m_pd3dcbUI->GetGPUVirtualAddress();
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&GetWorld())));
	CB_PLAYER_INFO* pbMappedcbUI = (CB_PLAYER_INFO*)((UINT8*)m_pcbMappedUI);
	::memcpy(&pbMappedcbUI->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
	pbMappedcbUI->Value = MouseOn;

	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dcbUIGpuVirtualAddress);
}

void UI::ReleaseShaderVariables()
{
}
void UI::SetMyPos(float x, float y, float w, float h)
{
	XMStoreFloat4x4(&XYWH, XMMatrixIdentity());
	XYWH._11 = w;
	XYWH._22 = h;
	XYWH._41 = x;
	XYWH._42 = y;
}
void UI::OnPreRender()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	m_xmf4x4World = XYWH;
	if (ParentUI) {
		m_xmf4x4World = Matrix4x4::Multiply(XYWH, ParentUI->XYWH);
	}
	m_xmf4x4World._11 = m_xmf4x4World._11 * 2; // 0 ~ 1 -> 0 ~ 2
	m_xmf4x4World._22 = m_xmf4x4World._22 * 2;
	m_xmf4x4World._41 = m_xmf4x4World._41 * 2 - 1; // 0 ~ 1 -> -1 ~ 1
	m_xmf4x4World._42 = m_xmf4x4World._42 * 2 - 1;
};

void UI::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	update();
	OnPreRender();
	UpdateShaderVariables(pd3dCommandList);

	if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
	if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dCommandList->DrawInstanced(6, 1, 0, 0);

}

Player_State_UI::Player_State_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) :UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Player_State.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);

	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);
	CanClick = false;
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.02, 0.02, 0.5, 0.2);
}

Player_HP_UI::Player_HP_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) :UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/HP.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = false;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.17, 0.04, 0.82, 0.32);
}

Warrior_Player_State_UI::Warrior_Player_State_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) :Player_State_UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Warrior_Player_State.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);

	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);
	CanClick = false;
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.02, 0.02, 0.5, 0.2);
}


Player_HP_DEC_UI::Player_HP_DEC_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) :UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/HP_Decrease.dds", RESOURCE_TEXTURE2D, 0);
	CanClick = false;
	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);

	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void Player_HP_DEC_UI::update() {
	if (Dec_HP < HP) {
		HP -= (HP - Dec_HP) / 65;
		if (HP < Dec_HP) HP = Dec_HP;
	}
	SetMyPos(0.17, 0.04, 0.82 * HP, 0.32);
}

Monster_HP_UI::Monster_HP_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) :UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	PlaneMesh* Plane = new PlaneMesh(pd3dDevice, pd3dCommandList, 0.5,0.05);

	SetMesh(Plane);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/HP.dds", RESOURCE_TEXTURE2D, 0);

	BillboardShader* pUIShader = new BillboardShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);
	
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void Monster_HP_UI::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_PLAYER_INFO) + 255) & ~255); //256의 배수

	m_pd3dcbUI = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbUI->Map(0, NULL, (void**)&m_pcbMappedUI);
}

void Monster_HP_UI::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbUIGpuVirtualAddress = m_pd3dcbUI->GetGPUVirtualAddress();
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&GetWorld())));
	CB_PLAYER_INFO* pbMappedcbUI = (CB_PLAYER_INFO*)((UINT8*)m_pcbMappedUI);
	::memcpy(&pbMappedcbUI->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
	::memcpy(&pbMappedcbUI->Value, &HP, sizeof(float));
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dcbUIGpuVirtualAddress);
}

void Monster_HP_UI::ReleaseShaderVariables()
{
}


void Monster_HP_UI::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	update();
	UpdateShaderVariables(pd3dCommandList);

	if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
	if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

	if (m_pMesh)
	{
		m_pMesh->Render(pd3dCommandList, 0);
	}

}


Option_UI::Option_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) :UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/OptionUI.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);

	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.1, 0.1, 0.8, 0.8);
}

void Option_UI::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	if (OptionMode) {
		OnPreRender();
		UpdateShaderVariables(pd3dCommandList);

		if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
		if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

		pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pd3dCommandList->DrawInstanced(6, 1, 0, 0);
	}

}


Login_BackGround_UI::Login_BackGround_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) :UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Capture.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = false;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.0, 0.0, 1.0, 1.0);
}

WhiteRect_UI::WhiteRect_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) :UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/WhiteRect.dds", RESOURCE_TEXTURE2D, 0);
	CanClick = false;
	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);

	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.3, 0.4, 0.4, 0.05);
}

Login_UI::Login_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) :UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/LoginUI.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.4, 0.2, 0.2, 0.15);
}

void Login_UI::OnClick()
{
	GameFramework::MainGameFramework->ChangeScene(LOBBY_SCENE);
}

Aim::Aim(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Aim.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);

	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);
	CanClick = false;
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.485, 0.47, 0.035, 0.05);
}

NPCScript::NPCScript(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Script.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);

	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);
	CanClick = false;
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.05, 0.0, 0.9, 0.4);
}

Lobby_BackGround_UI::Lobby_BackGround_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Lobby_Back.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = false;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.0, 0.0, 1.0, 1.0);
}

Lobby_UI::Lobby_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Lobby_UI.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = false;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.01, 0.012, 0.7, 0.787);
}

Make_Room_UI::Make_Room_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Make_Room_UI.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.72, 0.512, 0.27, 0.22);
}

void Make_Room_UI::OnClick()
{
	//if (!dynamic_cast<Lobby_GameScene*>(GameScene::MainScene)->MakingRoom)
	//{
	//	dynamic_cast<Lobby_GameScene*>(GameScene::MainScene)->MakingRoom = true;
	//}
	std::string name;
	std::cout << "방 제목을 입력하세요 : " << std::endl;
	std::cin >> name;
	dynamic_cast<Lobby_GameScene*>(GameScene::MainScene)->MakeRoom(name);
}

Join_Room_UI::Join_Room_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Join_Room.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.72, 0.262, 0.27, 0.22);
}

void Join_Room_UI::OnClick()
{
	GameFramework::MainGameFramework->ChangeScene(ROOM_SCENE);
}

Back_UI::Back_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Back.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.72, 0.012, 0.27, 0.22);
}

void Back_UI::OnClick()
{
	GameFramework::MainGameFramework->ChangeScene(LOGIN_SCENE);
}


Right_UI::Right_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Right.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.68, 0.012, 0.02, 0.05);
}

void Right_UI::OnClick()
{
	if (dynamic_cast<Lobby_GameScene*>(GameScene::MainScene)->Rooms.size() > dynamic_cast<Lobby_GameScene*>(GameScene::MainScene)->Page * 6 + 6)
		dynamic_cast<Lobby_GameScene*>(GameScene::MainScene)->Page++;
	std::cout << dynamic_cast<Lobby_GameScene*>(GameScene::MainScene)->Page << std::endl;
}

Left_UI::Left_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Left.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.02, 0.012, 0.02, 0.05);
}

void Left_UI::OnClick()
{
	if (dynamic_cast<Lobby_GameScene*>(GameScene::MainScene)->Page > 0)
		dynamic_cast<Lobby_GameScene*>(GameScene::MainScene)->Page--;
	std::cout << dynamic_cast<Lobby_GameScene*>(GameScene::MainScene)->Page << std::endl;
}

Title_UI::Title_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/NON_SENSE.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = false;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.25, 0.8, 0.5, 0.2);
}

Make_Title_UI::Make_Title_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Make_Title.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = false;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.25, 0.25, 0.54, 0.44);
}

Room_UI::Room_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, int num, std::string name, std::string owner) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Room.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);
	RoomName = name;
	RoomNum = num;
	RoomOwner = owner;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.25, 0.25, 0.54, 0.44);
}

void Room_UI::OnClick()
{
	dynamic_cast<Lobby_GameScene*>(GameScene::MainScene)->SelectNum = RoomNum;
	std::cout << RoomNum << "번 방 - " << RoomName << " - 방장 - " << RoomOwner << std::endl;
}

//////////////////////////////////////////////

Line_Right_UI::Line_Right_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Right.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.4, 0.615, 0.02, 0.05);
}

void Line_Right_UI::OnClick()
{
	if (OptionMode) {
		if (GameScene::MainScene->LineSize < 9)
			GameScene::MainScene->LineSize++;
	}
}

void Line_Right_UI::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	if (OptionMode) {
		OnPreRender();
		UpdateShaderVariables(pd3dCommandList);

		if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
		if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

		pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pd3dCommandList->DrawInstanced(6, 1, 0, 0);
	}

}

Line_Left_UI::Line_Left_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Left.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.17, 0.615, 0.02, 0.05);
}

void Line_Left_UI::OnClick()
{
	if (OptionMode) {
		if (GameScene::MainScene->LineSize > 0)
			GameScene::MainScene->LineSize--;
	}
}

void Line_Left_UI::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	if (OptionMode) {
		OnPreRender();
		UpdateShaderVariables(pd3dCommandList);

		if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
		if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

		pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pd3dCommandList->DrawInstanced(6, 1, 0, 0);
	}

}

Color_Right_UI::Color_Right_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Right.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.4, 0.475, 0.02, 0.05);
}

void Color_Right_UI::OnClick()
{
	if (OptionMode) {
		if (GameScene::MainScene->LineColor.x == 0 && GameScene::MainScene->LineColor.y == 0 && GameScene::MainScene->LineColor.z == 0)
		{
				GameScene::MainScene->LineColor.x = 1; GameScene::MainScene->LineColor.y = 0; GameScene::MainScene->LineColor.z = 0;
		}

		else if (GameScene::MainScene->LineColor.x == 1 && GameScene::MainScene->LineColor.y == 0 && GameScene::MainScene->LineColor.z == 0)
		{
				GameScene::MainScene->LineColor.x = 1; GameScene::MainScene->LineColor.y = 1; GameScene::MainScene->LineColor.z = 0;
		}

		else if (GameScene::MainScene->LineColor.x == 1 && GameScene::MainScene->LineColor.y == 1 && GameScene::MainScene->LineColor.z == 0)
		{
			GameScene::MainScene->LineColor.x = 0; GameScene::MainScene->LineColor.y = 1; GameScene::MainScene->LineColor.z = 0;
		}

		else if (GameScene::MainScene->LineColor.x == 0 && GameScene::MainScene->LineColor.y == 1 && GameScene::MainScene->LineColor.z == 0)
		{
			GameScene::MainScene->LineColor.x = 0; GameScene::MainScene->LineColor.y = 1; GameScene::MainScene->LineColor.z = 1;
		}

		else if (GameScene::MainScene->LineColor.x == 0 && GameScene::MainScene->LineColor.y == 1 && GameScene::MainScene->LineColor.z == 1)
		{
			GameScene::MainScene->LineColor.x = 0; GameScene::MainScene->LineColor.y = 0; GameScene::MainScene->LineColor.z = 1;
		}

		else if (GameScene::MainScene->LineColor.x == 0 && GameScene::MainScene->LineColor.y == 0 && GameScene::MainScene->LineColor.z == 1)
		{
			GameScene::MainScene->LineColor.x = 1; GameScene::MainScene->LineColor.y = 0; GameScene::MainScene->LineColor.z = 1;
		}

		else if (GameScene::MainScene->LineColor.x == 1 && GameScene::MainScene->LineColor.y == 0 && GameScene::MainScene->LineColor.z == 1)
		{
			GameScene::MainScene->LineColor.x = 1; GameScene::MainScene->LineColor.y = 1; GameScene::MainScene->LineColor.z = 1;
		}

		else if (GameScene::MainScene->LineColor.x == 1 && GameScene::MainScene->LineColor.y == 1 && GameScene::MainScene->LineColor.z == 1)
		{
			GameScene::MainScene->LineColor.x = 0; GameScene::MainScene->LineColor.y = 0; GameScene::MainScene->LineColor.z = 0;
		}
	}
}

void Color_Right_UI::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	if (OptionMode) {
		OnPreRender();
		UpdateShaderVariables(pd3dCommandList);

		if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
		if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

		pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pd3dCommandList->DrawInstanced(6, 1, 0, 0);
	}

}

Color_Left_UI::Color_Left_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Left.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.17, 0.475, 0.02, 0.05);
}

void Color_Left_UI::OnClick()
{
	if (OptionMode) {
		if (GameScene::MainScene->LineColor.x == 0 && GameScene::MainScene->LineColor.y == 0 && GameScene::MainScene->LineColor.z == 0)
		{
			GameScene::MainScene->LineColor.x = 1; GameScene::MainScene->LineColor.y = 1; GameScene::MainScene->LineColor.z = 1;
		}

		else if (GameScene::MainScene->LineColor.x == 1 && GameScene::MainScene->LineColor.y == 0 && GameScene::MainScene->LineColor.z == 0)
		{
			GameScene::MainScene->LineColor.x = 0; GameScene::MainScene->LineColor.y = 0; GameScene::MainScene->LineColor.z = 0;
		}

		else if (GameScene::MainScene->LineColor.x == 1 && GameScene::MainScene->LineColor.y == 1 && GameScene::MainScene->LineColor.z == 0)
		{
			GameScene::MainScene->LineColor.x = 1; GameScene::MainScene->LineColor.y = 0; GameScene::MainScene->LineColor.z = 0;
		}

		else if (GameScene::MainScene->LineColor.x == 0 && GameScene::MainScene->LineColor.y == 1 && GameScene::MainScene->LineColor.z == 0)
		{
			GameScene::MainScene->LineColor.x = 1; GameScene::MainScene->LineColor.y = 1; GameScene::MainScene->LineColor.z = 0;
		}

		else if (GameScene::MainScene->LineColor.x == 0 && GameScene::MainScene->LineColor.y == 1 && GameScene::MainScene->LineColor.z == 1)
		{
			GameScene::MainScene->LineColor.x = 0; GameScene::MainScene->LineColor.y = 1; GameScene::MainScene->LineColor.z = 0;
		}

		else if (GameScene::MainScene->LineColor.x == 0 && GameScene::MainScene->LineColor.y == 0 && GameScene::MainScene->LineColor.z == 1)
		{
			GameScene::MainScene->LineColor.x = 0; GameScene::MainScene->LineColor.y = 1; GameScene::MainScene->LineColor.z = 1;
		}

		else if (GameScene::MainScene->LineColor.x == 1 && GameScene::MainScene->LineColor.y == 0 && GameScene::MainScene->LineColor.z == 1)
		{
			GameScene::MainScene->LineColor.x = 0; GameScene::MainScene->LineColor.y = 0; GameScene::MainScene->LineColor.z = 1;
		}

		else if (GameScene::MainScene->LineColor.x == 1 && GameScene::MainScene->LineColor.y == 1 && GameScene::MainScene->LineColor.z == 1)
		{
			GameScene::MainScene->LineColor.x = 1; GameScene::MainScene->LineColor.y = 0; GameScene::MainScene->LineColor.z = 1;
		}
	}
}

void Color_Left_UI::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	if (OptionMode) {
		OnPreRender();
		UpdateShaderVariables(pd3dCommandList);

		if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
		if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

		pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pd3dCommandList->DrawInstanced(6, 1, 0, 0);
	}

}

Toon_Right_UI::Toon_Right_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Right.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.4, 0.320, 0.02, 0.05);
}

void Toon_Right_UI::OnClick()
{
	if (OptionMode) {
		if (GameScene::MainScene->LineSize < 10)
			GameScene::MainScene->ToonShading++;
	}
}

void Toon_Right_UI::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	if (OptionMode) {
		OnPreRender();
		UpdateShaderVariables(pd3dCommandList);

		if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
		if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

		pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pd3dCommandList->DrawInstanced(6, 1, 0, 0);
	}

}

Toon_Left_UI::Toon_Left_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Left.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.17, 0.320, 0.02, 0.05);
}

void Toon_Left_UI::OnClick()
{
	if (OptionMode) {
		if (GameScene::MainScene->ToonShading > 0)
			GameScene::MainScene->ToonShading--;
	}
}

void Toon_Left_UI::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	if (OptionMode) {
		OnPreRender();
		UpdateShaderVariables(pd3dCommandList);

		if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
		if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

		pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pd3dCommandList->DrawInstanced(6, 1, 0, 0);
	}

}


Mouse_Right_UI::Mouse_Right_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Right.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.82, 0.615, 0.02, 0.05);
}

void Mouse_Right_UI::OnClick()
{
	if (OptionMode) {
		if (GameFramework::MainGameFramework->MouseSen > 1)
			GameFramework::MainGameFramework->MouseSen--;
	}

}

void Mouse_Right_UI::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	if (OptionMode) {
		OnPreRender();
		UpdateShaderVariables(pd3dCommandList);

		if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
		if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

		pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pd3dCommandList->DrawInstanced(6, 1, 0, 0);
	}

}

Mouse_Left_UI::Mouse_Left_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Left.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.59, 0.615, 0.02, 0.05);
}

void Mouse_Left_UI::OnClick()
{
	if (OptionMode) {
		GameFramework::MainGameFramework->MouseSen++;
	}
}

void Mouse_Left_UI::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	if (OptionMode) {
		OnPreRender();
		UpdateShaderVariables(pd3dCommandList);

		if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
		if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

		pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pd3dCommandList->DrawInstanced(6, 1, 0, 0);
	}

}

///////////////////////////////////////////////////////

Mage_UI::Mage_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Mage_Propile.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);


	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.2, 0.6, 0.2, 0.4);
}

void Mage_UI::OnClick()
{

}

Warrior_UI::Warrior_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Warrior_Propile.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);


	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.0, 0.6, 0.2, 0.4);
}

void Warrior_UI::OnClick()
{

}

Leave_Room_UI::Leave_Room_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Leave_Room.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);


	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.73, 0.78, 0.27, 0.22);
}

void Leave_Room_UI::OnClick()
{

}

Ready_UI::Ready_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	GameScene::MainScene->creationUIQueue.push(this);
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Ready.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);
	CanClick = true;
	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);


	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.0, 0.0, 0.27, 0.22);
}

void Ready_UI::OnClick()
{

}