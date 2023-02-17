#pragma once
#include "Timer.h"
#include "Camera.h"
#include "GameScene.h"
#include "Player.h"

#define MS_PER_UPDATE (1'000'000 / 60) // microsec


class GameFramework
{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;
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
public:
	Camera* m_pCamera = NULL;
	Player* m_pPlayer = NULL;
	ScreenShader* m_pScreen = NULL;
	Object* m_pSelectedObject = NULL;
	POINT m_ptOldCursorPos;

	GameFramework();
	~GameFramework();
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);

	void OnDestroy();
	void CreateSwapChain();
	void ChangeSwapChainState();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	void MoveToNextFrame();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
	void WaitForGpuComplete();

	void ProcessSelectedObject(DWORD dwDirection, float cxDelta, float cyDelta);
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,LPARAM lParam);

};

