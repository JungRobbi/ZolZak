#include "Lobby_GameScene.h"
#include <iostream>
void Lobby_GameScene::update()
{
	while (!roomCreateList.empty() && m_ppRooms) {
		auto p = roomCreateList.front();
		std::cout << p.num << " " << p.name << " " << p.owner << std::endl;
		m_ppRooms[p.num]->RoomNum = p.num;
		m_ppRooms[p.num]->RoomName = p.name;
		m_ppRooms[p.num]->RoomOwner = p.owner;
		Rooms.emplace_back(m_ppRooms[p.num]);
		roomCreateList.pop();
	}

	GameScene::update();
}

void Lobby_GameScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 100);

	Material::PrepareShaders(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	BuildLightsAndMaterials();

	Lobby_BackGround_UI* m_Lobby_BackGround_UI = new Lobby_BackGround_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Lobby_UI* m_Lobby_UI = new Lobby_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Make_Room_UI* m_Make_Room_UI = new Make_Room_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Join_Room_UI* m_Join_Room_UI = new Join_Room_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Back_UI* m_Back_UI = new Back_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Right_UI* m_Right_UI = new Right_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Left_UI* m_Left_UI = new Left_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Title_UI* m_Title_UI = new Title_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	m_ppRooms = new Room_UI * [30];
	for (int i = 0; i < 30; ++i)
	{
		m_ppRooms[i] = new Room_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature,0,"name","owner");
	}

	Make_Title_UI* m_Make_Title_UI = new Make_Title_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
	Real_Make_Room_UI* m_Real_Make_Room_UI = new Real_Make_Room_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);

	Loading_UI* m_Loading_UI = new Loading_UI(pd3dDevice, pd3dCommandList, m_pGraphicsRootSignature);
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
	if (!LoadingMode && !MakingRoom) {
		for (int i = Page * 6; i < (Page + 1) * 6; ++i)
		{
			if (Rooms.size() > i) {
				Rooms[i]->SetMyPos(0.025, 0.6115 - (i % 6) * 0.105, 0.67, 0.09);
				Rooms[i]->UpdateTransform(NULL);
				Rooms[i]->Render(pd3dCommandList, pCamera);
			}
		}
	}
	for (auto& Chat_tb : ChatMGR::m_pUILayer->m_pTextBlocks) {
		ZeroMemory(Chat_tb.m_pstrText, 256);
	}
}

void Lobby_GameScene::MakeRoom(std::string name)
{
	//Room_UI* room = new Room_UI(m_pd3dDevice, m_pd3dCommandList, m_pGraphicsRootSignature, 0, name, m_pPlayer->m_name);
	//Rooms.emplace_back(room);
	static int s_roomNum{};
	roomCreateList.push({ s_roomNum++, name, m_pPlayer->m_name });
}

void Lobby_GameScene::MakeRoom(int roomNum, std::string name, std::string owner)
{
	//Room_UI* room = new Room_UI(m_pd3dDevice, m_pd3dCommandList, m_pGraphicsRootSignature, roomNum, name, owner);
	//Rooms.emplace_back(room);

	roomCreateList.push({ roomNum, name, owner });
}
