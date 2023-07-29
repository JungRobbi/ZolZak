#pragma once
#include <vector>

#include "Timer.h"
#include "Camera.h"
#include "GameScene.h"
#include "Player.h"
#include "Input.h"
#include "UI.h"
#include "NetworkMGR.h"
#include "UILayer.h"
#include "Vivox.h"
#include "ShadowMap.h"

#define MS_PER_UPDATE (1'000'000 / 60) // microsec

extern enum SCENE_TYPE
{
	LOGIN_SCENE = 0,
	LOBBY_SCENE = 1,
	ROOM_SCENE = 2,
	SIGHT_SCENE = 3,
	HEARING_SCENE = 4,
	TOUCH_SCENE = 5,
	BOSS_SCENE = 6
};

class GameFramework
{
private:
	HINSTANCE m_hInstance;


	int m_nWndClientWidth;
	int m_nWndClientHeight;
	_TCHAR m_FrameRate[50];

	IDXGIFactory4* m_pFactory;
	IDXGISwapChain3 *m_pSwapChain;
	ID3D12Device *m_pDevice;

	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0;

	static const UINT m_nSwapChainBuffers = 2;
	UINT m_nSwapChainBufferIndex;

	ID3D12Resource *m_ppRenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap* m_RTVDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_pSwapChainBackBufferRTVCPUHandles[m_nSwapChainBuffers];
	D3D12_CPU_DESCRIPTOR_HANDLE m_RTVDescriptorCPUHandle;

	ID3D12Resource *m_pDepthStencilBuffer;
	ID3D12DescriptorHeap* m_DSVDescriptorHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_DSVDescriptorCPUHandle;

	ID3D12CommandQueue *m_pCommandQueue;
	ID3D12CommandAllocator* m_pCommandAllocator;
	ID3D12GraphicsCommandList* m_pCommandList;

	ID3D12PipelineState *m_pPipelineState;

	ID3D12Fence *m_pFence;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_FenceEventHandle;

	VivoxSystem* m_pVivoxSystem;

	

public:
	static GameFramework* MainGameFramework;
public:
	std::vector<GameScene*> m_GameScenes;
	std::list<Player*> m_OtherPlayersPool;
	std::vector<Player*> m_OtherPlayers;
	int m_clearStage = 0;
	int ScriptNum = 0;
	int TalkingNPC = 0;
	float LastClick = 0;
	float MouseSen = 3;
	bool IsTouchDebuff = false;
	float TouchDebuffLeftTime = -1.0;
	int GameSceneState;
	float ShadowRange = 20;
public:
	HWND m_hWnd;
	Camera* m_pCamera = NULL;
	Player* m_pPlayer = NULL;
	ScreenShader* m_pScreen = NULL;
	DebugShader* m_pDebug = NULL;
	Object* m_pSelectedObject = NULL;
	POINT m_ptOldCursorPos;
	SCENE_TYPE scene_type;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_BlendShadowCPUSRV;
	D3D12_GPU_DESCRIPTOR_HANDLE	m_BlendShadowGPUSRV;

	ID3D12Resource* m_pShadowCamera = NULL;
	VS_CB_CAMERA_INFO* m_pShadowMappedCamera = NULL;

	GameFramework();
	~GameFramework();
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);

	void OnDestroy();
	void CreateLight();
	void CreateSwapChain();
	void ChangeSwapChainState();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	void CreateShadowMap();
	void MoveToNextFrame();
	void InitializeVivoxSystem(std::string UserName);

	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	void ChangeToSpaceShipCamera() { m_pCamera = m_pPlayer->ChangeCamera(SPACESHIP_CAMERA, Timer::GetTimeElapsed()); }

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
	void RenderHP();
	void WaitForGpuComplete();

	void Touch_Debuff(float time);

	void ProcessSelectedObject(DWORD dwDirection, float cxDelta, float cyDelta);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,LPARAM lParam);
	void SetWindowCentser(RECT rect);
	void ChangeScene(unsigned char num);
	void ChangeStage(unsigned char num);

	void SaveSceneOBB();
	VivoxSystem* GetVivoxSystem();

	UINT GetSwapChainBufferIndex() { return m_nSwapChainBufferIndex; }
	const UINT GetSwapChainBuffers() { return m_nSwapChainBuffers; }
	ID3D12Device* GetDevice() { return m_pDevice; }
	ID3D12CommandQueue* GetCommandQueue() { return m_pCommandQueue; }
	ID3D12Resource** GetRenderTargetBuffers() { return m_ppRenderTargetBuffers; }
	int GetWndClientWidth() { return m_nWndClientWidth; }
	int GetWndClientHeight() { return m_nWndClientHeight; }

	friend NetworkMGR;

	ShadowMap* m_ShadowMap = NULL;
	RECT WindowPos;
	POINT CenterOfWindow;
};

