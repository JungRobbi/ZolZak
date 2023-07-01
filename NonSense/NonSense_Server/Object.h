#pragma once

#include "stdafx.h"
#include <list>
#include <concurrent_vector.h>
#include <atomic>
#include "Mesh.h"
#include "Components/Component.h"

class Object; 
class Mesh;
class SkinnedMesh;
class HeightMapTerrain;
class BoundSphere;
class Monster_HP_UI;

struct CB_GAMEOBJECT_INFO;

enum OBJECT_TYPE
{
	DEFAULT_OBJECT = 1,
	BLEND_OBJECT = 2,
	UI_OBJECT = 3,
	BOUNDING_OBJECT = 4,
};

class LoadedModelInfo
{
public:
	LoadedModelInfo() {}
	~LoadedModelInfo() {}

	int m_nSkinnedMeshes = 0;
	SkinnedMesh** m_ppSkinnedMeshes = NULL;

	Object* m_pRoot = NULL;
public:
	void PrepareSkinning();
};

class Object
{
	std::atomic<char> isuse = 1; // Object를 Scene에 사용하는지를 구분하기 위한 변수
protected:
	concurrency::concurrent_vector<Component*> components;

	// std::list<Component*> components;
public:
	Object();
	// GameScene의 gameobjects 리스트에 안넣기 위해 만든 생성자
	Object(bool Push_List);
	Object(OBJECT_TYPE type);
	virtual ~Object();

	virtual void start();
	virtual void update();

	template<typename T>
	T* AddComponent();

	template<typename T>
	T* GetComponent();
	
	bool GetUsed() { return isuse; }
	void SetUsed(bool b) { isuse = b; }

private:
	std::atomic<int> m_nReferences = 0;
	std::atomic<int> Num = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	void SetLook(XMFLOAT3 look) { m_xmf4x4World._31 = look.x, m_xmf4x4World._32 = look.y, m_xmf4x4World._33 = look.z; }
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	XMFLOAT4X4 GetWorld() { return m_xmf4x4World; }
	
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetLookAt(XMFLOAT3& xmf3Target, XMFLOAT3& xmf3Up);
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void SetScale(float x, float y, float z);
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent);

	void SetMesh(Mesh* pMesh);

	int GetNum() { return Num; }
	void SetNum(int num) { Num = num; }

	XMFLOAT4X4 m_xmf4x4World;

public:
	Mesh* m_pMesh = NULL;
	UINT GetMeshType(); 

	Mesh* FindFirstMesh();

	// 맵 로드
	static void LoadMapData(char* pstrFileName);
	static void LoadMapData_Blend(char* pstrFileName);

	// 모델 & 애니메이션 로드
	static Object* LoadHierarchy(FILE* OpenedFile);
	static LoadedModelInfo* LoadAnimationModel(char* pstrFileName);

	char							m_pFrameName[64];
	
	XMFLOAT4X4 m_xmf4x4ToParent;

	int m_roomNum = 0;
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
	for (auto& component : components)
	{
		auto c = dynamic_cast<T*>(component);
		if (c) return c;
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class BoundBox : public Object
{
public:
	XMFLOAT3 Center = { 0,0,0 };        
	XMFLOAT3 Extents = { 1,1,1 };       
	XMFLOAT4 Orientation = { 0,0,0,1 }; 
	BoundBox();
	virtual ~BoundBox();
	virtual void Transform(_Out_ BoundBox& Out, _In_ FXMMATRIX M);
	virtual bool Intersects(BoundBox& box);
	virtual bool Intersects(BoundSphere& box);
	virtual void GetCorners(XMFLOAT3* Corners);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class BoundSphere : public Object
{
public:
	XMFLOAT3 Center = { 0,0,0 };
	float Radius = 1.f;
	BoundSphere();
	BoundSphere(bool Push_List);
	virtual ~BoundSphere() {};
	virtual void Transform(_Out_ BoundSphere& Out, _In_ FXMMATRIX M);
	virtual bool Intersects(BoundBox& box);
	virtual bool Intersects(BoundSphere& box);
	virtual bool Intersects(FXMVECTOR V0, FXMVECTOR V1, FXMVECTOR V2);

};