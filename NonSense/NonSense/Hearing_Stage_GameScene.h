#pragma once
#include "Sound.h"
#include "Stage_GameScene.h"
class Hearing_Stage_GameScene : public Stage_GameScene
{
public:
	void ReleaseObjects();
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	ID3D12Device* m_pd3dDevice = NULL;
	ID3D12GraphicsCommandList* m_pd3dCommandList = NULL;
};