#pragma once
#include "GameScene.h"
#include "Sound.h"

class Stage_GameScene : public GameScene
{
public:
	Player_State_UI* m_pUI = NULL;
	Player_HP_UI* m_pHP_UI = NULL;
	Player_HP_DEC_UI* m_pHP_Dec_UI = NULL;


	Sound* bgm;
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

};