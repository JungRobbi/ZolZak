#pragma once
#include <list>
#include "GameScene.h"
#include "Sound.h"
#include "Timer.h"

struct ItemInfo {
	int ItemNum, ItemID;
	float x, y, z;
public:
	ItemInfo() {}
	ItemInfo(int n, int i, float input_x, float input_y, float input_z) : ItemNum(n), ItemID(i), x(input_x), y(input_y), z(input_z) {}
	~ItemInfo() {}

};

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
	DEF_UI** HandDistance = {};
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

	std::list<ItemInfo> CreateItemList{};
};