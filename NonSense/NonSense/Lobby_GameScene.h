#pragma once
#include "GameScene.h"
class Lobby_GameScene : public GameScene
{
public:
    std::vector<Room_UI*>Rooms;
    void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void RenderUI(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
    virtual void MakeRoom(std::string name);
    virtual void MakeRoom(int roomNum, std::string name, std::string owner);
    Make_Title_UI* m_Make_Title_UI = NULL;
    bool MakingRoom = false;
    ID3D12Device* m_pd3dDevice = NULL;
    ID3D12GraphicsCommandList* m_pd3dCommandList = NULL;
    int Page = 0;
    int SelectNum = 0;
};

