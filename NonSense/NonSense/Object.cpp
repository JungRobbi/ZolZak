#include "stdafx.h"
#include "Object.h"
#include "Shader.h"

void Material::SetTexture(Texture* pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}

Texture::Texture(int nTextures, UINT nTextureType, int nRootParameters)
{
	m_nTextures = nTextures;
	m_nTextureType = nTextureType;

	if (nTextures > 0)
	{
		m_ppd3dTextureUploadBuffers = new ID3D12Resource * [m_nTextures];
		m_ppd3dTextures = new ID3D12Resource * [m_nTextures];
		for (int i = 0; i < nTextures; ++i)
			m_ppd3dTextureUploadBuffers[i] = m_ppd3dTextures[i] = NULL;
	}

	m_nRootParameters = nRootParameters;
	if (nRootParameters > 0) m_pRootParameterIndices = new int[nRootParameters];
	for (int i = 0; i < m_nRootParameters; i++) m_pRootParameterIndices[i] = -1;
}

Texture::~Texture()
{

	if (m_ppd3dTextures) delete[] m_ppd3dTextures;
	if (m_ppd3dTextureUploadBuffers) delete[] m_ppd3dTextureUploadBuffers;
	
	if (m_pRootParameterIndices) delete[] m_pRootParameterIndices;
	if (m_pd3dSRVGPUDescriptorHandle) delete[] m_pd3dSRVGPUDescriptorHandle;

}

D3D12_SHADER_RESOURCE_VIEW_DESC Texture::GetShaderResourceViewDesc(int nIndex)
{
	ID3D12Resource* pShaderResource = GetResource(nIndex);
	D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	int nTextureType = GetTextureType(nIndex);
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY: //[]
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = 1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.Format = m_pdxgiBufferFormats[nIndex];
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = m_pnBufferElements[nIndex];
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}

void Texture::SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pd3dSRVGPUDescriptorHandle[nIndex] = d3dSrvGpuDescriptorHandle;
}

void Texture::LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ);
}

bool Texture::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, Object* pParent, FILE* pInFile, Shader* pShader, UINT nIndex)
{
	char pstrTextureName[64] = { '\0' };

	BYTE nStrLength = 64;
	UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrTextureName, sizeof(char), nStrLength, pInFile);
	pstrTextureName[nStrLength] = '\0';

	bool bDuplicated = false;
	bool bLoaded = false;
	if (strcmp(pstrTextureName, "null"))
	{
		

		bLoaded = true;
		char pstrFilePath[64] = { '\0' };
		strcpy_s(pstrFilePath, 64, "Model/Textures/");

		bDuplicated = (pstrTextureName[0] == '@');
		strcpy_s(pstrFilePath + 15, 64 - 15, (bDuplicated) ? (pstrTextureName + 1) : pstrTextureName);
		strcpy_s(pstrFilePath + 15 + ((bDuplicated) ? (nStrLength - 1) : nStrLength), 64 - 15 - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

		size_t nConverted = 0;
		mbstowcs_s(&nConverted, m_ppstrTextureNames[nIndex], 64, pstrFilePath, _TRUNCATE);

#define _WITH_DISPLAY_TEXTURE_NAME

#ifdef _WITH_DISPLAY_TEXTURE_NAME
		static int nTextures = 0, nRepeatedTextures = 0;
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("Texture Name: %d %c %s\n"), (pstrTextureName[0] == '@') ? nRepeatedTextures++ : nTextures++, (pstrTextureName[0] == '@') ? '@' : ' ', m_ppstrTextureNames[nIndex]);
		OutputDebugString(pstrDebug);
#endif
		if (!bDuplicated)
		{
			LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, m_ppstrTextureNames[nIndex], RESOURCE_TEXTURE2D, nIndex);
			pShader->CreateShaderResourceView(pd3dDevice, this, nIndex);
#ifdef _WITH_STANDARD_TEXTURE_MULTIPLE_DESCRIPTORS
			m_pnRootParameterIndices[nIndex] = PARAMETER_STANDARD_TEXTURE + nIndex;
#endif
		}
		else
		{
			if (pParent)
			{
				Object* pRootGameObject = pParent;
				while (pRootGameObject)
				{
					if (!pRootGameObject->m_pParent) break;
					pRootGameObject = pRootGameObject->m_pParent;
				}
				D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle;
				int nParameterIndex = pRootGameObject->FindReplicatedTexture(m_ppstrTextureNames[nIndex], &d3dSrvGpuDescriptorHandle);
				if (nParameterIndex >= 0)
				{
					m_pd3dSRVGPUDescriptorHandle[nIndex] = d3dSrvGpuDescriptorHandle;
					m_pRootParameterIndices[nIndex] = nParameterIndex;
				}
			}
		}
	}
	return(bLoaded);
}

void Material::SetShader(Shader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

AnimationSet::AnimationSet(float fLength, int nFramesPerSecond, int nKeyFrames, int nAnimatedBones, char* pstrName)
{
	m_Length = fLength;
	m_FramePerSecond = nFramesPerSecond;
	m_KeyFrames = nKeyFrames;

	strcpy_s(m_AnimationSetName, 64, pstrName);

	m_pKeyFrameTimes = new float[nKeyFrames];
	m_ppKeyFrameTransforms = new XMFLOAT4X4 * [nKeyFrames];
	for (int i = 0; i < nKeyFrames; i++) m_ppKeyFrameTransforms[i] = new XMFLOAT4X4[nAnimatedBones];
}

AnimationSet::~AnimationSet()
{
	if (m_pKeyFrameTimes) delete[] m_pKeyFrameTimes;
	for (int j = 0; j < m_KeyFrames; j++) if (m_ppKeyFrameTransforms[j]) delete[] m_ppKeyFrameTransforms[j];
	if (m_ppKeyFrameTransforms) delete[] m_ppKeyFrameTransforms;
}

AnimationSets::AnimationSets(int nAnimationSets)
{
	m_nAnimationSets = nAnimationSets;
	m_pAnimationSets = new AnimationSet * [nAnimationSets];
}

AnimationSets::~AnimationSets()
{
	for (int i = 0; i < m_nAnimationSets; i++) if (m_pAnimationSets[i]) delete m_pAnimationSets[i];
	if (m_pAnimationSets) delete[] m_pAnimationSets;

	if (m_ppAnimatedBoneFrameCaches) delete[] m_ppAnimatedBoneFrameCaches;
}


void LoadedModelInfo::PrepareSkinning()
{
	int nSkinnedMesh = 0;
	m_ppSkinnedMeshes = new SkinnedMesh * [m_nSkinnedMeshes];
	m_pRoot->FindAndSetSkinnedMesh(m_ppSkinnedMeshes, &nSkinnedMesh);

	for (int i = 0; i < m_nSkinnedMeshes; i++) m_ppSkinnedMeshes[i]->PrepareSkinning(m_pRoot);
}



Object::Object()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}
Object::~Object()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pMaterial) m_pMaterial->Release();
}

void Object::SetChild(Object* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) pChild->AddRef();
	}
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void Object::SetShader(Shader* pShader)
{
	if (!m_pMaterial)
	{
		m_pMaterial = new Material();
		m_pMaterial->AddRef();
	}
	if (m_pMaterial) m_pMaterial->SetShader(pShader);
}

void Object::SetMaterials(int nMaterial, Material* pMaterial)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->Release();
	m_ppMaterials[nMaterial] = pMaterial;
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->AddRef();
}

void Object::SetMaterial(Material* pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}
void Object::SetMaterial(UINT nReflection)
{
	if (!m_pMaterial) m_pMaterial = new Material();
	m_pMaterial->m_nReflection = nReflection;
}

bool Object::IsVisible(Camera* pCamera)
{
	OnPrepareRender();
	bool bIsVisible = false;
	BoundingOrientedBox xmBoundingBox = m_pMesh->GetBoundingBox();
	//모델 좌표계의 바운딩 박스를 월드 좌표계로 변환한다.
	xmBoundingBox.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
	if (pCamera) bIsVisible = pCamera->IsInFrustum(xmBoundingBox);
	return(bIsVisible);
}

void Object::SetMesh(Mesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void Object::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}
void Object::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}
XMFLOAT3 Object::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 Object::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32,
		m_xmf4x4World._33)));
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
}

void Object::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void Object::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void Object::ReleaseShaderVariables()
{
}

void Object::FindAndSetSkinnedMesh(SkinnedMesh** ppSkinnedMeshes, int* pnSkinnedMesh)
{
	if (m_pMesh) ppSkinnedMeshes[(*pnSkinnedMesh)++] = (SkinnedMesh*)m_pMesh;

	if (m_pSibling) m_pSibling->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
	if (m_pChild) m_pChild->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
}

Object* Object::FindFrame(char* pstrFrameName)
{
	Object* pObject = NULL;
	if (!strncmp(m_pFrameName, pstrFrameName, strlen(pstrFrameName))) return(this);
	if (m_pSibling)
		if (pObject = m_pSibling->FindFrame(pstrFrameName))
			return (pObject);
	if (m_pChild)
		if (pObject = m_pChild->FindFrame(pstrFrameName))
			return (pObject);

	return(NULL);
}

int Object::FindReplicatedTexture(_TCHAR* pstrTextureName, D3D12_GPU_DESCRIPTOR_HANDLE* pd3dSrvGpuDescriptorHandle)
{
	int nParameterIndex = -1;

	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i] && m_ppMaterials[i]->m_pTexture)
		{
			int nTextures = m_ppMaterials[i]->m_pTexture->GetTextures();
			for (int j = 0; j < nTextures; j++)
			{
				if (!_tcsncmp(m_ppMaterials[i]->m_pTexture->GetTextureName(j), pstrTextureName, _tcslen(pstrTextureName)))
				{
					*pd3dSrvGpuDescriptorHandle = m_ppMaterials[i]->m_pTexture->GetGpuDescriptorHandle(j);
					nParameterIndex = m_ppMaterials[i]->m_pTexture->GetRootParameter(j);
					return(nParameterIndex);
				}
			}
		}
	}
	if (m_pSibling) if ((nParameterIndex = m_pSibling->FindReplicatedTexture(pstrTextureName, pd3dSrvGpuDescriptorHandle)) > 0) return(nParameterIndex);
	if (m_pChild) if ((nParameterIndex = m_pChild->FindReplicatedTexture(pstrTextureName, pd3dSrvGpuDescriptorHandle)) > 0) return(nParameterIndex);

	return(nParameterIndex);
}

// -------------- 모델 & 애니메이션 로드 --------------

int ReadIntegerFromFile(FILE* pInFile)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, pInFile);
	return(nValue);
}

float ReadFloatFromFile(FILE* pInFile)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, pInFile);
	return(fValue);
}

BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken)
{
	BYTE nStrLength = 0;
	UINT nReads = 0;
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
	pstrToken[nStrLength] = '\0';

	return(nStrLength);
}

void Object::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, Object* pParent, FILE* OpenedFile, Shader* pShader)
{
	char pstrToken[64] = { '\0' };
	int nMaterial = 0;
	UINT nReads = 0;

	m_nMaterials = ::ReadIntegerFromFile(OpenedFile);
	m_ppMaterials = new Material*[m_nMaterials];
	for (int i = 0; i < m_nMaterials; ++i) m_ppMaterials[i] = NULL;

	Material* pMaterial = NULL;
	Texture* pTexture = NULL;

	while (true)
	{
		::ReadStringFromFile(OpenedFile, pstrToken);
		if (!strcmp(pstrToken, "<Material>:"))
		{
			nMaterial = ReadIntegerFromFile(OpenedFile);

			pMaterial = new Material;
			pTexture = new Texture(7, RESOURCE_TEXTURE2D, 7);

			pMaterial->SetTexture(pTexture);
			SetMaterials(nMaterial, pMaterial);
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			nReads = ::fread(&(pMaterial->m_xmf4Albedo), sizeof(float), 4, OpenedFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			nReads = ::fread(&(pMaterial->m_xmf4Emissive), sizeof(float), 4, OpenedFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			nReads = ::fread(&(pMaterial->m_xmf4Specular), sizeof(float), 4, OpenedFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			nReads = ::fread(&(pMaterial->m_fGlossiness), sizeof(float), 1, OpenedFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			nReads = ::fread(&(pMaterial->m_fSmoothness), sizeof(float), 1, OpenedFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			nReads = ::fread(&(pMaterial->m_fMetallic), sizeof(float), 1, OpenedFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			nReads = ::fread(&(pMaterial->m_fSpecularHighlight), sizeof(float), 1, OpenedFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			nReads = ::fread(&(pMaterial->m_fGlossyReflection), sizeof(float), 1, OpenedFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, OpenedFile, pShader, 0)) pMaterial->SetMaterialType(MATERIAL_ALBEDO_MAP);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, OpenedFile, pShader, 1)) pMaterial->SetMaterialType(MATERIAL_SPECULAR_MAP);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, OpenedFile, pShader, 2)) pMaterial->SetMaterialType(MATERIAL_NORMAL_MAP);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, OpenedFile, pShader, 3)) pMaterial->SetMaterialType(MATERIAL_METALLIC_MAP);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, OpenedFile, pShader, 4)) pMaterial->SetMaterialType(MATERIAL_EMISSION_MAP);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, OpenedFile, pShader, 5)) pMaterial->SetMaterialType(MATERIAL_DETAIL_ALBEDO_MAP);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			if (pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pParent, OpenedFile, pShader, 6)) pMaterial->SetMaterialType(MATERIAL_DETAIL_NORMAL_MAP);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}


}

Object* Object::LoadHierarchy(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, Object* pParent, FILE* OpenedFile, Shader* pShader, int* pnSkinnedMeshes)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	Object* pObject = new Object();

	while (true)
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
			XMFLOAT3 Position, Rotation, Scale;
			XMFLOAT4 Quat;

			nReads = (UINT)::fread(&Position, sizeof(float), 3, OpenedFile);
			nReads = (UINT)::fread(&Rotation, sizeof(float), 3, OpenedFile); //Euler Angle
			nReads = (UINT)::fread(&Scale, sizeof(float), 3, OpenedFile);
			nReads = (UINT)::fread(&Quat, sizeof(float), 4, OpenedFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&pObject->m_xmf4x4ToParent, sizeof(float), 16, OpenedFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			LoadMesh* pMesh = new LoadMesh(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, OpenedFile);
			pObject->SetMesh(pMesh);
		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{
			if (pnSkinnedMeshes) (*pnSkinnedMeshes)++;

			SkinnedMesh* pSkinnedMesh = new SkinnedMesh(pd3dDevice, pd3dCommandList);
			pSkinnedMesh->LoadSkinInfoFromFile(pd3dDevice, pd3dCommandList, OpenedFile);
			

			::ReadStringFromFile(OpenedFile, pstrToken); //<Mesh>:
			if (!strcmp(pstrToken, "<Mesh>:")) pSkinnedMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, OpenedFile);

			pObject->SetMesh(pSkinnedMesh);

		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			pObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pParent, OpenedFile, pShader);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChildren = ::ReadIntegerFromFile(OpenedFile);
			if (nChildren > 0)
			{
				for (int i = 0; i < nChildren; i++)
				{
					Object* pChild = Object::LoadHierarchy(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pObject, OpenedFile, pShader, pnSkinnedMeshes);
					if (pChild) pObject->SetChild(pChild);
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return pObject;
}

void Object::LoadAnimationFromFile(FILE* OpenedFile, LoadedModelInfo* pLoadModel)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nAnimationSets = 0;
	while (true)
	{
		::ReadStringFromFile(OpenedFile, pstrToken);
		if (!strcmp(pstrToken, "<AnimationSets>:"))
		{
			nAnimationSets = ::ReadIntegerFromFile(OpenedFile);
			pLoadModel->m_pAnimationSets = new AnimationSets(nAnimationSets);
		}
		else if (!strcmp(pstrToken, "<FrameNames>:"))
		{
			pLoadModel->m_pAnimationSets->m_nAnimatedBoneFrames = ::ReadIntegerFromFile(OpenedFile);
			pLoadModel->m_pAnimationSets->m_ppAnimatedBoneFrameCaches = new Object * [pLoadModel->m_pAnimationSets->m_nAnimatedBoneFrames];
			for (int i = 0; i < pLoadModel->m_pAnimationSets->m_nAnimatedBoneFrames; ++i)
			{
				::ReadStringFromFile(OpenedFile, pstrToken);
				pLoadModel->m_pAnimationSets->m_ppAnimatedBoneFrameCaches[i] = pLoadModel->m_pRoot->FindFrame(pstrToken);

			}
		}
		else if (!strcmp(pstrToken, "<AnimationSet>:"))
		{
			int nAnimationSet = ::ReadIntegerFromFile(OpenedFile);

			::ReadStringFromFile(OpenedFile, pstrToken); //Animation Set Name

			float fLength = ::ReadFloatFromFile(OpenedFile);
			int nFramesPerSecond = ::ReadIntegerFromFile(OpenedFile);
			int nKeyFrames = ::ReadIntegerFromFile(OpenedFile);

			pLoadModel->m_pAnimationSets->m_pAnimationSets[nAnimationSet] = new AnimationSet(fLength, nFramesPerSecond, nKeyFrames, pLoadModel->m_pAnimationSets->m_nAnimatedBoneFrames, pstrToken);

			for (int i = 0; i < nKeyFrames; i++)
			{
				::ReadStringFromFile(OpenedFile, pstrToken);
				if (!strcmp(pstrToken, "<Transforms>:"))
				{
					int nKey = ::ReadIntegerFromFile(OpenedFile); //i
					float fKeyTime = ::ReadFloatFromFile(OpenedFile);

					AnimationSet* pAnimationSet = pLoadModel->m_pAnimationSets->m_pAnimationSets[nAnimationSet];
					pAnimationSet->m_pKeyFrameTimes[i] = fKeyTime;
					nReads = (UINT)::fread(pAnimationSet->m_ppKeyFrameTransforms[i], sizeof(XMFLOAT4X4), pLoadModel->m_pAnimationSets->m_nAnimatedBoneFrames, OpenedFile);
				}
			}


		}
	}
}



LoadedModelInfo* Object::LoadAnimationModel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, Shader* pShader)
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
				pLoadedModel->m_pRoot = Object::LoadHierarchy(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL, OpenedFile, pShader, &pLoadedModel->m_nSkinnedMeshes);
				::ReadStringFromFile(OpenedFile, pstrToken);

			}
			else if (!strcmp(pstrToken, "<Animation>:"))
			{
				Object::LoadAnimationFromFile(OpenedFile, pLoadedModel);
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




void Object::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	//객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다.
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void Object::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}
void Object::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}
void Object::Animate(float fTimeElapsed)
{
}
void Object::OnPrepareRender()
{
}
void Object::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	if (IsVisible(pCamera))
	{
		OnPrepareRender();
		if (m_pMaterial)
		{
			if (m_pMaterial->m_pShader)
			{
				m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
				m_pMaterial->m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
			}
		}
		if (m_pMesh) m_pMesh->Render(pd3dCommandList);
	}
}
void Object::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView)
{
	if (IsVisible(pCamera))
	{
		OnPrepareRender();
		if (m_pMesh) m_pMesh->Render(pd3dCommandList, nInstances, d3dInstancingBufferView);
	}
}

void Object::GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection)
{
	XMFLOAT4X4 xmf4x4WorldView = Matrix4x4::Multiply(m_xmf4x4World, xmf4x4View);
	XMFLOAT4X4 xmf4x4Inverse = Matrix4x4::Inverse(xmf4x4WorldView);
	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	//카메라 좌표계의 원점을 모델 좌표계로 변환한다.
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);
	//카메라 좌표계의 점(마우스 좌표를 역변환하여 구한 점)을 모델 좌표계로 변환한다.
	*pxmf3PickRayDirection= Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse);
	//광선의 방향 벡터를 구한다.
	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection, *pxmf3PickRayOrigin));
} 

int Object::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance)
{
	int nIntersected = 0;
	if (m_pMesh)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		//모델 좌표계의 광선을 생성한다.
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, &xmf3PickRayDirection);
		//모델 좌표계의 광선과 메쉬의 교차를 검사한다.
		nIntersected = m_pMesh->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, pfHitDistance);
	}
	return(nIntersected);
}

RotatingObject::RotatingObject()
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
}
RotatingObject::~RotatingObject()
{
}

void RotatingObject::Animate(float fTimeElapsed)
{
	Object::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}

