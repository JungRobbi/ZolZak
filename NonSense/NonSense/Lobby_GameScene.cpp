#include "Lobby_GameScene.h"

void Lobby_GameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 150);

	Material::PrepareShaders(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	BuildLightsAndMaterials();

	m_pSkyBox = new SkyBox(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_pSkyBox->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	Lobby_BackGround_UI* m_Lobby_BackGround_UI = new Lobby_BackGround_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Lobby_UI* m_Lobby_UI = new Lobby_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Make_Room_UI* m_Make_Room_UI = new Make_Room_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Join_Room_UI* m_Join_Room_UI = new Join_Room_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Back_UI* m_Back_UI = new Back_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Right_UI* m_Right_UI = new Right_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Left_UI* m_Left_UI = new Left_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Title_UI* m_Title_UI = new Title_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_Make_Title_UI = new Make_Title_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pd3dDevice = pd3dDevice;
	m_pd3dCommandList = pd3dCommandList;
}

void Lobby_GameScene::RenderUI(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	OnPrepareRender(pd3dCommandList, pCamera);
	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	for (auto& object : UIGameObjects)
	{
		object->UpdateTransform(NULL);
		object->Render(pd3dCommandList, pCamera);
	}
	if (MakingRoom) {
		m_Make_Title_UI->UpdateTransform(NULL);
		m_Make_Title_UI->Render(pd3dCommandList, pCamera);
	}
	for (int i = Page*6; i < (Page+1) * 6; ++i)
	{
		if (Rooms.size() > i) {
			Rooms[i]->SetMyPos(0.025, 0.6115 - (i%6)*0.105, 0.67, 0.09);
			Rooms[i]->UpdateTransform(NULL);
			Rooms[i]->Render(pd3dCommandList, pCamera);
		}
	}
}

void Lobby_GameScene::MakeRoom(std::string name)
{
	Room_UI* room = new Room_UI(m_pd3dDevice, m_pd3dCommandList, m_pGraphicsRootSignature, 0, name, m_pPlayer->m_name);
	Rooms.emplace_back(room);
}
