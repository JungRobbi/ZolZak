#pragma once
#include "GameScene.h"
class Lobby_GameScene :
    public GameScene
{
public:
    void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void RenderUI(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
    Make_Title_UI* m_Make_Title_UI = NULL;
    bool MakingRoom = false;
};

