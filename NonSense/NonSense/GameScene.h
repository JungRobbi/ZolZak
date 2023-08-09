
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
#include "UILayer.h"
#include "Sound.h"

struct CB_SCREEN_INFO
{
	XMFLOAT4 LineColor = XMFLOAT4(0,0,0,1);
	UINT LineSize=1;
	UINT ToonShading=1;
	float darkness=0;
};

struct LIGHT
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;
	XMFLOAT3 m_xmf3Position;
	float m_fFalloff;
	XMFLOAT3 m_xmf3Direction;
	float m_fTheta; //cos(m_fTheta)
	XMFLOAT3 m_xmf3Attenuation;
	float m_fPhi; //cos(m_fPhi)
	bool m_bEnable;
	int m_nType;
	float m_fRange;
	float padding;
};
struct LIGHTS
{
	LIGHT m_pLights[MAX_LIGHTS];
	XMFLOAT4 m_xmf4GlobalAmbient;
};
struct MATERIALS
{
	MATERIAL m_pReflections[MAX_MATERIALS];
};

class GameScene
{
public:
	std::map<std::string, LoadedModelInfo*> ModelMap;
	std::map<std::string, CTexture*> TextureMap;

	std::queue<Object*> creationQueue;
	std::deque<Object*> deletionQueue;
	std::list<Object*> gameObjects;

	std::queue<Monster*> creationMonsterQueue;
	std::deque<Monster*> deletionMonsterQueue;
	std::list<Monster*> MonsterObjects;

	std::queue<Object*> creationBlendQueue;
	std::deque<Object*> deletionBlendQueue;
	std::list<Object*> blendGameObjects;

	std::queue<Object*> creationUIQueue;
	std::deque<Object*> deletionUIQueue;
	std::list<Object*> UIGameObjects;

	std::queue<Object*> creationBoundingQueue;
	std::deque<Object*> deletionBoundingQueue;
	std::list<Object*> BoundingGameObjects;

	std::queue<Object*> creationForwardQueue;
	std::deque<Object*> deletionForwardQueue;
	std::list<Object*> ForwardObjects;

	std::list<Sound*> Sounds;

	XMFLOAT4 LineColor = XMFLOAT4(0, 0, 0, 1);
	UINT LineSize = 1;
	UINT ToonShading = 10;

	int SelectNum = 0;
	bool HaveEye = false;
	bool HaveEar = false;
	bool HaveHand = false;
public:
	static GameScene* MainScene;
	Player* m_pPlayer = NULL;
	NPC* StartNPC = NULL;
	NPC* EndNPC = NULL;
	NPCScript* ScriptUI = NULL;
	HeightMapTerrain* m_pTerrain = NULL;
	bool IsSoundDebuff = false;
	float SoundDebuffLeftTime = -1.0f;
	Sound* MainBGM = NULL;
protected:
	Object* CreateEmpty();
	float elapseTime;
public:
	virtual void update();
	virtual void render();
	bool change = false;
	void PushDelete(Object* gameObject);

	friend Object;

public:
	GameScene();
	virtual ~GameScene();
	std::list<Object*> GetObjects() { return gameObjects; }
	//���� ��� ����� ������ ����
	virtual void BuildLightsAndMaterials();
	//���� ��� ����� ������ ���� ���ҽ��� �����ϰ� ����
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	//������ ���콺�� Ű���� �޽����� ó���Ѵ�.
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseObjects();
	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	void RenderBlend(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual void RenderUI(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	void RenderForward(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	void RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	void ReleaseUploadBuffers();
	HeightMapTerrain* GetTerrain() { return(m_pTerrain); }
	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();
	//���� ��� ���� ��ü�鿡 ���� ���콺 ��ŷ�� �����Ѵ�.
	Object* PickObjectPointedByCursor(int xClient, int yClient, Camera* pCamera);
	SkyBox* m_pSkyBox = NULL;
	SkyBox* m_pDaySkyBox = NULL;
	SkyBox* m_pNightSkyBox = NULL;
	SkyBox* m_pSunsetSkyBox = NULL;

	void CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nRootParameter, bool bAutoIncrement);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride);
	static ID3D12DescriptorHeap* m_pd3dCbvSrvDescriptorHeap;
	CB_SCREEN_INFO* m_pMappedScreenOptions = NULL;
	ID3D12Resource* m_pScreenOptions = NULL;

	void Sound_Debuff(float time);
	void AddSound(Sound* s);
	ID3D12RootSignature* m_pGraphicsRootSignature = NULL;
	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorStartHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorStartHandle;

	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorNextHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorNextHandle;
protected:

	int m_nShaders = 0;

	//���� ����
	LIGHTS* m_pLights = NULL;
	//������ ��Ÿ���� ���ҽ��� ���ҽ��� ���� �������̴�.
	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;
	//���� ��ü�鿡 ����Ǵ� ����
	MATERIALS* m_pMaterials = NULL;
	//������ ��Ÿ���� ���ҽ��� ���ҽ��� ���� �������̴�.
	ID3D12Resource* m_pd3dcbMaterials = NULL;
	MATERIAL* m_pcbMappedMaterials = NULL;
	// Screen option

	int m_nObjects = 0;
	Object** m_GameObjects = NULL;
	int m_nBlendObjects = 0;
	Object** m_ppBlendObjects = NULL;
	Shader* m_pBlendShader = NULL;

	Shader* m_pBoundingShader = NULL;
	CubeMesh* m_pBoundMesh = NULL;

	Object* TempObject = NULL;
	



};