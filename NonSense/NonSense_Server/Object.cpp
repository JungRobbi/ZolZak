#include "Object.h"
#include "Scene.h"
#include "stdafx.h"
#include "Components/BoxCollideComponent.h"
#include "Components/SphereCollideComponent.h"

void LoadedModelInfo::PrepareSkinning()
{
	int nSkinnedMesh = 0;
}

Object::Object()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	XMStoreFloat4x4(&m_xmf4x4ToParent, XMMatrixIdentity());
	Scene::scene->creationQueue.push(this);
}
Object::Object(bool Push_List)
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	XMStoreFloat4x4(&m_xmf4x4ToParent, XMMatrixIdentity());
	if (Push_List) {
		Scene::scene->creationQueue.push(this);
	}
}
Object::Object(OBJECT_TYPE type)
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	XMStoreFloat4x4(&m_xmf4x4ToParent, XMMatrixIdentity());
	switch (type) {
	case DEFAULT_OBJECT:
		Scene::scene->creationQueue.push(this);
		break;
	//case BLEND_OBJECT:
	//	Scene::scene->creationBlendQueue.push(this);
	//	break;
	//case UI_OBJECT:
	//	Scene::scene->creationUIQueue.push(this);
	//	break;
	//case BOUNDING_OBJECT:
	//	Scene::scene->creationBoundingQueue.push(this);
	//	break;

	}
}

Object::~Object()
{
	for (auto& p : components)
		delete p;
	components.clear();
}

void Object::start()
{
	for (auto& component : components)
		component->start();
}

void Object::update()
{
	for (auto& component : components)
		component->update();
}


Mesh* Object::FindFirstMesh()
{
	return m_pMesh;
}

void Object::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}
void Object::SetPosition(float x, float y, float z)
{
	m_xmf4x4ToParent._41 = x;
	m_xmf4x4ToParent._42 = y;
	m_xmf4x4ToParent._43 = z;

	UpdateTransform(NULL);
}
void Object::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}
void Object::SetLookAt(XMFLOAT3& xmf3Target, XMFLOAT3& xmf3Up)
{
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(xmf3Position, xmf3Target, xmf3Up);
	m_xmf4x4World._11 = mtxLookAt._11; m_xmf4x4World._12 = mtxLookAt._21; m_xmf4x4World._13 = mtxLookAt._31;
	m_xmf4x4World._21 = mtxLookAt._12; m_xmf4x4World._22 = mtxLookAt._22; m_xmf4x4World._23 = mtxLookAt._32;
	m_xmf4x4World._31 = mtxLookAt._13; m_xmf4x4World._32 = mtxLookAt._23; m_xmf4x4World._33 = mtxLookAt._33;
}

XMFLOAT3 Object::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 Object::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 Object::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22,
		m_xmf4x4World._23)));
}

XMFLOAT3 Object::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12,
		m_xmf4x4World._13)));
}

void Object::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	Object::SetPosition(xmf3Position);
}

void Object::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	Object::SetPosition(xmf3Position);
}

void Object::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	Object::SetPosition(xmf3Position);
	if (Scene::terrain)
	{
		float width = Scene::terrain->GetWidth();
		float length = Scene::terrain->GetLength();
		m_xmf4x4ToParent._42 = Scene::terrain->GetHeight(m_xmf4x4ToParent._41 + (width/2), m_xmf4x4ToParent._43 + (length /2));
	}
}

void Object::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);
	UpdateTransform(NULL);
}

void Object::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;
}

// -------------- 모델 & 애니메이션 로드 --------------

int ReadIntegerFromFile(FILE* OpenedFile)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, OpenedFile);
	return(nValue);
}

float ReadFloatFromFile(FILE* OpenedFile)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, OpenedFile);
	return(fValue);
}

BYTE ReadStringFromFile(FILE* OpenedFile, char* pstrToken)
{
	BYTE nStrLength = 0;
	UINT nReads = 0;
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, OpenedFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, OpenedFile);
	pstrToken[nStrLength] = '\0';

	return(nStrLength);
}

void Object::LoadMapData(char* pstrFileName)
{
	char pstrToken[64] = { '\0' };
	int nMesh = 0;
	UINT nReads = 0;
	Object* pObject = NULL;
	FILE* OpenedFile = NULL;
	::fopen_s(&OpenedFile, pstrFileName, "rb");
	::rewind(OpenedFile);
	std::map<std::string,LoadedModelInfo*> ModelMap;
	::ReadStringFromFile(OpenedFile, pstrToken);
	if (!strcmp(pstrToken, "<Objects>:"))
	{
		while (true)
		{
			::ReadStringFromFile(OpenedFile, pstrToken);
			if (!strcmp(pstrToken, "<Mesh>:"))
			{
				BYTE Length = ::ReadStringFromFile(OpenedFile, pstrToken);
			
				if (pstrToken[0] == '@') // Mesh이름과 맞는 Mesh가 이미 로드가 되었다면 true -> 있는 모델 쓰면 됨
				{
					std::string str(pstrToken + 1);
				}
				else
				{
					std::string str(pstrToken);
					char pstrFilePath[64] = { '\0' };
					strcpy_s(pstrFilePath, 64, "Model/");
					strcpy_s(pstrFilePath + 6, 64 - 6, pstrToken);
					strcpy_s(pstrFilePath + 6 + Length, 64 - 6 - Length, ".bin");

					LoadedModelInfo* pLoadedModel = LoadAnimationModel(pstrFilePath);

					pObject = pLoadedModel->m_pRoot;
				}
			}
			if (!strcmp(pstrToken, "<Position>:"))
			{
				BoundBox* bb = new BoundBox();
				BoundSphere* bs = new BoundSphere();

				nReads = (UINT)::fread(&pObject->m_xmf4x4ToParent, sizeof(float), 16, OpenedFile);
				pObject->UpdateTransform(NULL);
				pObject->AddComponent<BoxCollideComponent>();
				pObject->GetComponent<BoxCollideComponent>()->SetBoundingObject(bb);

				float rd = pObject->FindFirstMesh()->GetBoundingBox().Extents.x > pObject->FindFirstMesh()->GetBoundingBox().Extents.y ? (pObject->FindFirstMesh()->GetBoundingBox().Extents.x > pObject->FindFirstMesh()->GetBoundingBox().Extents.z) ? pObject->FindFirstMesh()->GetBoundingBox().Extents.x : pObject->FindFirstMesh()->GetBoundingBox().Extents.z : (pObject->FindFirstMesh()->GetBoundingBox().Extents.y > pObject->FindFirstMesh()->GetBoundingBox().Extents.z) ? pObject->FindFirstMesh()->GetBoundingBox().Extents.y : pObject->FindFirstMesh()->GetBoundingBox().Extents.z;

				pObject->AddComponent<SphereCollideComponent>();
				pObject->GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
				pObject->GetComponent<SphereCollideComponent>()->SetCenterRadius(pObject->FindFirstMesh()->GetBoundingBox().Center, rd);
			}
			if (!strcmp(pstrToken, "</Objects>"))
			{
				break;
			}
		}
	}
}



void Object::LoadMapData_Blend(char* pstrFileName)
{
	//char pstrToken[64] = { '\0' };
	//int nMesh = 0;
	//UINT nReads = 0;
	//Object* pObject = NULL;
	//FILE* OpenedFile = NULL;
	//::fopen_s(&OpenedFile, pstrFileName, "rb");
	//::rewind(OpenedFile);
	//std::map<std::string, LoadedModelInfo*> ModelMap;
	//::ReadStringFromFile(OpenedFile, pstrToken);
	//if (!strcmp(pstrToken, "<Objects>:"))
	//{
	//	while (true)
	//	{
	//		::ReadStringFromFile(OpenedFile, pstrToken);
	//		if (!strcmp(pstrToken, "<Mesh>:"))
	//		{
	//			BYTE Length = ::ReadStringFromFile(OpenedFile, pstrToken);

	//			if (pstrToken[0] == '@') // Mesh이름과 맞는 Mesh가 이미 로드가 되었다면 true -> 있는 모델 쓰면 됨
	//			{
	//				std::string str(pstrToken + 1);
	//				pObject = new TestModelBlendObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, ModelMap[str], pBlendShader);

	//			}
	//			else
	//			{
	//				std::string str(pstrToken);
	//				char pstrFilePath[64] = { '\0' };
	//				strcpy_s(pstrFilePath, 64, "Model/");
	//				strcpy_s(pstrFilePath + 6, 64 - 6, pstrToken);
	//				strcpy_s(pstrFilePath + 6 + Length, 64 - 6 - Length, ".bin");


	//				LoadedModelInfo* pLoadedModel = LoadAnimationModel(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pstrFilePath, NULL);

	//				ModelMap.insert(std::pair<std::string, LoadedModelInfo*>(str, pLoadedModel)); // 읽은 모델은 map에 저장


	//				pObject = new TestModelBlendObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pLoadedModel, pBlendShader);

	//			}
	//		}
	//		if (!strcmp(pstrToken, "<Position>:"))
	//		{
	//			nReads = (UINT)::fread(&pObject->m_xmf4x4ToParent, sizeof(float), 16, OpenedFile);

	//		}
	//		if (!strcmp(pstrToken, "</Objects>"))
	//		{
	//			break;
	//		}
	//	}
	//}
}


Object* Object::LoadHierarchy(FILE* OpenedFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	Object* pObject = new Object();

	for (; ; )
	{
		::ReadStringFromFile(OpenedFile, pstrToken);
		if (!strcmp(pstrToken, "<Frame>:"))
		{
			nFrame = ::ReadIntegerFromFile(OpenedFile);
			nTextures = ::ReadIntegerFromFile(OpenedFile);

			::ReadStringFromFile(OpenedFile, pObject->m_pFrameName);
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, OpenedFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, OpenedFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, OpenedFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, OpenedFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&pObject->m_xmf4x4ToParent, sizeof(float), 16, OpenedFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			LoadMesh* pMesh = new LoadMesh();
			pMesh->LoadMeshFromFile(OpenedFile);
			pObject->SetMesh(pMesh);
		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{
			::ReadStringFromFile(OpenedFile, pstrToken); //<Mesh>:
			if (!strcmp(pstrToken, "<Mesh>:"));

		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{

		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
	
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return(pObject);
}

LoadedModelInfo* Object::LoadAnimationModel(char* pstrFileName)
{
	FILE* OpenedFile = NULL;
	::fopen_s(&OpenedFile, pstrFileName, "rb");
	::rewind(OpenedFile);

	LoadedModelInfo* pLoadedModel = new LoadedModelInfo();
	char pstrToken[64] = { '\0' };
	while (true)
	{
		if (::ReadStringFromFile(OpenedFile, pstrToken))
		{

			if (!strcmp(pstrToken, "<Hierarchy>:"))
			{
				pLoadedModel->m_pRoot = Object::LoadHierarchy(OpenedFile);
				::ReadStringFromFile(OpenedFile, pstrToken);

			}
			else if (!strcmp(pstrToken, "<Animation>:"))
			{
				pLoadedModel->PrepareSkinning();
			}
			else if (!strcmp(pstrToken, "</Animation>:"))
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	return pLoadedModel;
}

void Object::SetMesh(Mesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

UINT Object::GetMeshType()
{
	{ return((m_pMesh) ? m_pMesh->GetType() : 0x00); }
}

void Object::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

/////////////////////////////////////////////////////////////////////////////

BoundBox::BoundBox() : Object(BOUNDING_OBJECT)
{
}

BoundBox::~BoundBox()
{
}

void BoundBox::Transform(_Out_ BoundBox& Out,_In_ FXMMATRIX M)
{
	// Load the box.
	XMVECTOR vCenter = XMLoadFloat3(&Center);
	XMVECTOR vExtents = XMLoadFloat3(&Extents);
	XMVECTOR vOrientation = XMLoadFloat4(&Orientation);

	assert(DirectX::Internal::XMQuaternionIsUnit(vOrientation));

	// Composite the box rotation and the transform rotation.
	XMMATRIX nM;
	nM.r[0] = XMVector3Normalize(M.r[0]);
	nM.r[1] = XMVector3Normalize(M.r[1]);
	nM.r[2] = XMVector3Normalize(M.r[2]);
	nM.r[3] = g_XMIdentityR3;
	XMVECTOR Rotation = XMQuaternionRotationMatrix(nM);
	vOrientation = XMQuaternionMultiply(vOrientation, Rotation);

	// Transform the center.
	vCenter = XMVector3Transform(vCenter, M);

	// Scale the box extents.
	XMVECTOR dX = XMVector3Length(M.r[0]);
	XMVECTOR dY = XMVector3Length(M.r[1]);
	XMVECTOR dZ = XMVector3Length(M.r[2]);

	XMVECTOR VectorScale = XMVectorSelect(dY, dX, g_XMSelect1000);
	VectorScale = XMVectorSelect(dZ, VectorScale, g_XMSelect1100);
	vExtents = XMVectorMultiply(vExtents, VectorScale);

	// Store the box.
	XMStoreFloat3(&Out.Center, vCenter);
	XMStoreFloat3(&Out.Extents, vExtents);
	XMStoreFloat4(&Out.Orientation, vOrientation);
}

bool BoundBox::Intersects(BoundBox& box)
{
	// Build the 3x3 rotation matrix that defines the orientation of B relative to A.
	XMVECTOR A_quat = XMLoadFloat4(&Orientation);
	XMVECTOR B_quat = XMLoadFloat4(&box.Orientation);

	assert(DirectX::Internal::XMQuaternionIsUnit(A_quat));
	assert(DirectX::Internal::XMQuaternionIsUnit(B_quat));

	XMVECTOR Q = XMQuaternionMultiply(A_quat, XMQuaternionConjugate(B_quat));
	XMMATRIX R = XMMatrixRotationQuaternion(Q);

	// Compute the translation of B relative to A.
	XMVECTOR A_cent = XMLoadFloat3(&Center);
	XMVECTOR B_cent = XMLoadFloat3(&box.Center);
	XMVECTOR t = XMVector3InverseRotate(XMVectorSubtract(B_cent, A_cent), A_quat);

	//
	// h(A) = extents of A.
	// h(B) = extents of B.
	//
	// a(u) = axes of A = (1,0,0), (0,1,0), (0,0,1)
	// b(u) = axes of B relative to A = (r00,r10,r20), (r01,r11,r21), (r02,r12,r22)
	//
	// For each possible separating axis l:
	//   d(A) = sum (for i = u,v,w) h(A)(i) * abs( a(i) dot l )
	//   d(B) = sum (for i = u,v,w) h(B)(i) * abs( b(i) dot l )
	//   if abs( t dot l ) > d(A) + d(B) then disjoint
	//

	// Load extents of A and B.
	XMVECTOR h_A = XMLoadFloat3(&Extents);
	XMVECTOR h_B = XMLoadFloat3(&box.Extents);

	// Rows. Note R[0,1,2]X.w = 0.
	XMVECTOR R0X = R.r[0];
	XMVECTOR R1X = R.r[1];
	XMVECTOR R2X = R.r[2];

	R = XMMatrixTranspose(R);

	// Columns. Note RX[0,1,2].w = 0.
	XMVECTOR RX0 = R.r[0];
	XMVECTOR RX1 = R.r[1];
	XMVECTOR RX2 = R.r[2];

	// Absolute value of rows.
	XMVECTOR AR0X = XMVectorAbs(R0X);
	XMVECTOR AR1X = XMVectorAbs(R1X);
	XMVECTOR AR2X = XMVectorAbs(R2X);

	// Absolute value of columns.
	XMVECTOR ARX0 = XMVectorAbs(RX0);
	XMVECTOR ARX1 = XMVectorAbs(RX1);
	XMVECTOR ARX2 = XMVectorAbs(RX2);

	// Test each of the 15 possible seperating axii.
	XMVECTOR d, d_A, d_B;

	// l = a(u) = (1, 0, 0)
	// t dot l = t.x
	// d(A) = h(A).x
	// d(B) = h(B) dot abs(r00, r01, r02)
	d = XMVectorSplatX(t);
	d_A = XMVectorSplatX(h_A);
	d_B = XMVector3Dot(h_B, AR0X);
	XMVECTOR NoIntersection = XMVectorGreater(XMVectorAbs(d), XMVectorAdd(d_A, d_B));

	// l = a(v) = (0, 1, 0)
	// t dot l = t.y
	// d(A) = h(A).y
	// d(B) = h(B) dot abs(r10, r11, r12)
	d = XMVectorSplatY(t);
	d_A = XMVectorSplatY(h_A);
	d_B = XMVector3Dot(h_B, AR1X);
	NoIntersection = XMVectorOrInt(NoIntersection,
		XMVectorGreater(XMVectorAbs(d), XMVectorAdd(d_A, d_B)));

	// l = a(w) = (0, 0, 1)
	// t dot l = t.z
	// d(A) = h(A).z
	// d(B) = h(B) dot abs(r20, r21, r22)
	d = XMVectorSplatZ(t);
	d_A = XMVectorSplatZ(h_A);
	d_B = XMVector3Dot(h_B, AR2X);
	NoIntersection = XMVectorOrInt(NoIntersection,
		XMVectorGreater(XMVectorAbs(d), XMVectorAdd(d_A, d_B)));

	// l = b(u) = (r00, r10, r20)
	// d(A) = h(A) dot abs(r00, r10, r20)
	// d(B) = h(B).x
	d = XMVector3Dot(t, RX0);
	d_A = XMVector3Dot(h_A, ARX0);
	d_B = XMVectorSplatX(h_B);
	NoIntersection = XMVectorOrInt(NoIntersection,
		XMVectorGreater(XMVectorAbs(d), XMVectorAdd(d_A, d_B)));

	// l = b(v) = (r01, r11, r21)
	// d(A) = h(A) dot abs(r01, r11, r21)
	// d(B) = h(B).y
	d = XMVector3Dot(t, RX1);
	d_A = XMVector3Dot(h_A, ARX1);
	d_B = XMVectorSplatY(h_B);
	NoIntersection = XMVectorOrInt(NoIntersection,
		XMVectorGreater(XMVectorAbs(d), XMVectorAdd(d_A, d_B)));

	// l = b(w) = (r02, r12, r22)
	// d(A) = h(A) dot abs(r02, r12, r22)
	// d(B) = h(B).z
	d = XMVector3Dot(t, RX2);
	d_A = XMVector3Dot(h_A, ARX2);
	d_B = XMVectorSplatZ(h_B);
	NoIntersection = XMVectorOrInt(NoIntersection,
		XMVectorGreater(XMVectorAbs(d), XMVectorAdd(d_A, d_B)));

	// l = a(u) x b(u) = (0, -r20, r10)
	// d(A) = h(A) dot abs(0, r20, r10)
	// d(B) = h(B) dot abs(0, r02, r01)
	d = XMVector3Dot(t, XMVectorPermute<XM_PERMUTE_0W, XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_0X>(RX0, XMVectorNegate(RX0)));
	d_A = XMVector3Dot(h_A, XMVectorSwizzle<XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_X>(ARX0));
	d_B = XMVector3Dot(h_B, XMVectorSwizzle<XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_X>(AR0X));
	NoIntersection = XMVectorOrInt(NoIntersection,
		XMVectorGreater(XMVectorAbs(d), XMVectorAdd(d_A, d_B)));

	// l = a(u) x b(v) = (0, -r21, r11)
	// d(A) = h(A) dot abs(0, r21, r11)
	// d(B) = h(B) dot abs(r02, 0, r00)
	d = XMVector3Dot(t, XMVectorPermute<XM_PERMUTE_0W, XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_0X>(RX1, XMVectorNegate(RX1)));
	d_A = XMVector3Dot(h_A, XMVectorSwizzle<XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_X>(ARX1));
	d_B = XMVector3Dot(h_B, XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_X, XM_SWIZZLE_Y>(AR0X));
	NoIntersection = XMVectorOrInt(NoIntersection,
		XMVectorGreater(XMVectorAbs(d), XMVectorAdd(d_A, d_B)));

	// l = a(u) x b(w) = (0, -r22, r12)
	// d(A) = h(A) dot abs(0, r22, r12)
	// d(B) = h(B) dot abs(r01, r00, 0)
	d = XMVector3Dot(t, XMVectorPermute<XM_PERMUTE_0W, XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_0X>(RX2, XMVectorNegate(RX2)));
	d_A = XMVector3Dot(h_A, XMVectorSwizzle<XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_X>(ARX2));
	d_B = XMVector3Dot(h_B, XMVectorSwizzle<XM_SWIZZLE_Y, XM_SWIZZLE_X, XM_SWIZZLE_W, XM_SWIZZLE_Z>(AR0X));
	NoIntersection = XMVectorOrInt(NoIntersection,
		XMVectorGreater(XMVectorAbs(d), XMVectorAdd(d_A, d_B)));

	// l = a(v) x b(u) = (r20, 0, -r00)
	// d(A) = h(A) dot abs(r20, 0, r00)
	// d(B) = h(B) dot abs(0, r12, r11)
	d = XMVector3Dot(t, XMVectorPermute<XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1X, XM_PERMUTE_0Y>(RX0, XMVectorNegate(RX0)));
	d_A = XMVector3Dot(h_A, XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_X, XM_SWIZZLE_Y>(ARX0));
	d_B = XMVector3Dot(h_B, XMVectorSwizzle<XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_X>(AR1X));
	NoIntersection = XMVectorOrInt(NoIntersection,
		XMVectorGreater(XMVectorAbs(d), XMVectorAdd(d_A, d_B)));

	// l = a(v) x b(v) = (r21, 0, -r01)
	// d(A) = h(A) dot abs(r21, 0, r01)
	// d(B) = h(B) dot abs(r12, 0, r10)
	d = XMVector3Dot(t, XMVectorPermute<XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1X, XM_PERMUTE_0Y>(RX1, XMVectorNegate(RX1)));
	d_A = XMVector3Dot(h_A, XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_X, XM_SWIZZLE_Y>(ARX1));
	d_B = XMVector3Dot(h_B, XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_X, XM_SWIZZLE_Y>(AR1X));
	NoIntersection = XMVectorOrInt(NoIntersection,
		XMVectorGreater(XMVectorAbs(d), XMVectorAdd(d_A, d_B)));

	// l = a(v) x b(w) = (r22, 0, -r02)
	// d(A) = h(A) dot abs(r22, 0, r02)
	// d(B) = h(B) dot abs(r11, r10, 0)
	d = XMVector3Dot(t, XMVectorPermute<XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1X, XM_PERMUTE_0Y>(RX2, XMVectorNegate(RX2)));
	d_A = XMVector3Dot(h_A, XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_X, XM_SWIZZLE_Y>(ARX2));
	d_B = XMVector3Dot(h_B, XMVectorSwizzle<XM_SWIZZLE_Y, XM_SWIZZLE_X, XM_SWIZZLE_W, XM_SWIZZLE_Z>(AR1X));
	NoIntersection = XMVectorOrInt(NoIntersection,
		XMVectorGreater(XMVectorAbs(d), XMVectorAdd(d_A, d_B)));

	// l = a(w) x b(u) = (-r10, r00, 0)
	// d(A) = h(A) dot abs(r10, r00, 0)
	// d(B) = h(B) dot abs(0, r22, r21)
	d = XMVector3Dot(t, XMVectorPermute<XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0Z>(RX0, XMVectorNegate(RX0)));
	d_A = XMVector3Dot(h_A, XMVectorSwizzle<XM_SWIZZLE_Y, XM_SWIZZLE_X, XM_SWIZZLE_W, XM_SWIZZLE_Z>(ARX0));
	d_B = XMVector3Dot(h_B, XMVectorSwizzle<XM_SWIZZLE_W, XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_X>(AR2X));
	NoIntersection = XMVectorOrInt(NoIntersection,
		XMVectorGreater(XMVectorAbs(d), XMVectorAdd(d_A, d_B)));

	// l = a(w) x b(v) = (-r11, r01, 0)
	// d(A) = h(A) dot abs(r11, r01, 0)
	// d(B) = h(B) dot abs(r22, 0, r20)
	d = XMVector3Dot(t, XMVectorPermute<XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0Z>(RX1, XMVectorNegate(RX1)));
	d_A = XMVector3Dot(h_A, XMVectorSwizzle<XM_SWIZZLE_Y, XM_SWIZZLE_X, XM_SWIZZLE_W, XM_SWIZZLE_Z>(ARX1));
	d_B = XMVector3Dot(h_B, XMVectorSwizzle<XM_SWIZZLE_Z, XM_SWIZZLE_W, XM_SWIZZLE_X, XM_SWIZZLE_Y>(AR2X));
	NoIntersection = XMVectorOrInt(NoIntersection,
		XMVectorGreater(XMVectorAbs(d), XMVectorAdd(d_A, d_B)));

	// l = a(w) x b(w) = (-r12, r02, 0)
	// d(A) = h(A) dot abs(r12, r02, 0)
	// d(B) = h(B) dot abs(r21, r20, 0)
	d = XMVector3Dot(t, XMVectorPermute<XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0Z>(RX2, XMVectorNegate(RX2)));
	d_A = XMVector3Dot(h_A, XMVectorSwizzle<XM_SWIZZLE_Y, XM_SWIZZLE_X, XM_SWIZZLE_W, XM_SWIZZLE_Z>(ARX2));
	d_B = XMVector3Dot(h_B, XMVectorSwizzle<XM_SWIZZLE_Y, XM_SWIZZLE_X, XM_SWIZZLE_W, XM_SWIZZLE_Z>(AR2X));
	NoIntersection = XMVectorOrInt(NoIntersection,
		XMVectorGreater(XMVectorAbs(d), XMVectorAdd(d_A, d_B)));

	// No seperating axis found, boxes must intersect.
	return XMVector4NotEqualInt(NoIntersection, XMVectorTrueInt()) ? true : false;
}
bool BoundBox::Intersects(BoundSphere& sh)
{
	XMVECTOR SphereCenter = XMLoadFloat3(&sh.Center);
	XMVECTOR SphereRadius = XMVectorReplicatePtr(&sh.Radius);

	XMVECTOR BoxCenter = XMLoadFloat3(&Center);
	XMVECTOR BoxExtents = XMLoadFloat3(&Extents);
	XMVECTOR BoxOrientation = XMLoadFloat4(&Orientation);

	assert(DirectX::Internal::XMQuaternionIsUnit(BoxOrientation));

	// Transform the center of the sphere to be local to the box.
	// BoxMin = -BoxExtents
	// BoxMax = +BoxExtents
	SphereCenter = XMVector3InverseRotate(XMVectorSubtract(SphereCenter, BoxCenter), BoxOrientation);

	// Find the distance to the nearest point on the box.
	// for each i in (x, y, z)
	// if (SphereCenter(i) < BoxMin(i)) d2 += (SphereCenter(i) - BoxMin(i)) ^ 2
	// else if (SphereCenter(i) > BoxMax(i)) d2 += (SphereCenter(i) - BoxMax(i)) ^ 2

	XMVECTOR d = XMVectorZero();

	// Compute d for each dimension.
	XMVECTOR LessThanMin = XMVectorLess(SphereCenter, XMVectorNegate(BoxExtents));
	XMVECTOR GreaterThanMax = XMVectorGreater(SphereCenter, BoxExtents);

	XMVECTOR MinDelta = XMVectorAdd(SphereCenter, BoxExtents);
	XMVECTOR MaxDelta = XMVectorSubtract(SphereCenter, BoxExtents);

	// Choose value for each dimension based on the comparison.
	d = XMVectorSelect(d, MinDelta, LessThanMin);
	d = XMVectorSelect(d, MaxDelta, GreaterThanMax);

	// Use a dot-product to square them and sum them together.
	XMVECTOR d2 = XMVector3Dot(d, d);

	return XMVector4LessOrEqual(d2, XMVectorMultiply(SphereRadius, SphereRadius)) ? true : false;
}

BoundSphere::BoundSphere() : Object(BOUNDING_OBJECT)
{
}

BoundSphere::BoundSphere(bool Push_List) : Object(Push_List)
{
}

void BoundSphere::Transform(_Out_ BoundSphere& Out, _In_ FXMMATRIX M)
{
	// Load the center of the sphere.
	XMVECTOR vCenter = XMLoadFloat3(&Center);

	// Transform the center of the sphere.
	XMVECTOR C = XMVector3Transform(vCenter, M);

	XMVECTOR dX = XMVector3Dot(M.r[0], M.r[0]);
	XMVECTOR dY = XMVector3Dot(M.r[1], M.r[1]);
	XMVECTOR dZ = XMVector3Dot(M.r[2], M.r[2]);

	XMVECTOR d = XMVectorMax(dX, XMVectorMax(dY, dZ));

	// Store the center sphere.
	XMStoreFloat3(&Out.Center, C);

	// Scale the radius of the pshere.
	float Scale = sqrtf(XMVectorGetX(d));
	Out.Radius = Radius * Scale;
}

bool BoundSphere::Intersects(BoundBox& box)
{
	return box.Intersects(*this);
}
bool BoundSphere::Intersects(BoundSphere& sh)
{
	XMVECTOR vCenterA = XMLoadFloat3(&Center);
	XMVECTOR vRadiusA = XMVectorReplicatePtr(&Radius);


	XMVECTOR vCenterB = XMLoadFloat3(&sh.Center);
	XMVECTOR vRadiusB = XMVectorReplicatePtr(&sh.Radius);

	XMVECTOR Delta = XMVectorSubtract(vCenterB, vCenterA);
	XMVECTOR DistanceSquared = XMVector3LengthSq(Delta);

	XMVECTOR RadiusSquared = XMVectorAdd(vRadiusA, vRadiusB);
	RadiusSquared = XMVectorMultiply(RadiusSquared, RadiusSquared);

	return XMVector3LessOrEqual(DistanceSquared, RadiusSquared);
}
