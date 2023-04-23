
#pragma once
#include <list>
#include <queue>
#include <deque>

#include "Shader.h"
#include "Player.h"
#include "Camera.h"
#include "Object.h"
#include "Characters.h"
#include "UI.h"
#include "Scene.h"

class LoginScene : public Scene
{
public:
	std::queue<Object*> creationQueue;
	std::deque<Object*> deletionQueue;
	std::list<Object*> gameObjects;

	std::queue<Character*> creationMonsterQueue;
	std::deque<Character*> deletionMonsterQueue;
	std::list<Character*> MonsterObjects;

	std::queue<Object*> creationBlendQueue;
	std::deque<Object*> deletionBlendQueue;
	std::list<Object*> blendGameObjects;

	std::queue<Object*> creationUIQueue;
	std::deque<Object*> deletionUIQueue;
	std::list<Object*> UIGameObjects;

	std::queue<Object*> creationBoundingQueue;
	std::deque<Object*> deletionBoundingQueue;
	std::list<Object*> BoundingGameObjects;

public:
	static LoginScene* MainScene;

protected:
	Object* CreateEmpty();

public:
	virtual void update();
	virtual void render();

	void PushDelete(Object* gameObject);

	friend Object;

public:
	LoginScene();
	virtual ~LoginScene();
	std::list<Object*> GetObjects() { return gameObjects; }
	//씬의 모든 조명과 재질을 생성
	void BuildLightsAndMaterials();
	//씬의 모든 조명과 재질을 위한 리소스를 생성하고 갱신
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	//씬에서 마우스와 키보드 메시지를 처리한다.
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();
	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	void RenderBlend(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	void RenderUI(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	void RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	void ReleaseUploadBuffers();
	HeightMapTerrain* GetTerrain() { return(m_pTerrain); }
	//그래픽 루트 시그너쳐를 생성한다.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();
	//씬의 모든 게임 객체들에 대한 마우스 픽킹을 수행한다.
	Object* PickObjectPointedByCursor(int xClient, int yClient, Camera* pCamera);
	Player* m_pPlayer = NULL;
	SkyBox* m_pSkyBox = NULL;
	Player_State_UI* m_pUI = NULL;
	Player_HP_UI* m_pHP_UI = NULL;
	Player_HP_DEC_UI* m_pHP_Dec_UI = NULL;

	void CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nRootParameter, bool bAutoIncrement);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride);
	static ID3D12DescriptorHeap* m_pd3dCbvSrvDescriptorHeap;

protected:

	int m_nShaders = 0;
	ID3D12RootSignature* m_pGraphicsRootSignature = NULL;

	//씬의 조명
	LIGHTS* m_pLights = NULL;
	//조명을 나타내는 리소스와 리소스에 대한 포인터이다.
	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;
	//씬의 객체들에 적용되는 재질
	MATERIALS* m_pMaterials = NULL;
	//재질을 나타내는 리소스와 리소스에 대한 포인터이다.
	ID3D12Resource* m_pd3dcbMaterials = NULL;
	MATERIAL* m_pcbMappedMaterials = NULL;

	int m_nObjects = 0;
	Object** m_GameObjects = NULL;
	int m_nBlendObjects = 0;
	Object** m_ppBlendObjects = NULL;
	Shader* m_pBlendShader = NULL;

	Shader* m_pBoundingShader = NULL;
	CubeMesh* m_pBoundMesh = NULL;

	Object* TempObject = NULL;
	HeightMapTerrain* m_pTerrain = NULL;

	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorStartHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorStartHandle;

	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorNextHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorNextHandle;

};