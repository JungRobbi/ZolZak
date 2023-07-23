#include "stdafx.h"
#include <fstream>
#include "GameFramework.h"
#include "PlayerMovementComponent.h"
#include "BoxCollideComponent.h"
#include "SphereCollideComponent.h"
#include "AttackComponent.h"

#include "Input.h"

#include "Login_GameScene.h"
#include "Lobby_GameScene.h"
#include "Room_GameScene.h"
#include "Stage_GameScene.h"
#include "Boss_Stage_GameScene.h"
#include "Hearing_Stage_GameScene.h"
#include "Sight_Stage_GameScene.h"
#include "Touch_Stage_GameScene.h"

#include "resource.h"
#include "Sound.h"
GameFramework* GameFramework::MainGameFramework;

GameFramework::GameFramework()
{
	m_pFactory = NULL;
	m_pSwapChain = NULL;
	m_pDevice = NULL;
	m_pCommandAllocator = NULL;
	m_pCommandQueue = NULL;
	m_pPipelineState = NULL;
	m_pCommandList = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_ppRenderTargetBuffers[i] = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;
	m_RTVDescriptorHeap = NULL;
	m_pDepthStencilBuffer = NULL;
	m_DSVDescriptorHeap = NULL;
	m_nSwapChainBufferIndex = 0;
	m_FenceEventHandle = NULL;
	m_pFence = NULL;
	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;
	_tcscpy_s(m_FrameRate, _T("NonSense("));
	
	MainGameFramework = this;
	Timer::Initialize();
}

GameFramework::~GameFramework()
{
}

bool GameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	CreateDirect3DDevice();				// Device ����
	CreateCommandQueueAndList();		// Command ť, ����Ʈ ����
	CreateRtvAndDsvDescriptorHeaps();	// RTV, DSV Descriptor Heap ����
	CreateSwapChain();					// Swap Chain ����
#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetViews();			// Render Target View ����
#endif
	CreateDepthStencilView();			// Depth Stencil View ����
	
	NetworkMGR::start();
	Sound::InitFmodSystem();			// FMOD System �ʱ�ȭ
	CreateLight();
	BuildObjects();						// Object ����
	CreateShadowMap();
	return(true);
}

void GameFramework::CreateLight()
{
	UINT ncbElementBytes = ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255); //256의 배수
	m_pShadowCamera = ::CreateBufferResource(m_pDevice, m_pCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pShadowCamera->Map(0, NULL, (void**)&m_pShadowMappedCamera);

}

void GameFramework::OnDestroy()
{
	WaitForGpuComplete();

	ReleaseObjects();

	::CloseHandle(m_FenceEventHandle);

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppRenderTargetBuffers[i]) m_ppRenderTargetBuffers[i]->Release();
	if (m_RTVDescriptorHeap) m_RTVDescriptorHeap->Release();
	if (m_pDepthStencilBuffer) m_pDepthStencilBuffer->Release();
	if (m_DSVDescriptorHeap) m_DSVDescriptorHeap->Release();
	if (m_pCommandAllocator) m_pCommandAllocator->Release();
	if (m_pCommandQueue) m_pCommandQueue->Release();
	if (m_pPipelineState) m_pPipelineState->Release();
	if (m_pCommandList) m_pCommandList->Release();
	if (m_pFence) m_pFence->Release();
	m_pSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pSwapChain) m_pSwapChain->Release();
	if (m_pDevice) m_pDevice->Release();
	if (m_pFactory) m_pFactory->Release();

	if (m_pVivoxSystem)
	{
		m_pVivoxSystem->Disconnect();
		m_pVivoxSystem->Uninitialize();
		delete m_pVivoxSystem;
	}
	m_GameScenes.clear();

	Sound::ReleaseFmodSystem();		// FMOD System ����
#ifdef defined(_DEBUG)
	IDXGIDebug1* pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
		DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

void GameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hResult = m_pFactory->CreateSwapChain(m_pCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain**)&m_pSwapChain);

	hResult = m_pFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	m_nSwapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

void GameFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();
	BOOL bFullScreenState = TRUE;
	m_pSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	m_pSwapChain->SetFullscreenState(!bFullScreenState, NULL);
	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = m_nWndClientWidth;
	dxgiTargetParameters.Height = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppRenderTargetBuffers[i]) m_ppRenderTargetBuffers[i]->Release();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_nSwapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}
void GameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;

	UINT nDXGIFactoryFlags = 0;
#if defined(_DEBUG)
	ID3D12Debug* pd3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&pd3dDebugController);
	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void**)&m_pFactory);

	IDXGIAdapter1* pd3dAdapter = NULL;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void**)&m_pDevice))) break;
	}

	if (!m_pDevice)
	{
		hResult = m_pFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void**)&pd3dAdapter);
		hResult = D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)&m_pDevice);
	}

	if (!m_pDevice)
	{
		MessageBox(NULL, L"Direct3D 12 Device Cannot be Created.", L"Error", MB_OK);
		::PostQuitMessage(0);
		return;
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	hResult = m_pDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	hResult = m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pFence);
	for (UINT i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 1;
	m_FenceEventHandle = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	::RTVDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	::CBVSRVDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	::DSVDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	if (pd3dAdapter) pd3dAdapter->Release();
}

void GameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hResult = m_pDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void**)&m_pCommandQueue);

	hResult = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pCommandAllocator);
	hResult = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pCommandList);
	hResult = m_pCommandList->Close();
}

void GameFramework::CreateShadowMap()
{
	m_ShadowMap = new ShadowMap(m_pDevice, 2048, 2048);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dDsvCPUDescriptorHandle.ptr += (::DSVDescriptorSize);

	/// 

	m_ShadowMap->BuildDescriptors(m_pScreen->m_SRVCPUDescriptorNextHandle, m_pScreen->m_SRVGPUDescriptorNextHandle, d3dDsvCPUDescriptorHandle);

	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc{};

		UINT nInputElementDescs = 1;
		D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

		pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

		D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
		d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
		d3dInputLayoutDesc.NumElements = nInputElementDescs;

		PsoDesc.InputLayout = d3dInputLayoutDesc;

		PsoDesc.pRootSignature = GameScene::MainScene->m_pGraphicsRootSignature;
		ID3DBlob* ppd3dShaderBlob = NULL;
		ID3DBlob* pd3dPixelShaderBlob = NULL;
		PsoDesc.VS = CompileShaderFromFile(L"Shaders.hlsl", "VSShadowMap", "vs_5_1", &ppd3dShaderBlob);
		PsoDesc.PS = CompileShaderFromFile(L"Shaders.hlsl", "PSShadowMap", "ps_5_1", &pd3dPixelShaderBlob);
		{
			PsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
			PsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
			PsoDesc.RasterizerState.FrontCounterClockwise = FALSE;
			PsoDesc.RasterizerState.DepthBias = 100000;
			PsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
			PsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
			PsoDesc.RasterizerState.DepthClipEnable = TRUE;
			PsoDesc.RasterizerState.MultisampleEnable = FALSE;
			PsoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
			PsoDesc.RasterizerState.ForcedSampleCount = 0;
			PsoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

			PsoDesc.BlendState.AlphaToCoverageEnable = FALSE;
			PsoDesc.BlendState.IndependentBlendEnable = FALSE;
			PsoDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
			PsoDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
			PsoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
			PsoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
			PsoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			PsoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
			PsoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
			PsoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			PsoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
			PsoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

			PsoDesc.DepthStencilState.DepthEnable = TRUE;
			PsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			PsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
			PsoDesc.DepthStencilState.StencilEnable = FALSE;
			PsoDesc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
			PsoDesc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
			PsoDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
			PsoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
			PsoDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
			PsoDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
			PsoDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
			PsoDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
			PsoDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
			PsoDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		}

		PsoDesc.SampleMask = UINT_MAX;
		PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PsoDesc.NumRenderTargets = 0;
		PsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		PsoDesc.SampleDesc.Count = 1;
		PsoDesc.SampleDesc.Quality = 0;
		PsoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		HRESULT hResult = m_pDevice->CreateGraphicsPipelineState(&PsoDesc, __uuidof(ID3D12PipelineState), (void**)&m_pPipelineState);

		if (ppd3dShaderBlob) ppd3dShaderBlob->Release();
		if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

		if (PsoDesc.InputLayout.pInputElementDescs) delete[] PsoDesc.InputLayout.pInputElementDescs;
	}

	/// 

}

void GameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;	// Descriptor Heap�� ����� �ش�.
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC)); // ������� Descriptor Heap�� 0���� �ʱ�ȭ �����ش�.
	// Render Target Descriptor Heap ����
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers + MRT; // Descriptor�� ������ m_nSwapChainBuffers (2��)
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // Render Target View�� Descriptor ����
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_RTVDescriptorHeap); // Render Target Descriptor Heap ����

	// Depth/Stencil Descriptor Heap ����
	d3dDescriptorHeapDesc.NumDescriptors = 2; // Descriptor�� ������ 1��
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; // Depth/Stencil View�� Descriptor ����
	hResult = m_pDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_DSVDescriptorHeap); // Depth/Stencil Descriptor Heap ����
}

void GameFramework::CreateRenderTargetViews()
{
	D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc;
	d3dRenderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
	d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE m_RTVDescriptorCPUHandle = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_ppRenderTargetBuffers[i]);
		m_pDevice->CreateRenderTargetView(m_ppRenderTargetBuffers[i], NULL, m_RTVDescriptorCPUHandle);
		m_pSwapChainBackBufferRTVCPUHandles[i] = m_RTVDescriptorCPUHandle;
		m_RTVDescriptorCPUHandle.ptr += RTVDescriptorSize;
	}
}

void GameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	m_pDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void**)&m_pDepthStencilBuffer);

	m_DSVDescriptorCPUHandle = m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, NULL, m_DSVDescriptorCPUHandle);
}

void GameFramework::BuildObjects()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (::RTVDescriptorSize * m_nSwapChainBuffers);

	ChatMGR::m_pUILayer = new UILayer(m_nSwapChainBuffers, 10, m_pDevice, m_pCommandQueue, m_ppRenderTargetBuffers, m_nWndClientWidth, m_nWndClientHeight);
	ChatMGR::SetTextinfos(m_nWndClientWidth, m_nWndClientHeight);
	// m_GameScenes[0] : Login | m_GameScenes[1] : Lobby | m_GameScenes[2] : Room | m_GameScenes[3] : Stage

	m_GameScenes.emplace_back(new Login_GameScene());
	m_GameScenes.emplace_back(new Lobby_GameScene());
	m_GameScenes.emplace_back(new Room_GameScene());
	m_GameScenes.emplace_back(new Sight_Stage_GameScene());
	m_GameScenes.emplace_back(new Hearing_Stage_GameScene());
	m_GameScenes.emplace_back(new Touch_Stage_GameScene());
	m_GameScenes.emplace_back(new Boss_Stage_GameScene());
	ChangeScene(LOGIN_SCENE);

	m_pCommandList->Reset(m_pCommandAllocator, NULL);

	m_pDebug = new DebugShader();
	m_pScreen = new ScreenShader();
	m_pScreen->CreateShader(m_pDevice, GameScene::MainScene->GetGraphicsRootSignature(), 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	m_pDebug->CreateShader(m_pDevice, GameScene::MainScene->GetGraphicsRootSignature(), 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);

	DXGI_FORMAT pdxgiResourceFormats[MRT - 1] = { DXGI_FORMAT_R32G32B32A32_FLOAT,  DXGI_FORMAT_R32G32B32A32_FLOAT,  DXGI_FORMAT_R8G8B8A8_UNORM };
	m_pScreen->CreateResourcesAndViews(m_pDevice, MRT - 1, pdxgiResourceFormats, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, d3dRtvCPUDescriptorHandle, MRT); //SRV to (Render Targets) + (Depth Buffer)
	DXGI_FORMAT pdxgiDepthSrvFormats[1] = { DXGI_FORMAT_R24_UNORM_X8_TYPELESS };
	m_pScreen->CreateShaderResourceViews(m_pDevice, 1, &m_pDepthStencilBuffer, pdxgiDepthSrvFormats);
	m_pCommandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	WaitForGpuComplete();

	for (auto& gameScene : m_GameScenes) {
		gameScene->ReleaseUploadBuffers();
	}
	if (m_pPlayer)
		m_pPlayer->ReleaseUploadBuffers();
	for(auto& p : m_OtherPlayersPool)
		p->ReleaseUploadBuffers();
	m_pPlayer->GetComponent<PlayerMovementComponent>()->CursorExpose = true;
	Timer::Reset();
}

void GameFramework::ReleaseObjects()
{
	GameScene::MainScene->ReleaseObjects();

	if (ChatMGR::m_pUILayer) ChatMGR::m_pUILayer->ReleaseResources();
	if (ChatMGR::m_pUILayer) delete ChatMGR::m_pUILayer;
}

void GameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (NetworkMGR::b_isNet) {
		if (nMessageID == WM_RBUTTONDOWN || nMessageID == WM_LBUTTONDOWN) {
			CS_KEYDOWN_PACKET send_packet;
			send_packet.size = sizeof(CS_KEYDOWN_PACKET);
			send_packet.type = E_PACKET::E_PACKET_CS_KEYDOWN;
			send_packet.key = wParam;
			PacketQueue::AddSendPacket(&send_packet);

		}
		else if (nMessageID == WM_RBUTTONUP || nMessageID == WM_LBUTTONUP) {
			CS_KEYUP_PACKET send_packet;
			send_packet.size = sizeof(CS_KEYUP_PACKET);
			send_packet.type = E_PACKET::E_PACKET_CS_KEYUP;
			send_packet.key = wParam;
			PacketQueue::AddSendPacket(&send_packet);
		}
	}

	GameScene::MainScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		//���콺�� �������� ���콺 ��ŷ�� �Ͽ� ������ ���� ��ü�� ã�´�.
		m_pSelectedObject = GameScene::MainScene->PickObjectPointedByCursor(LOWORD(lParam), HIWORD(lParam), m_pCamera);
		//���콺 ĸ�ĸ� �ϰ� ���� ���콺 ��ġ�� �����´�.
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();

		break;
	}
}
void GameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	GameScene::MainScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	if (NetworkMGR::b_isNet && wParam != VK_RETURN) {
		if (nMessageID == WM_KEYDOWN) {
			if (Input::keys[wParam] != TRUE) {
				CS_KEYDOWN_PACKET send_packet;
				send_packet.size = sizeof(CS_KEYDOWN_PACKET);
				send_packet.type = E_PACKET::E_PACKET_CS_KEYDOWN;
				send_packet.key = wParam;
				PacketQueue::AddSendPacket(&send_packet);
				Input::keys[wParam] = TRUE;
			}
		}
		else if (nMessageID == WM_KEYUP) {
			if (Input::keys[wParam] != FALSE) {
				CS_KEYUP_PACKET send_packet;
				send_packet.size = sizeof(CS_KEYUP_PACKET);
				send_packet.type = E_PACKET::E_PACKET_CS_KEYUP;
				send_packet.key = wParam;
				PacketQueue::AddSendPacket(&send_packet);
				Input::keys[wParam] = FALSE;
			}
		}
	}

	HIMC hIMC;
	DWORD dwConversion, dwSentence;

	hIMC = ImmGetContext(hWnd);
	ImmGetConversionStatus(hIMC, &dwConversion, &dwSentence);

	int len{};

	if (ChatMGR::m_ChatMode == E_MODE_CHAT::E_MODE_CHAT) { // Chat Mode
		switch (nMessageID)
		{
		case WM_IME_COMPOSITION:
			if (ChatMGR::m_HangulMode == E_MODE_HANGUL::E_MODE_HANGUL) {
				WCHAR ch;
				if (lParam & GCS_COMPSTR) {
					len = ImmGetCompositionString(hIMC, GCS_COMPSTR, NULL, 0);
					ImmGetCompositionString(hIMC, GCS_COMPSTR, &ChatMGR::m_combtext, len);
					memcpy(&ChatMGR::m_textbuf[ChatMGR::m_textindex], &ChatMGR::m_combtext, sizeof(ChatMGR::m_combtext));
				}
				if (lParam & GCS_RESULTSTR) {
					len = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0);
					ImmGetCompositionString(hIMC, GCS_RESULTSTR, &ch, len);
					memcpy(&ChatMGR::m_textbuf[ChatMGR::m_textindex++], &ch, sizeof(ch));
				}
			}
			break;
		case WM_KEYUP:
			switch (wParam)
			{
			case VK_RETURN:
				ChatMGR::m_ChatMode = E_MODE_CHAT::E_MODE_PLAY;
				ChatMGR::StoreTextSelf();
				ChatMGR::m_textindex = 0;

				if (NetworkMGR::b_isNet && scene_type == LOGIN_SCENE) {
					char* p = ConvertWCtoC(ChatMGR::m_textbuf);
					NetworkMGR::name = string{ p };
					delete[] p;
					if(!NetworkMGR::b_isNet) // 클라 모드일 때 
						ChangeScene(SIGHT_SCENE);
					else if (!NetworkMGR::b_isLogin && !NetworkMGR::b_isLoginProg) { // 로그인 하지 않은 상태
						NetworkMGR::b_isLoginProg = true; // 로그인 진행
						CS_LOGIN_PACKET send_packet;
						send_packet.size = sizeof(CS_LOGIN_PACKET);
						send_packet.type = E_PACKET::E_PACKET_CS_LOGIN;
						memcpy(send_packet.name, NetworkMGR::name.c_str(), NetworkMGR::name.size());
						PacketQueue::AddSendPacket(&send_packet);
					}
					else {
						//로그인 실패
						cout << "로그인 시도 실패!" << endl;
						cout << "Login Try Fail!" << endl;
					}

				}
				ZeroMemory(ChatMGR::m_textbuf, sizeof(ChatMGR::m_textbuf));
				break;
			case VK_BACK:
				if (ChatMGR::m_textindex > 0)
					--ChatMGR::m_textindex;
				ChatMGR::m_textbuf[ChatMGR::m_textindex] = NULL;
				break;
			case VK_HANGUL:
				if (ChatMGR::m_HangulMode == E_MODE_HANGUL::E_MODE_ENGLISH) {
					ChatMGR::m_HangulMode = E_MODE_HANGUL::E_MODE_HANGUL;
				}
				else {
					ChatMGR::m_HangulMode = E_MODE_HANGUL::E_MODE_ENGLISH;
				}
				break;
			case ' ':
				ChatMGR::m_textbuf[ChatMGR::m_textindex++] = wParam;
				break;
			default:
				if (ChatMGR::m_HangulMode == E_MODE_HANGUL::E_MODE_ENGLISH) {
					if (isalpha(wParam)) {
						ChatMGR::m_textbuf[ChatMGR::m_textindex++] = wParam;
					}
				}
				break;
			}
			break;

		default:
			break;
		}
	}
	else {
		switch (nMessageID)
		{
		case WM_KEYUP:
			switch (wParam)
			{

			case VK_F1:
			case VK_F2:
			case VK_F3:
				if (m_pPlayer) m_pCamera = m_pPlayer->ChangeCamera((wParam - VK_F1 + 1), Timer::GetTimeElapsed());
				break;
			case VK_ESCAPE:
				if (OptionMode)
				{
					m_pPlayer->GetComponent<PlayerMovementComponent>()->CursorExpose = false;
					OptionMode = false;
				}
				else {
					m_pPlayer->GetComponent<PlayerMovementComponent>()->CursorExpose = true;
					OptionMode = true;
				}
				break;
			case VK_RETURN:
				PostQuitMessage(0);
				break;
			case VK_F8:
				(DebugMode) ? (DebugMode = 0) : (DebugMode = 1);
				break;
			case VK_F9:
				ChangeSwapChainState();
				break;
			case VK_F11: // �� �ٿ�� �ڽ� ���Ϸ� ����
				SaveSceneOBB();
				break;
			case '2':
				cout << m_pPlayer->GetPosition().x << ", " << m_pPlayer->GetPosition().y << ", " << m_pPlayer->GetPosition().z << endl;
				break;
			case 'f':	// 상호작용
			case 'F':
				if (scene_type >= SIGHT_SCENE)
				{
					if (m_pPlayer->GetComponent<SphereCollideComponent>()->GetBoundingObject()->Intersects(*GameScene::MainScene->StartNPC->GetComponent<SphereCollideComponent>()->GetBoundingObject()))	// Start NPC
					{
						if (!ScriptMode)	// 대화 시작
						{
							m_pCamera = m_pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, Timer::GetTimeElapsed());
							ScriptMode = true;
							TalkingNPC = 1;
						//	cout << GameScene::MainScene->StartNPC->script[0] << endl;
						}
						else
						{
							ScriptNum++;
							if (ScriptNum >= GameScene::MainScene->StartNPC->script.size())	// 대화 끝
							{
								::SetCursorPos(CenterOfWindow.x, CenterOfWindow.y);
								m_pCamera = m_pPlayer->ChangeCamera(FIRST_PERSON_CAMERA, Timer::GetTimeElapsed());
								ScriptMode = false;
								ScriptNum = 0;
								TalkingNPC = 0;
								break;
							}
						//	cout << GameScene::MainScene->StartNPC->script[ScriptNum] << endl;
						}
					}

					if (m_pPlayer->GetComponent<SphereCollideComponent>()->GetBoundingObject()->Intersects(*GameScene::MainScene->EndNPC->GetComponent<SphereCollideComponent>()->GetBoundingObject()))		// End NPC
					{
						if (!ScriptMode)	// 대화 시작
						{
							m_pCamera = m_pPlayer->ChangeCamera(THIRD_PERSON_CAMERA, Timer::GetTimeElapsed());
							ScriptMode = true;
							TalkingNPC = 2;
						//	cout << GameScene::MainScene->EndNPC->script[0] << endl;
						}
						else
						{
							ScriptNum++;
							if (ScriptNum >= GameScene::MainScene->EndNPC->script.size())	// 대화 끝
							{
								::SetCursorPos(CenterOfWindow.x, CenterOfWindow.y);
								m_pCamera = m_pPlayer->ChangeCamera(FIRST_PERSON_CAMERA, Timer::GetTimeElapsed());
								ScriptMode = false;
								ScriptNum = 0;
								TalkingNPC = 0;
								break;
							}
						//	cout << GameScene::MainScene->EndNPC->script[ScriptNum] << endl;
						}
					}
				}
				break;
			case 'G':
			case 'g':
				delete m_pPlayer;
				m_pPlayer = new WarriorPlayer(m_pDevice, m_pCommandList, GameScene::MainScene->GetGraphicsRootSignature(), GameScene::MainScene->GetTerrain());
				m_pCamera = m_pPlayer->GetCamera();
				GameScene::MainScene->m_pPlayer = m_pPlayer;
				break;
			case '7':
				ChangeScene(0);
				break;
			case '8':
				ChangeScene(1);
				break;
			case '9':
				ChangeScene(SIGHT_SCENE);
				break;
			case 't':
			case 'T':
				ChatMGR::m_ChatMode = E_MODE_CHAT::E_MODE_CHAT;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}
LRESULT CALLBACK GameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		m_nWndClientWidth = FRAME_BUFFER_WIDTH;
		m_nWndClientHeight = FRAME_BUFFER_HEIGHT;
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}
void GameFramework::SetWindowCentser(RECT rect)
{
	WindowPos = rect;
	CenterOfWindow.x = (rect.right - rect.left) / 2;
	CenterOfWindow.y = (rect.bottom - rect.top) / 2;
}
void GameFramework::ChangeScene(unsigned char num)
{
	m_pCommandList->Reset(m_pCommandAllocator, NULL);

	GameScene::MainScene->ReleaseObjects();
	GameScene::MainScene = m_GameScenes.at(num);
	GameScene::MainScene->BuildObjects(m_pDevice, m_pCommandList);

	if (NetworkMGR::is_mage)
	m_pPlayer = new MagePlayer(m_pDevice, m_pCommandList, GameScene::MainScene->GetGraphicsRootSignature(), GameScene::MainScene->GetTerrain());
	else m_pPlayer = new WarriorPlayer(m_pDevice, m_pCommandList, GameScene::MainScene->GetGraphicsRootSignature(), GameScene::MainScene->GetTerrain());

	switch (num)
	{
	case LOGIN_SCENE:
		if (m_pPlayer)
		m_pPlayer->GetComponent<PlayerMovementComponent>()->CursorExpose = true;
		break;
	case LOBBY_SCENE:
		if(m_pPlayer)
		m_pPlayer->GetComponent<PlayerMovementComponent>()->CursorExpose = true;
		break;
	case ROOM_SCENE:
		if (m_pPlayer)
			m_pPlayer->GetComponent<PlayerMovementComponent>()->CursorExpose = true;
		break;
	case SIGHT_SCENE:
	case HEARING_SCENE:
	case TOUCH_SCENE:
	case BOSS_SCENE:
		if (m_pPlayer)
		m_pPlayer->GetComponent<PlayerMovementComponent>()->CursorExpose = false;
		break;
	default:
		break;
	}

	GameSceneState = num;


	if (num != LOGIN_SCENE) {
		m_OtherPlayers.clear();
		m_OtherPlayersPool.clear();
		for (int i{}; i < 3; ++i) {
			if (i == 1) m_OtherPlayersPool.emplace_back(new WarriorPlayer(m_pDevice, m_pCommandList, GameScene::MainScene->GetGraphicsRootSignature(), GameScene::MainScene->GetTerrain()));
			else m_OtherPlayersPool.emplace_back(new MagePlayer(m_pDevice, m_pCommandList, GameScene::MainScene->GetGraphicsRootSignature(), GameScene::MainScene->GetTerrain()));
			dynamic_cast<Player*>(m_OtherPlayersPool.back())->SetCamera(dynamic_cast<Player*>(m_OtherPlayersPool.back())->ChangeCamera(THIRD_PERSON_CAMERA, 0.0f));
			m_OtherPlayersPool.back()->SetUsed(true);
		}
	}

	ChatMGR::m_ChatMode = E_MODE_CHAT::E_MODE_PLAY;
	if (num >= SIGHT_SCENE) {
		ChatMGR::SetInGame(m_nWndClientWidth, m_nWndClientHeight);
	}
	else if (num == LOGIN_SCENE) {
		ChatMGR::SetLoginScene(m_nWndClientWidth, m_nWndClientHeight);
	}
	for (auto& Chat_tb : ChatMGR::m_pUILayer->m_pTextBlocks) {
		ZeroMemory(Chat_tb.m_pstrText, 256);
	}
	scene_type = (SCENE_TYPE)num;
	m_pCamera = m_pPlayer->GetCamera();
	GameScene::MainScene->m_pPlayer = m_pPlayer;
	m_pCommandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	WaitForGpuComplete();
}

VivoxSystem* GameFramework::GetVivoxSystem()
{
	return m_pVivoxSystem;
}

void GameFramework::ProcessSelectedObject(DWORD dwDirection, float cxDelta, float cyDelta)
{
	//��ŷ���� ������ ���� ��ü�� ������ Ű���带 �����ų� ���콺�� �����̸� ���� ��ü�� �̵� �Ǵ� ȸ���Ѵ�.
	if (dwDirection != 0)
	{
		if (dwDirection & DIR_FORWARD) m_pSelectedObject->MoveForward(+1.0f);
		if (dwDirection & DIR_BACKWARD) m_pSelectedObject->MoveForward(-1.0f);
		if (dwDirection & DIR_LEFT) m_pSelectedObject->MoveStrafe(+1.0f);
		if (dwDirection & DIR_RIGHT) m_pSelectedObject->MoveStrafe(-1.0f);
		if (dwDirection & DIR_UP) m_pSelectedObject->MoveUp(+1.0f);
		if (dwDirection & DIR_DOWN) m_pSelectedObject->MoveUp(-1.0f);
	}
	else if ((cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		m_pSelectedObject->Rotate(cyDelta, cxDelta, 0.0f);
	}
}

void GameFramework::ProcessInput()
{
	if (!ScriptMode) {
		Input::update();
		static UCHAR pKeyBuffer[256];
		DWORD dwDirection = 0;
		if (::GetKeyboardState(pKeyBuffer))
		{
			if (pKeyBuffer[0x57] & 0xF0) dwDirection |= DIR_FORWARD;
			if (pKeyBuffer[0x53] & 0xF0) dwDirection |= DIR_BACKWARD;
			if (pKeyBuffer[0x41] & 0xF0) dwDirection |= DIR_LEFT;
			if (pKeyBuffer[0x44] & 0xF0) dwDirection |= DIR_RIGHT;
			if (pKeyBuffer[0x45] & 0xF0) dwDirection |= DIR_UP;
			if (pKeyBuffer[0x51] & 0xF0) dwDirection |= DIR_DOWN;
		}
		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;

		if (!m_pPlayer->GetComponent<PlayerMovementComponent>()->CursorExpose)
		{
			::SetCapture(m_hWnd);
			RECT rect;
			::GetWindowRect(m_hWnd, &rect);
			SetWindowCentser(rect);
			::SetCursor(NULL);
			::GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - CenterOfWindow.x) / MouseSen;
			cyDelta = (float)(ptCursorPos.y - CenterOfWindow.y) / MouseSen;

			::SetCursorPos(CenterOfWindow.x, CenterOfWindow.y);
		}
		else
		{
			::SetCursor(LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_CURSOR1)));
			if (::GetCapture() == m_hWnd)
			{
				RECT rect;
				::GetCursorPos(&ptCursorPos);
				::GetWindowRect(m_hWnd, &rect);
				if (((scene_type < SIGHT_SCENE) || (scene_type >= SIGHT_SCENE && OptionMode)) && (Timer::GetTotalTime() - LastClick > 0.2)) {
					float px = (ptCursorPos.x - rect.left - 10) / (float)FRAME_BUFFER_WIDTH;
					float py = (ptCursorPos.y - rect.top - 30) / (float)FRAME_BUFFER_HEIGHT;

					for (auto& ui : GameScene::MainScene->UIGameObjects)
					{
						if (px >= dynamic_cast<UI*>(ui)->XYWH._41 && px <= dynamic_cast<UI*>(ui)->XYWH._41 + dynamic_cast<UI*>(ui)->XYWH._11 &&
							py <= 1 - dynamic_cast<UI*>(ui)->XYWH._42 && py >= 1 - (dynamic_cast<UI*>(ui)->XYWH._42 + dynamic_cast<UI*>(ui)->XYWH._22))
						{
							dynamic_cast<UI*>(ui)->OnClick();
						}
					}
					if (scene_type == LOBBY_SCENE)
					{
						for (auto& room : dynamic_cast<Lobby_GameScene*>(GameScene::MainScene)->Rooms)
						{
							if (px >= room->XYWH._41 && px <= room->XYWH._41 + room->XYWH._11 &&
								py <= 1 - room->XYWH._42 && py >= 1 - (room->XYWH._42 + room->XYWH._22))
							{
								room->OnClick();
							}
						}
					}
					LastClick = Timer::GetTotalTime();
				}
			}
			else {
				RECT rect;
				::GetCursorPos(&ptCursorPos);
				::GetWindowRect(m_hWnd, &rect);
				float px = (ptCursorPos.x - rect.left - 10) / (float)FRAME_BUFFER_WIDTH;
				float py = (ptCursorPos.y - rect.top - 30) / (float)FRAME_BUFFER_HEIGHT;

				for (auto& ui : GameScene::MainScene->UIGameObjects)
				{
					if (px >= dynamic_cast<UI*>(ui)->XYWH._41 && px <= dynamic_cast<UI*>(ui)->XYWH._41 + dynamic_cast<UI*>(ui)->XYWH._11 &&
						py <= 1 - dynamic_cast<UI*>(ui)->XYWH._42 && py >= 1 - (dynamic_cast<UI*>(ui)->XYWH._42 + dynamic_cast<UI*>(ui)->XYWH._22))
					{
						if (dynamic_cast<UI*>(ui)->CanClick) {
							dynamic_cast<UI*>(ui)->MouseOn = true;
						}
					}
					else {
						dynamic_cast<UI*>(ui)->MouseOn = false;
					}
				}
				if (scene_type == LOBBY_SCENE)
				{
					for (auto& room : dynamic_cast<Lobby_GameScene*>(GameScene::MainScene)->Rooms)
					{
						if (px >= room->XYWH._41 && px <= room->XYWH._41 + room->XYWH._11 &&
							py <= 1 - room->XYWH._42 && py >= 1 - (room->XYWH._42 + room->XYWH._22))
						{
							room->MouseOn = true;
						}
						else {
							room->MouseOn = false;
						}
					}
				}
			}
		}
		if (cxDelta || cyDelta)
		{
			m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}

		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (m_pSelectedObject)
			{
				ProcessSelectedObject(dwDirection, cxDelta, cyDelta);
			}
			else
			{

				if (cxDelta || cyDelta)
				{

				}
				if (dwDirection) {
					m_pPlayer->Move(dwDirection, 50.0f * Timer::GetTimeElapsed(), true);

				}
			}
		}
	}
}

void GameFramework::AnimateObjects()
{
	GameScene::MainScene->AnimateObjects(Timer::GetTimeElapsed());
}

void GameFramework::WaitForGpuComplete()
{
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pCommandQueue->Signal(m_pFence, nFenceValue);
	if (m_pFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pFence->SetEventOnCompletion(nFenceValue, m_FenceEventHandle);
		::WaitForSingleObject(m_FenceEventHandle, INFINITE);
	}
}

void GameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pCommandQueue->Signal(m_pFence, nFenceValue);
	if (m_pFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pFence->SetEventOnCompletion(nFenceValue, m_FenceEventHandle);
		::WaitForSingleObject(m_FenceEventHandle, INFINITE);
	}
}

void GameFramework::InitializeVivoxSystem(std::string UserName)
{
	m_pVivoxSystem = new VivoxSystem(UserName.c_str());
	m_pVivoxSystem->Initialize();
}

void GameFramework::Touch_Debuff(float time)
{
	IsTouchDebuff = true;
	TouchDebuffLeftTime = time;
}

void GameFramework::FrameAdvance()
{
	Timer::Tick(0.0f);

	m_pVivoxSystem->Listen();

	Sound::SystemUpdate(); //FMOD System Update


	if (NetworkMGR::b_isNet)
		NetworkMGR::Tick();

	HRESULT hResult = m_pCommandAllocator->Reset();
	hResult = m_pCommandList->Reset(m_pCommandAllocator, NULL);
	ChatMGR::UpdateText();
	ProcessInput();

	AnimateObjects();
	GameScene::MainScene->update();

	m_pPlayer->Update(Timer::GetTimeElapsed());
	for (auto& p : m_OtherPlayers) {
		if (p->GetUsed()) {
			dynamic_cast<Player*>(p)->Update(Timer::GetTimeElapsed());
		}
	}

	/////////////// Shadow Map Render ////////////////////////
	GameScene::MainScene->OnPrepareRender(m_pCommandList, m_pCamera);
	m_pCommandList->RSSetViewports(1, &m_ShadowMap->Viewport());
	m_pCommandList->RSSetScissorRects(1, &m_ShadowMap->ScissorRect());

	ResourceTransition(m_pCommandList, m_ShadowMap->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	m_pCommandList->ClearDepthStencilView(m_ShadowMap->Dsv(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	m_pCommandList->OMSetRenderTargets(0, nullptr, false, &m_ShadowMap->Dsv());

	m_pCommandList->SetPipelineState(m_pPipelineState);

	XMFLOAT3 pos;
	XMFLOAT3 dir = XMFLOAT3(-0.707f, -0.707f, 0.0f);
	float radius = 20;

	XMFLOAT3 targetpos = m_pPlayer->GetPosition();
	//XMFLOAT3 targetpos = XMFLOAT3(-16,0,103);
	
	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = XMLoadFloat3(&dir);
	XMVECTOR targetPos = XMLoadFloat3(&targetpos);
	XMVECTOR lightPos = targetPos - 2.0f * radius * lightDir;
	XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);

	XMStoreFloat3(&pos, lightPos);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - radius;
	float b = sphereCenterLS.y - radius;
	float n = sphereCenterLS.z - radius;
	float r = sphereCenterLS.x + radius;
	float t = sphereCenterLS.y + radius;
	float f = sphereCenterLS.z + radius;

	XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
	XMMATRIX T(	0.5f, 0.0f,	 0.0f, 0.0f,
				0.0f, -0.5f, 0.0f, 0.0f,
				0.0f, 0.0f,  1.0f, 0.0f,
				0.5f, 0.5f,  0.0f, 1.0f);
	XMMATRIX S = lightView * lightProj * T;

	XMFLOAT4X4 proj;
	XMStoreFloat4x4(&proj, lightProj);

	XMFLOAT4X4 view;
	XMStoreFloat4x4(&view, lightView);

	XMStoreFloat4x4(&m_pShadowMappedCamera->m_xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&view)));
	XMStoreFloat4x4(&m_pShadowMappedCamera->m_xmf4x4InverseView, XMMatrixTranspose(XMMatrixInverse(NULL, XMLoadFloat4x4(&view))));
	XMStoreFloat4x4(&m_pShadowMappedCamera->m_xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&proj)));
	XMStoreFloat4x4(&m_pShadowMappedCamera->m_xmf4x4InverseProjection, XMMatrixTranspose(XMMatrixInverse(NULL, XMLoadFloat4x4(&proj))));
	XMStoreFloat4x4(&m_pShadowMappedCamera->m_xm4x4ShadowTransform, XMMatrixTranspose(S));
	XMStoreFloat4x4(&m_pCamera->m_pcbMappedCamera->m_xm4x4ShadowTransform, XMMatrixTranspose(S));

	::memcpy(&m_pShadowMappedCamera->m_xmf3Position, &pos, sizeof(XMFLOAT3));
	::memcpy(&m_pShadowMappedCamera->m_xmf3Direction, &dir, sizeof(XMFLOAT3));

	D3D12_GPU_VIRTUAL_ADDRESS d3dGPUVirtualAddress = m_pShadowCamera->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_CAMERA, d3dGPUVirtualAddress);
	m_pCommandList->SetDescriptorHeaps(1, &GameScene::MainScene->m_pd3dCbvSrvDescriptorHeap);

	for (auto& object : GameScene::MainScene->MonsterObjects)
	{
		//object->Animate(elapseTime);
		object->UpdateTransform(NULL);
		object->Render(m_pCommandList, m_pCamera);
	}
	for (auto& object : GameScene::MainScene->gameObjects)
	{
		//object->Animate(elapseTime);
		object->UpdateTransform(NULL);
		object->Render(m_pCommandList, m_pCamera);
	}
	for (auto& blendObject : GameScene::MainScene->blendGameObjects)
	{
		blendObject->UpdateTransform(NULL);
		blendObject->Render(m_pCommandList, m_pCamera);
	}
	if (m_pPlayer) m_pPlayer->Render(m_pCommandList, m_pCamera);
	for (auto& p : m_OtherPlayers) {
		if (p->GetUsed()) {
			dynamic_cast<Player*>(p)->Render(m_pCommandList, m_pCamera);
		}
	}

	ResourceTransition(m_pCommandList, m_ShadowMap->Resource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);

	//GameScene::MainScene->m_pLights->m_pLights[0]
	//////////////////////////////////////////////////////////

	ResourceTransition(m_pCommandList, m_ppRenderTargetBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	GameScene::MainScene->OnPrepareRender(m_pCommandList, m_pCamera);

	m_pCommandList->ClearDepthStencilView(m_DSVDescriptorCPUHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	//////////// MRT Render Target /////////////
	m_pScreen->OnPrepareRenderTarget(m_pCommandList, 1, &m_pSwapChainBackBufferRTVCPUHandles[m_nSwapChainBufferIndex], m_DSVDescriptorCPUHandle);
	// Object , Terrain
	GameScene::MainScene->Render(m_pCommandList, m_pCamera);

	// �÷��̾�
	if (m_pPlayer) m_pPlayer->Render(m_pCommandList, m_pCamera);
	for (auto& p : m_OtherPlayers) {
		if (p->GetUsed()) {
			dynamic_cast<Player*>(p)->Render(m_pCommandList, m_pCamera);
		}
	}
	///////////////////////////////////////////


	////////// Back Buffer ///////////
	m_pCommandList->OMSetRenderTargets(1, &m_pSwapChainBackBufferRTVCPUHandles[m_nSwapChainBufferIndex], TRUE, &m_DSVDescriptorCPUHandle);

	// MRT ���
	m_pCommandList->SetGraphicsRootDescriptorTable(23, m_ShadowMap->Srv());
	m_pScreen->Render(m_pCommandList, m_pCamera);

	m_pScreen->OnPostRenderTarget(m_pCommandList);

	// Blend Object
	GameScene::MainScene->RenderBlend(m_pCommandList, m_pCamera);
	// Sky Box
	if(GameScene::MainScene->m_pSkyBox)GameScene::MainScene->m_pSkyBox->Render(m_pCommandList, m_pCamera);
	// Bounding Box
	if (DebugMode) GameScene::MainScene->RenderBoundingBox(m_pCommandList, m_pCamera);
	// Debug ȭ��
	
	if (DebugMode)
	{
		m_pScreen->SetDescriptorHeap(m_pCommandList);
		m_pCommandList->SetGraphicsRootDescriptorTable(23, m_ShadowMap->Srv());

		m_pDebug->Render(m_pCommandList, m_pCamera);
	}


	// UI
	GameScene::MainScene->RenderUI(m_pCommandList, m_pCamera);
	if (!IsTouchDebuff)
	{
		if (!ScriptMode && !OptionMode)RenderHP();
	}
	else
	{
		TouchDebuffLeftTime -= Timer::GetTimeElapsed();
		if (TouchDebuffLeftTime < 0)
		{
			IsTouchDebuff = false;
		}
	}

	ResourceTransition(m_pCommandList, m_ppRenderTargetBuffers[m_nSwapChainBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	m_pCommandList->SetDescriptorHeaps(1, &GameScene::m_pd3dCbvSrvDescriptorHeap);

	hResult = m_pCommandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	WaitForGpuComplete();

	if (scene_type == LOGIN_SCENE) {
		ChatMGR::m_pUILayer->RenderSingle(m_nSwapChainBufferIndex);
	}
	else if (scene_type >= LOBBY_SCENE) {
		ChatMGR::m_pUILayer->Render(m_nSwapChainBufferIndex);
	}


	m_pSwapChain->Present(0, 0);

	MoveToNextFrame();

	Timer::GetFrameRate(m_FrameRate + 9, 10);
	::SetWindowText(m_hWnd, m_FrameRate);
}

void GameFramework::RenderHP()
{
	if (scene_type >= SIGHT_SCENE) {
		//for (auto& p : m_OtherPlayers)
		//{
		//	p->m_pHP_Dec_UI->UpdateTransform(NULL);
		//	p->m_pHP_Dec_UI->Render(m_pCommandList, m_pCamera);
		//	p->m_pHP_UI->UpdateTransform(NULL);
		//	p->m_pHP_UI->Render(m_pCommandList, m_pCamera);
		//	p->m_pUI->UpdateTransform(NULL);
		//	p->m_pUI->Render(m_pCommandList, m_pCamera);
		//}
		m_pPlayer->m_pHP_Dec_UI->UpdateTransform(NULL);
		m_pPlayer->m_pHP_Dec_UI->Render(m_pCommandList, m_pCamera);
		m_pPlayer->m_pHP_UI->UpdateTransform(NULL);
		m_pPlayer->m_pHP_UI->Render(m_pCommandList, m_pCamera);
		m_pPlayer->m_pUI->UpdateTransform(NULL);
		m_pPlayer->m_pUI->Render(m_pCommandList, m_pCamera);
	}
}

void GameFramework::SaveSceneOBB()
{
	ofstream out{ "NonSenseMapOBB.txt" };
	
	cout << "�� �ٿ�� �ڽ� ���� ��.." << endl;
	for (auto p : GameScene::MainScene->BoundingGameObjects) {
		if (!dynamic_cast<BoundBox*>(p))
			continue;
		BoundBox* obb = dynamic_cast<BoundBox*>(p);
		if (obb->GetNum() != 0)
			continue;

		out << obb->Center.x << " " << obb->Center.y << " " << obb->Center.z << " ";
		out << obb->Extents.x << " " << obb->Extents.y << " " << obb->Extents.z << " ";
		out << obb->Orientation.x << " " << obb->Orientation.y << " " << obb->Orientation.z << " " << obb->Orientation.w << endl;
	}
	cout << "�� �ٿ�� �ڽ� ����Ϸ�!" << endl;
}

