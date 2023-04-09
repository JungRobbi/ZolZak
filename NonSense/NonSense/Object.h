#pragma once

#include "stdafx.h"
#include <list>
#include "Mesh.h"

#include "Camera.h"
#include "Component.h"

class Shader;
class Camera;
class Object; 
class Mesh;
class SkinnedMesh;
class HeightMapTerrain;
#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05


#define MATERIAL_ALBEDO_MAP				0x01
#define MATERIAL_SPECULAR_MAP			0x02
#define MATERIAL_NORMAL_MAP				0x04
#define MATERIAL_METALLIC_MAP			0x08
#define MATERIAL_EMISSION_MAP			0x10
#define MATERIAL_DETAIL_ALBEDO_MAP		0x20
#define MATERIAL_DETAIL_NORMAL_MAP		0x40

struct CB_GAMEOBJECT_INFO;
struct CB_PLAYER_INFO;

struct MATERIAL
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4 m_xmf4Emissive;
};

struct SRVROOTARGUMENTINFO
{
	int								m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGpuDescriptorHandle;
};
class CTexture
{
public:
	CTexture(int nTextureResources, UINT nResourceType, int nSamplers, int nRootParameters);
	virtual ~CTexture();

private:
	int								m_nReferences = 0;

	UINT							m_nTextureType = RESOURCE_TEXTURE2D;

	int								m_nTextures = 0;
	ID3D12Resource** m_ppd3dTextures = NULL;
	ID3D12Resource** m_ppd3dTextureUploadBuffers = NULL;

	UINT* m_pnResourceTypes = NULL;

	DXGI_FORMAT* m_pdxgiBufferFormats = NULL;
	int* m_pnBufferElements = NULL;

	int								m_nRootParameters = 0;
	int* m_pnRootParameterIndices = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSrvGpuDescriptorHandles = NULL;

	int								m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSamplerGpuDescriptorHandles = NULL;
public:
	SRVROOTARGUMENTINFO* m_pRootArgumentInfos = NULL;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle);

	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList,int index);

	void ReleaseShaderVariables();

	void LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex);
	void LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex);
	ID3D12Resource* CreateTexture(ID3D12Device* pd3dDevice, UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue, UINT nResourceType, UINT nIndex);

	void SetRootParameterIndex(int nIndex, UINT nRootParameterIndex);
	int GetRootParameterIndex(int nIndex) { return(m_pnRootParameterIndices[nIndex]); }
	void SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);

	int GetRootParameters() { return(m_nRootParameters); }
	int GetTextures() { return(m_nTextures); }
	ID3D12Resource* GetResource(int nIndex) { return(m_ppd3dTextures[nIndex]); }

	UINT GetTextureType() { return(m_nTextureType); }
	UINT GetTextureType(int nIndex) { return(m_pnResourceTypes[nIndex]); }
	DXGI_FORMAT GetBufferFormat(int nIndex) { return(m_pdxgiBufferFormats[nIndex]); }
	int GetBufferElements(int nIndex) { return(m_pnBufferElements[nIndex]); }

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);

	ID3D12Resource* GetTexture(int nIndex) { return(m_ppd3dTextures[nIndex]); }

	void ReleaseUploadBuffers();
};

class Material
{
public:
	Material() {};
	Material(int Textures);
	virtual ~Material() {};
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	//재질의 기본 색상
	XMFLOAT4						m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4						m_xmf4Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	UINT							m_nType = 0x00;

	float							m_fGlossiness = 0.0f;
	float							m_fSmoothness = 0.0f;
	float							m_fSpecularHighlight = 0.0f;
	float							m_fMetallic = 0.0f;
	float							m_fGlossyReflection = 0.0f;
	int 							m_nTextures = 0;
	_TCHAR(*m_ppstrTextureNames)[64] = NULL;
	CTexture** m_ppTextures = NULL;


	void SetMaterialType(UINT nType) { m_nType |= nType; }

	static Shader* m_pStandardShader;
	static Shader* m_pSkinnedAnimationShader;

	//재질의 번호
	UINT m_nReflection = 0;
	CTexture* m_pTexture = NULL;
	Shader* m_pShader = NULL;
	void SetAlbedo(XMFLOAT4& xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetShader(Shader* pShader);
	void SetTexture(CTexture* pTexture);
	void SetTexture(CTexture* pTexture, UINT nTexture);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	void LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nType, UINT nRootParameter, _TCHAR* pwstrTextureName, CTexture** ppTexture, Object* pParent, FILE* OpenedFile, Shader* pShader);
	void SetStandardShader() { Material::SetShader(m_pStandardShader); }
	void SetSkinnedAnimationShader() { Material::SetShader(m_pSkinnedAnimationShader); }

	static void Material::PrepareShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);


};

class SkyBoxMaterial : public Material
{
public:
	SkyBoxMaterial() {};
	~SkyBoxMaterial() {};
};

class AnimationSet
{
public:
	AnimationSet(float Length, int FramesPerSecond, int KeyFrameTransforms, int SkinningBones, char* Name);
	~AnimationSet();

public:
	char			m_AnimationSetName[64];
	float			m_Length = 0.0f;
	int				m_FramePerSecond = 0;

	int				m_KeyFrames = 0;
	float* m_pKeyFrameTimes = NULL;
	XMFLOAT4X4** m_ppKeyFrameTransforms = NULL;


	float			m_Position = 0.0f;

	int 			m_nType = ANIMATION_TYPE_LOOP; //Once, Loop, PingPong
public:
	bool SetPosition(float fTrackPosition);
	XMFLOAT4X4 GetSRT(int nBone);
};

class AnimationSets
{
private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	AnimationSets(int nAnimationSets);
	~AnimationSets();

public:
	int								m_nAnimationSets = 0;
	AnimationSet** m_pAnimationSets = NULL;

	int								m_nAnimatedBoneFrames = 0;
	Object** m_ppAnimatedBoneFrameCaches = NULL; //[m_nAnimatedBoneFrames]
};


class AnimationTrack
{
public:
	AnimationTrack() { }
	~AnimationTrack() { }

public:
	BOOL 							m_bEnable = true;
	float 							m_fSpeed = 1.0f;
	float 							m_fPosition = 0.0f;
	float 							m_fWeight = 1.0f;

	int 							m_nAnimationSet = 0;

public:
	void SetAnimationSet(int nAnimationSet) { m_nAnimationSet = nAnimationSet; }

	void SetEnable(bool bEnable) { m_bEnable = bEnable; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	void SetWeight(float fWeight) { m_fWeight = fWeight; }
	void SetPosition(float fPosition) { m_fPosition = fPosition; }
};
class LoadedModelInfo
{
public:
	LoadedModelInfo() {}
	~LoadedModelInfo() {}

	int m_nSkinnedMeshes = 0;
	SkinnedMesh** m_ppSkinnedMeshes = NULL;

	AnimationSets* m_pAnimationSets = NULL;

	Object* m_pRoot = NULL;
public:
	void PrepareSkinning();
};
class AnimationController
{
public:
	AnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, LoadedModelInfo* pModel);
	~AnimationController();

public:
	float 							m_fTime = 0.0f;
	float							m_BlendingWeight = 1.0f;
	int 							m_nAnimationTracks = 0;
	AnimationTrack* m_pAnimationTracks = NULL;

	AnimationSets* m_pAnimationSets = NULL;

	int 							m_nSkinnedMeshes = 0;
	SkinnedMesh** m_ppSkinnedMeshes = NULL; //[SkinnedMeshes], Skinned Mesh Cache

	ID3D12Resource** m_ppd3dcbSkinningBoneTransforms = NULL; //[SkinnedMeshes]
	XMFLOAT4X4** m_ppcbxmf4x4MappedSkinningBoneTransforms = NULL; //[SkinnedMeshes]

	bool							m_bRootMotion = false;
	Object* m_pModelRootObject = NULL;

	Object* m_pRootMotionObject = NULL;
	XMFLOAT3						m_xmf3FirstRootMotionPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);


public:
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet);
	void ChangeAnimationUseBlending(int nAnimationSet);
	void SetTrackEnable(int nAnimationTrack, bool bEnable);
	void SetTrackPosition(int nAnimationTrack, float fPosition);
	void SetTrackSpeed(int nAnimationTrack, float fSpeed);
	void SetTrackWeight(int nAnimationTrack, float fWeight);

	void AdvanceTime(float fElapsedTime, Object* pRootGameObject);
};



class Object
{
	std::list<Component*> components;
public:
	Object();
	// GameScene의 gameobjects 리스트에 안넣기 위해 만든 생성자
	Object(bool Push_List);
	Object(bool Push_List, bool isBlendObject);
	virtual ~Object();

	virtual void start();
	virtual void update();

	template<typename T>
	T* AddComponent();

	template<typename T>
	T* GetComponent();

	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;

	ID3D12Resource* m_pd3dcbGameObjects = NULL;
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObjects = NULL;
	

private:
	int m_nReferences = 0;
	int Num = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	XMFLOAT4X4 GetWorld() { return m_xmf4x4World; }
	Material* GetMaterial() { return m_pMaterial; }
	Material* GetMaterials(int index) { return m_ppMaterials[index]; }
	void SetMaterial(Material* pMaterial);
	void SetMaterial(UINT nReflection);
	void SetMaterials(int nMaterial, Material* pMaterial);
	void ChangeShader(Shader* pShader);
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void SetScale(float x, float y, float z);
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent);

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection);
	//카메라 좌표계의 한 점에 대한 모델 좌표계의 픽킹 광선을 생성하고 객체와의 교차를 검사한다.
	int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);

	XMFLOAT4X4 m_xmf4x4World;

protected:
	Mesh* m_pMesh = NULL;
	Material* m_pMaterial = NULL;
public:
	UINT GetMeshType(); 

	void ReleaseUploadBuffers();
	virtual bool IsVisible(Camera* pCamera = NULL);
	virtual void SetMesh(Mesh* pMesh);
	virtual Mesh* GetMesh() { return m_pMesh; }
	virtual void SetShader(Shader* pShader);
	virtual void SetNum(int num); 
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView);
	//상수 버퍼를 생성한다.
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	//상수 버퍼의 내용을 갱신한다.
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	CTexture* FindReplicatedTexture(_TCHAR* pstrTextureName);

	Mesh* FindFirstMesh();
	Object* FindFrame(char* pstrFrameName);
	void FindAndSetSkinnedMesh(SkinnedMesh** ppSkinnedMeshes, int* pnSkinnedMesh);

	// 맵 로드
	static void LoadMapData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName);
	static void LoadMapData_Blend(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, Shader* pBlendShader);

	// 모델 & 애니메이션 로드
	static Object* LoadHierarchy(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, Object* pParent, FILE* OpendFile, Shader* pShader, int* pnSkinnedMeshes);
	static LoadedModelInfo* LoadAnimationModel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, Shader* pShader);
	static void LoadAnimationFromFile(FILE* OpenedFile, LoadedModelInfo* pLoadModel);
	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, Object* pParent, FILE* OpenedFile, Shader* pShader);

	void SetChild(Object* pChild, bool bReferenceUpdate = false);


	Object* m_pParent = NULL;
	Object* m_pChild = NULL;
	Object* m_pSibling = NULL;

	char							m_pFrameName[64];

	int m_nMaterials = 0;
	Material** m_ppMaterials = NULL;

	XMFLOAT4X4 m_xmf4x4ToParent;

	AnimationController* m_pSkinnedAnimationController = NULL;

};


template<typename T>
inline T* Object::AddComponent()
{
	auto component = new T;
	component->gameObject = this;
	components.push_back(component);
	return component;
}

template<typename T>
inline T* Object::GetComponent()
{
	for (auto component : components)
	{
		auto c = dynamic_cast<T*>(component);
		if (c) return c;
	}
	return nullptr;
}


//임시 객체
class TestModelObject : public Object
{
public:
	TestModelObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel, LoadedModelInfo* pWeaponModel, int nAnimationTracks);
	virtual ~TestModelObject() {};

};

class TestModelBlendObject : public Object
{
public:
	TestModelBlendObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel, Shader* pShader);
	virtual ~TestModelBlendObject() {};

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SkyBox : public Object
{
public:
	SkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~SkyBox();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class UI : public Object
{
public:
	UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~UI();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
	virtual void SetParentUI(UI* Parent) { ParentUI = Parent; }
	virtual void SetMyPos(float x, float y, float w, float h);
	virtual void OnPreRender();
private:
	ID3D12Resource* m_pd3dcbUI = NULL;
	CB_PLAYER_INFO* m_pcbMappedUI = NULL;
	UI* ParentUI = NULL;
	XMFLOAT4X4 XYWH;
};

class Player_State_UI : public UI
{
public:
	Player_State_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Player_State_UI() {};
};

class Player_HP_UI : public UI
{
public:
	Player_HP_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Player_HP_UI() {};
	float HP = 1.0;
};

class Player_HP_DEC_UI : public Player_HP_UI
{
public:
	Player_HP_DEC_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Player_HP_DEC_UI() {};
	virtual void update();
	float Dec_HP = 1.0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class HeightMapTerrain : public Object
{
public:
	HeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~HeightMapTerrain();

private:
	HeightMapImage				*m_pHeightMapImage;

	int							m_nWidth;
	int							m_nLength;

	XMFLOAT3					m_xmf3Scale;

public:
	float GetHeight(float x, float z, bool bReverseQuad = false); //World
	XMFLOAT3 GetNormal(float x, float z);

	int GetHeightMapWidth();
	int GetHeightMapLength();

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
	bool IsVisible(Camera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);

};