#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "GameObject.h"
class Shader;
class Camera;

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
	//재질의 기본 색상
	XMFLOAT4 m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//재질의 번호
	UINT m_nReflection = 0;
	//재질을 적용하여 렌더링을 하기 위한 쉐이더
	Shader* m_pShader = NULL;
	void SetAlbedo(XMFLOAT4& xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetShader(Shader* pShader);
};

class Object : public GameObject
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
	void SetMaterial(Material* pMaterial);
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


protected:
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
