#pragma once
#include <queue>
#include <string>
#include "GameScene.h"
struct ROOM_CREATE_STRUCT {
    int num;
    std::string name;
    std::string owner;
};

class Lobby_GameScene : public GameScene
{
public:
    virtual void update();
    std::vector<Room_UI*>Rooms;
    void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void RenderUI(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
    virtual void MakeRoom(std::string name);
    virtual void MakeRoom(int roomNum, std::string name, std::string owner);
    Room_UI** m_ppRooms = {};
    bool MakingRoom = false;
    ID3D12Device* m_pd3dDevice = NULL;
    ID3D12GraphicsCommandList* m_pd3dCommandList = NULL;
    int Page = 0;
    std::queue<ROOM_CREATE_STRUCT> roomCreateList;
};

