#pragma once
#include "GameScene.h"
class Lobby_GameScene :
    public GameScene
{
public:
    void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

