#pragma once
#include "GameScene.h"
#include "Sound.h"
#include "Timer.h"

class Stage_GameScene : public GameScene
{
private:
	float HearingDebuffTime = 0.0f;
	
public:
	void ReleaseObjects();
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	ID3D12Device* m_pd3dDevice = NULL;
	ID3D12GraphicsCommandList* m_pd3dCommandList = NULL;
	ATK_UI** ATKs = {};
	DEF_UI** DEFs = {};
	EYE_UI* m_Eye = NULL;
	EAR_UI* m_Ear = NULL;
	HAND_UI* m_Hand = NULL;

	void TouchStage(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void HearingStage(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void SightStage(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BossStage(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void RenderUI(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	void ClearMonster();

	void LoadAllModels(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void LoadAllTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void update();
public:
	Object* Boss = NULL;
	bool BossDead = false;
	float SceneChangeCount = 5.0f;

};