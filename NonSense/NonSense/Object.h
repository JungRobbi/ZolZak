#pragma once
#include "Mesh.h"
#include "Camera.h"
class Shader;
class Camera;
class Object;

#define RESOURCE_TEXTURE1D			0x01
#define RESOURCE_TEXTURE2D			0x02
#define RESOURCE_TEXTURE2D_ARRAY	0x03
#define RESOURCE_TEXTURE2DARRAY		0x04
#define RESOURCE_TEXTURE_CUBE		0x05
#define RESOURCE_BUFFER				0x06
#define RESOURCE_STRUCTURED_BUFFER	0x07


class Texture
{
public:
	Texture(int nTextureResource, UINT nTextureType, int nRootParameters);
	~Texture();

private:
	int								m_nReferences = 0;

	UINT							m_nTextureType = NULL;

	_TCHAR							(*m_ppstrTextureNames)[64] = NULL;

	int								m_nTextures = 0;
	ID3D12Resource**				m_ppd3dTextures = NULL;
	ID3D12Resource**				m_ppd3dTextureUploadBuffers;

	int								m_nRootParameters = 0;
	int*							m_pRootParameterIndices = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE*	m_pd3dSRVGPUDescriptorHandle = NULL;
	
	DXGI_FORMAT* m_pdxgiBufferFormats = NULL;
	int* m_pnBufferElements = NULL;


	UINT* m_pnResourceTypes = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }


	int GetRootParameters() { return(m_nRootParameters); }
	int GetTextures() { return(m_nTextures); }
	ID3D12Resource* GetResource(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	_TCHAR* GetTextureName(int nIndex) { return(m_ppstrTextureNames[nIndex]); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(int nIndex) { return(m_pd3dSRVGPUDescriptorHandle[nIndex]); }
	int GetRootParameter(int nIndex) { return(m_pRootParameterIndices[nIndex]); }
	UINT GetTextureType() { return(m_nTextureType); }
	UINT GetTextureType(int nIndex) { return(m_pnResourceTypes[nIndex]); }
	DXGI_FORMAT GetBufferFormat(int nIndex) { return(m_pdxgiBufferFormats[nIndex]); }
	int GetBufferElements(int nIndex) { return(m_pnBufferElements[nIndex]); }

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);


	void SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);

	void LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex);
	bool LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, Object* pParent, FILE* pInFile, Shader* pShader, UINT nIndex);
};

#define MATERIAL_ALBEDO_MAP				0x01
#define MATERIAL_SPECULAR_MAP			0x02
#define MATERIAL_NORMAL_MAP				0x04
#define MATERIAL_METALLIC_MAP			0x08
#define MATERIAL_EMISSION_MAP			0x10
#define MATERIAL_DETAIL_ALBEDO_MAP		0x20
#define MATERIAL_DETAIL_NORMAL_MAP		0x40

struct MATERIAL
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4 m_xmf4Emissive;
};

class Material
{
public:
	Material() {};
	virtual ~Material() {};
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	UINT							m_nType = 0x00;

	Texture* m_pTexture = NULL;

	//재질의 기본 색상
	XMFLOAT4 m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 m_xmf4Emissive = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 m_xmf4Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 m_xmf4Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	float							m_fGlossiness = 0.0f;
	float							m_fSmoothness = 0.0f;
	float							m_fSpecularHighlight = 0.0f;
	float							m_fMetallic = 0.0f;
	float							m_fGlossyReflection = 0.0f;

	int 							m_nTextures = 0;
	_TCHAR(*m_ppstrTextureNames)[64] = NULL;
	Texture** m_ppTextures = NULL;

	//재질의 번호
	UINT m_nReflection = 0;
	//재질을 적용하여 렌더링을 하기 위한 쉐이더
	Shader* m_pShader = NULL;
	void SetAlbedo(XMFLOAT4& xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetShader(Shader* pShader);
	void SetMaterialType(UINT nType) { m_nType |= nType; }
	void SetTexture(Texture* pTexture);

};
// Animation
//struct CALLBACKKEY
//{
//	float  							m_fTime = 0.0f;
//	void* m_pCallbackData = NULL;
//};

class AnimationSet
{
public:
	AnimationSet(float Length, int FramesPerSecond, int KeyFrameTransforms, int SkinningBones, char* Name);
	~AnimationSet();

public:
	char			m_AnimationSetName[64];
	float			m_Length = 0.0f;
	int				m_FramePerSecond =0;

	int				m_KeyFrames = 0;
	float*			m_pKeyFrameTimes = NULL;
	XMFLOAT4X4**	m_ppKeyFrameTransforms = NULL;


	float			m_Postion = 0.0f;

	int 			m_nType = ANIMATION_TYPE_LOOP; //Once, Loop, PingPong
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
	AnimationSet**					m_pAnimationSets = NULL;

	int								m_nAnimatedBoneFrames = 0;
	Object**						m_ppAnimatedBoneFrameCaches = NULL; //[m_nAnimatedBoneFrames]
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

class Object
{
public:
	Object();
	virtual ~Object();
private:
	int m_nReferences = 0;
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

	void SetChild(Object* pChild, bool bReferenceUpdate = false);
	void SetMaterial(Material* pMaterial);
	void SetMaterials(int nMaterial,Material* pMaterial);
	void SetMaterial(UINT nReflection);
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	//모델 좌표계의 픽킹 광선을 생성한다.
	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection);
	//카메라 좌표계의 한 점에 대한 모델 좌표계의 픽킹 광선을 생성하고 객체와의 교차를 검사한다.
	int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);

	int FindReplicatedTexture(_TCHAR* pstrTextureName, D3D12_GPU_DESCRIPTOR_HANDLE* pd3dSrvGpuDescriptorHandle);


	Object* m_pParent = NULL;
	Object* m_pChild = NULL;
	Object* m_pSibling = NULL;

	char							m_pFrameName[64];

	int m_nMaterials = 0;
	Material** m_ppMaterials = NULL;

protected:
	XMFLOAT4X4 m_xmf4x4ToParent;
	XMFLOAT4X4 m_xmf4x4World;
	Mesh* m_pMesh = NULL;
	Material* m_pMaterial = NULL;
public:
	void ReleaseUploadBuffers();
	bool IsVisible(Camera* pCamera = NULL);
	virtual void SetMesh(Mesh* pMesh);
	virtual void SetShader(Shader* pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView);
	//상수 버퍼를 생성한다.
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	//상수 버퍼의 내용을 갱신한다.
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();


	Object* FindFrame(char* pstrFrameName);
	void FindAndSetSkinnedMesh(SkinnedMesh** ppSkinnedMeshes, int* pnSkinnedMesh);

public: // 모델 & 애니메이션 로드
	static Object* LoadHierarchy(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, Object* pParent, FILE* OpendFile, Shader* pShader, int* pnSkinnedMeshes);
	static LoadedModelInfo* LoadAnimationModel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, Shader* pShader);
	static void LoadAnimationFromFile(FILE* OpenedFile, LoadedModelInfo* pLoadModel);
	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, Object* pParent, FILE* OpenedFile, Shader* pShader);
};

class RotatingObject : public Object
{
public:
	RotatingObject();
	virtual ~RotatingObject();
private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
	virtual void Animate(float fTimeElapsed);
};
