#pragma once
#include "GameScene.h"
#include "Sound.h"
#include "Timer.h"

class Stage_GameScene : public GameScene
{

	
public:
	void ReleaseObjects();
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	ID3D12Device* m_pd3dDevice = NULL;
	ID3D12GraphicsCommandList* m_pd3dCommandList = NULL;


	void TouchStage(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void HearingStage(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void SightStage(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BossStage(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void ClearMonster();

	void LoadAllModels(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void LoadAllTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void update();
public:
	Object* Boss = NULL;
	bool BossDead = false;
	float SceneChangeCount = 5.0f;
};