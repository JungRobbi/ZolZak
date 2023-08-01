#include "Object.h"
#include "Shader.h"
#include "GameScene.h"
#include "stdafx.h"
#include "BoxCollideComponent.h"
#include "SphereCollideComponent.h"
#include "GameFramework.h"
#include <cmath>

CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers, int nRootParameters)
{
	m_nTextureType = nTextureType;

	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_pRootArgumentInfos = new SRVROOTARGUMENTINFO[m_nTextures];
		m_ppd3dTextures = new ID3D12Resource * [m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_ppd3dTextures[i] = NULL;
		m_ppd3dTextureUploadBuffers = new ID3D12Resource * [m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_ppd3dTextureUploadBuffers[i] = NULL;
		m_pd3dSrvGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pd3dSrvGpuDescriptorHandles[i].ptr = NULL;

		m_pnResourceTypes = new UINT[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pnResourceTypes[i] = -1;

		m_pdxgiBufferFormats = new DXGI_FORMAT[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pnResourceTypes[i] = DXGI_FORMAT_UNKNOWN;
		m_pnBufferElements = new int[m_nTextures];
		for (int i = 0; i < m_nTextures; i++) m_pnBufferElements[i] = 0;
	}
	m_nRootParameters = nRootParameters;
	if (nRootParameters > 0) m_pnRootParameterIndices = new int[nRootParameters];
	for (int i = 0; i < m_nRootParameters; i++) m_pnRootParameterIndices[i] = -1;

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppd3dTextures)
	{
		for (int i = 0; i < m_nTextures; i++) {
			if (m_ppd3dTextures[i])
				m_ppd3dTextures[i]->Release();
		}
		delete[] m_ppd3dTextures;

	}
	if (m_pnResourceTypes) 
		delete[] m_pnResourceTypes;
	if (m_pdxgiBufferFormats) 
		delete[] m_pdxgiBufferFormats;
	if (m_pnBufferElements) 
		delete[] m_pnBufferElements;

	if (m_pnRootParameterIndices) 
		delete[] m_pnRootParameterIndices;
	if (m_pd3dSrvGpuDescriptorHandles) 
		delete[] m_pd3dSrvGpuDescriptorHandles;

	if (m_pd3dSamplerGpuDescriptorHandles) 
		delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetRootParameterIndex(int nIndex, UINT nRootParameterIndex)
{
	m_pnRootParameterIndices[nIndex] = nRootParameterIndex;
}

void CTexture::SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pd3dSrvGpuDescriptorHandles[nIndex] = d3dSrvGpuDescriptorHandle;
}


void CTexture::SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pRootArgumentInfos[nIndex].m_nRootParameterIndex = nRootParameterIndex;
	m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[0], m_pd3dSrvGpuDescriptorHandles[0]);
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int index)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[index].m_nRootParameterIndex, m_pRootArgumentInfos[index].m_d3dSrvGpuDescriptorHandle);
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[nParameterIndex], m_pd3dSrvGpuDescriptorHandles[nTextureIndex]);
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}
void CTexture::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ/*D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/);
}

ID3D12Resource* CTexture::CreateTexture(ID3D12Device* pd3dDevice, UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTexture2DResource(pd3dDevice, nWidth, nHeight, 1, 0, dxgiFormat, d3dResourceFlags, d3dResourceStates, pd3dClearValue);
	return(m_ppd3dTextures[nIndex]);
}

void CTexture::LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = RESOURCE_BUFFER;
	m_pdxgiBufferFormats[nIndex] = ndxgiFormat;
	m_pnBufferElements[nIndex] = nElements;
	m_ppd3dTextures[nIndex] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pData, nElements * nStride, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, &m_ppd3dTextureUploadBuffers[nIndex]);
}

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetShaderResourceViewDesc(int nIndex)
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
		d3dShaderResourceViewDesc.TextureCube.MipLevels = -1;
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

Material::Material(int Textures)
{
	m_nTextures = Textures;

	m_ppTextures = new CTexture * [m_nTextures];
	m_ppstrTextureNames = new _TCHAR[m_nTextures][64];
	for (int i = 0; i < m_nTextures; i++) m_ppTextures[i] = NULL;
	for (int i = 0; i < m_nTextures; i++) m_ppstrTextureNames[i][0] = '\0';
}
Material::~Material()
{
	if (m_pShader)
		m_pShader->Release();
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) m_ppTextures[i]->Release();
	}
	if (m_pTexture)
		m_pTexture->Release();
}
Shader* Material::m_pSkinnedAnimationShader = NULL;
Shader* Material::m_pStandardShader = NULL;
void Material::PrepareShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	DXGI_FORMAT pdxgiRtvFormats[MRT] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,  DXGI_FORMAT_R8G8B8A8_UNORM };

	m_pStandardShader = new StandardShader();
	m_pStandardShader->CreateShader(pd3dDevice,  pd3dGraphicsRootSignature, MRT, pdxgiRtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);

	m_pSkinnedAnimationShader = new SkinnedModelShader();
	m_pSkinnedAnimationShader->CreateShader(pd3dDevice,  pd3dGraphicsRootSignature, MRT, pdxgiRtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);
}

void Material::SetShader(Shader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}
void Material::SetTexture(CTexture* pTexture)
{
	if (m_pTexture) {
		m_pTexture->Release();
		delete m_pTexture;
		m_pTexture = nullptr;
	}
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}
void Material::SetTexture(CTexture* pTexture, UINT nTexture)
{
	if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->Release();
	m_ppTextures[nTexture] = pTexture;
	if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->AddRef();
}
void Material::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRoot32BitConstants(3, 4, &m_xmf4Ambient, 0);
	pd3dCommandList->SetGraphicsRoot32BitConstants(3, 4, &m_xmf4Albedo, 4);
	pd3dCommandList->SetGraphicsRoot32BitConstants(3, 4, &m_xmf4Specular, 8);
	pd3dCommandList->SetGraphicsRoot32BitConstants(3, 4, &m_xmf4Emissive, 12);
	
	pd3dCommandList->SetGraphicsRoot32BitConstants(3, 1, &m_nType, 16);

	//if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) m_ppTextures[i]->UpdateShaderVariable(pd3dCommandList,0);
	}
}

void Material::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nType, UINT nRootParameter, _TCHAR* pwstrTextureName, CTexture** ppTexture, Object* pParent, FILE* OpenedFile, Shader* pShader)
{
	char pstrTextureName[64] = { '\0' };

	BYTE nStrLength = ::ReadStringFromFile(OpenedFile, pstrTextureName);

	bool bDuplicated = false;
	if (strcmp(pstrTextureName, "null"))
	{
		SetMaterialType(nType);

		char pstrFilePath[64] = { '\0' };
		strcpy_s(pstrFilePath, 64, "Model/Textures/");

		bDuplicated = (pstrTextureName[0] == '@');
		strcpy_s(pstrFilePath + 15, 64 - 15, (bDuplicated) ? (pstrTextureName + 1) : pstrTextureName);
		strcpy_s(pstrFilePath + 15 + ((bDuplicated) ? (nStrLength - 1) : nStrLength), 64 - 15 - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

		size_t nConverted = 0;
		mbstowcs_s(&nConverted, pwstrTextureName, 64, pstrFilePath, _TRUNCATE);

		//#define _WITH_DISPLAY_TEXTURE_NAME

#ifdef _WITH_DISPLAY_TEXTURE_NAME
		static int nTextures = 0, nRepeatedTextures = 0;
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("Texture Name: %d %c %s\n"), (pstrTextureName[0] == '@') ? nRepeatedTextures++ : nTextures++, (pstrTextureName[0] == '@') ? '@' : ' ', pwstrTextureName);
		OutputDebugString(pstrDebug);
#endif
		if (!bDuplicated)
		{
			*ppTexture = GameScene::MainScene->TextureMap[pstrTextureName];
			//*ppTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0 , nRootParameter);
			//(*ppTexture)->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pwstrTextureName, RESOURCE_TEXTURE2D, 0);
			if (*ppTexture) (*ppTexture)->AddRef();

			//GameScene::CreateShaderResourceViews(pd3dDevice, *ppTexture, nRootParameter, false);
		}
		else
		{
			if (pParent)
			{
				while (pParent)
				{
					if (!pParent->m_pParent) break;
					pParent = pParent->m_pParent;
				}
				//Object* pRootGameObject = pParent;
				//*ppTexture = pRootGameObject->FindReplicatedTexture(pwstrTextureName);
				char extractedString[63];
				std::strcpy(extractedString, pstrTextureName + 1);
				*ppTexture = GameScene::MainScene->TextureMap[extractedString];
				if (*ppTexture) (*ppTexture)->AddRef();
			}
		}
	}
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
	if (m_pKeyFrameTimes) 
		delete[] m_pKeyFrameTimes;
	for (int j = 0; j < m_KeyFrames; j++)
	{
		if (m_ppKeyFrameTransforms[j])
			delete[] m_ppKeyFrameTransforms[j];
	}
	if (m_ppKeyFrameTransforms) 
		delete[] m_ppKeyFrameTransforms;
}

AnimationSets::AnimationSets(int nAnimationSets)
{
	m_nAnimationSets = nAnimationSets;
	m_pAnimationSets = new AnimationSet * [nAnimationSets];
}

AnimationSets::~AnimationSets()
{
	//for (int i = 0; i < m_nAnimationSets; i++) if (m_pAnimationSets[i]) delete m_pAnimationSets[i];
	if (m_pAnimationSets) delete[] m_pAnimationSets;

	if (m_ppAnimatedBoneFrameCaches) delete[] m_ppAnimatedBoneFrameCaches;
}

bool AnimationSet::SetPosition(float fElapsedPosition)
{
	if (m_Position >= m_Length)
	{
		return false;
	}
	switch (m_nType)
	{
	case ANIMATION_TYPE_LOOP:
	{
		m_Position += fElapsedPosition;
		if (m_Position >= m_Length) 
			m_Position = 0.0f;
		//			m_fPosition = fmod(fTrackPosition, m_pfKeyFrameTimes[m_nKeyFrames-1]); // m_fPosition = fTrackPosition - int(fTrackPosition / m_pfKeyFrameTimes[m_nKeyFrames-1]) * m_pfKeyFrameTimes[m_nKeyFrames-1];
		//			m_fPosition = fmod(fTrackPosition, m_fLength); //if (m_fPosition < 0) m_fPosition += m_fLength;
		//			m_fPosition = fTrackPosition - int(fTrackPosition / m_fLength) * m_fLength;
		break;
	}
	case ANIMATION_TYPE_ONCE:
		m_Position += fElapsedPosition;
		if (m_Position >= m_Length) {
			//std::cout << "Animation Length : " << m_Length << " END" << std::endl;
			return true;
		}
		break;
	case ANIMATION_TYPE_PINGPONG:
		break;
	}
	return false;
}

XMFLOAT4X4 AnimationSet::GetSRT(int nBonefloat,float fPosition)
{
	XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Identity();

	for (int i = 0; i < (m_KeyFrames - 1); i++)
	{
		if ((m_pKeyFrameTimes[i] <= fPosition) && (fPosition < m_pKeyFrameTimes[i + 1]))
		{
			float t = (fPosition - m_pKeyFrameTimes[i]) / (m_pKeyFrameTimes[i + 1] - m_pKeyFrameTimes[i]);
			xmf4x4Transform = Matrix4x4::Interpolate(m_ppKeyFrameTransforms[i][nBonefloat], m_ppKeyFrameTransforms[i + 1][nBonefloat], t);
			break;
		}
	}
	if (fPosition >= m_pKeyFrameTimes[m_KeyFrames - 1]) xmf4x4Transform = m_ppKeyFrameTransforms[m_KeyFrames - 1][nBonefloat];


	return(xmf4x4Transform);
}

LoadedModelInfo::~LoadedModelInfo()
{
	if (m_nSkinnedMeshes > 0)
	{
		//for (int i = 0; i < m_nSkinnedMeshes; ++i)
		//{
		//	if (m_ppSkinnedMeshes[i])
		//		m_ppSkinnedMeshes[i]->Release();
		//}
		//delete[] m_ppSkinnedMeshes;
	}
	if (m_pAnimationSets)
	{
		m_pAnimationSets->Release();
		m_pAnimationSets = NULL;
	}
	if (m_pRoot)
	{
		m_pRoot->Release();
		m_pRoot = NULL;
	}
}

void LoadedModelInfo::PrepareSkinning()
{
	int nSkinnedMesh = 0;
	m_ppSkinnedMeshes = new SkinnedMesh * [m_nSkinnedMeshes];
	m_pRoot->FindAndSetSkinnedMesh(m_ppSkinnedMeshes, &nSkinnedMesh);

	for (int i = 0; i < m_nSkinnedMeshes; i++) m_ppSkinnedMeshes[i]->PrepareSkinning(m_pRoot);
}

AnimationController::AnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, LoadedModelInfo* pModel)
{
	m_nAnimationTracks = nAnimationTracks;
	m_pAnimationTracks = new AnimationTrack[nAnimationTracks];

	m_pAnimationSets = pModel->m_pAnimationSets;
	m_pAnimationSets->AddRef();

	m_pModelRootObject = pModel->m_pRoot;

	m_nSkinnedMeshes = pModel->m_nSkinnedMeshes;
	m_ppSkinnedMeshes = new SkinnedMesh * [m_nSkinnedMeshes];
	for (int i = 0; i < m_nSkinnedMeshes; i++) m_ppSkinnedMeshes[i] = pModel->m_ppSkinnedMeshes[i];

	m_ppd3dcbSkinningBoneTransforms = new ID3D12Resource * [m_nSkinnedMeshes];
	m_ppcbxmf4x4MappedSkinningBoneTransforms = new XMFLOAT4X4 * [m_nSkinnedMeshes];

	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256�� ���
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbSkinningBoneTransforms[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Map(0, NULL, (void**)&m_ppcbxmf4x4MappedSkinningBoneTransforms[i]);
	}
}

AnimationController::~AnimationController()
{
	if (m_pAnimationTracks) 
		delete[] m_pAnimationTracks;

	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbSkinningBoneTransforms[i]->Unmap(0, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Release();
	}
	if (m_ppd3dcbSkinningBoneTransforms) 
		delete[] m_ppd3dcbSkinningBoneTransforms;
	if (m_ppcbxmf4x4MappedSkinningBoneTransforms) 
		delete[] m_ppcbxmf4x4MappedSkinningBoneTransforms;

	if (m_pAnimationSets) m_pAnimationSets->Release();

	if (m_ppSkinnedMeshes) delete[] m_ppSkinnedMeshes;
}

void AnimationController::ChangeAnimationUseBlending(int nAnimationSet)
{
	if (m_pAnimationTracks)
	{
		if (m_pAnimationTracks[1].m_nAnimationSet == nAnimationSet)
		{
			return;
		}
		else if (m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[0].m_nAnimationSet]->m_nType == ANIMATION_TYPE_ONCE)
		{
			m_pAnimationTracks[2].m_nAnimationSet = nAnimationSet;
			return;
		}
		else if (m_BlendingWeight < 1.0f)
		{
			m_pAnimationTracks[2].m_nAnimationSet = nAnimationSet;
			return;
		}
		else 
		{
			m_pAnimationTracks[1].m_nAnimationSet = nAnimationSet;
			m_pAnimationTracks[2].m_nAnimationSet = nAnimationSet;
			SetTrackEnable(1, true);
			m_BlendingWeight = 0.0f;
		}
	}
}
void AnimationController::ChangeAnimationWithoutBlending(int nAnimationSet)
{
	SetTrackAnimationSet(0, nAnimationSet);
	SetTrackAnimationSet(1, nAnimationSet);
	SetTrackAnimationSet(2, nAnimationSet);
}

float AnimationTrack::UpdatePosition(float fTrackPosition, float fElapsedTime, float fAnimationLength)
{
	float fTrackElapsedTime = fElapsedTime * m_fSpeed;
	switch (m_nType)
	{
	case ANIMATION_TYPE_LOOP:
	{
		if (m_fPosition < 0.0f) m_fPosition = 0.0f;
		else
		{
			m_fPosition = fTrackPosition + fTrackElapsedTime;
			if (m_fPosition > fAnimationLength)
			{
				m_fPosition = 0.0f;
				return(fAnimationLength);
			}
		}

		break;
	}
	case ANIMATION_TYPE_ONCE:
		m_fPosition = fTrackPosition + fTrackElapsedTime;
		if (m_fPosition > fAnimationLength) m_fPosition = fAnimationLength;
		break;
	case ANIMATION_TYPE_PINGPONG:
		break;
	}

	TriggerEvent(fTrackPosition);

	return(m_fPosition);
}

void AnimationTrack::AddAnimationEvent(std::string EventName, int nAnimationSet, float Position, std::function<void()>Callback)
{
	m_AnimationEvents.emplace_back(AnimationEvent{ EventName, nAnimationSet, Position, Callback });
}
void AnimationTrack::TriggerEvent(float fTrackPosition)
{
	for (auto ev : m_AnimationEvents)
	{
		if (ev.AnimationSet == m_nAnimationSet) {							// �̺�Ʈ�� �ִϸ��̼ǰ� ���� �ִϸ��̼��� ������ ��
			if (ev.Position <= m_fPosition && ev.Position > fTrackPosition)	// �̺�Ʈ ��ġ�� ������ġ���� �۰ų� ���� ���� ��ġ���� Ŭ ��
			{
				ev.Callback();
			}
		}
	}
}
void AnimationController::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pAnimationTracks)
	{
		m_pAnimationTracks[nAnimationTrack].m_nAnimationSet = nAnimationSet;
		m_pAnimationTracks[nAnimationTrack].m_nType = m_pAnimationSets->m_pAnimationSets[nAnimationSet]->m_nType;
		if (m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[nAnimationTrack].m_nAnimationSet]->m_nType == ANIMATION_TYPE_ONCE)
		{
			m_pAnimationTracks[nAnimationTrack].m_fPosition = 0.0f;
			m_pAnimationSets->m_pAnimationSets[nAnimationSet]->m_Position = 0.0f;
		}
	}
}

void AnimationController::SetTrackEnable(int nAnimationTrack, bool bEnable)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetEnable(bEnable);
}

void AnimationController::SetTrackPosition(int nAnimationTrack, float fPosition)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetPosition(fPosition);
}

void AnimationController::SetTrackSpeed(int nAnimationTrack, float fSpeed)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetSpeed(fSpeed);
}

void AnimationController::SetTrackWeight(int nAnimationTrack, float fWeight)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetWeight(fWeight);
}

void AnimationController::EndAnimation()
{
	NoMoreAnimation = true;
}

void AnimationController::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppSkinnedMeshes[i]->m_pd3dcbSkinningBoneTransforms = m_ppd3dcbSkinningBoneTransforms[i];
		m_ppSkinnedMeshes[i]->m_pMappedSkinningBoneTransforms = m_ppcbxmf4x4MappedSkinningBoneTransforms[i];

	}
}

void AnimationController::AdvanceTime(float fTimeElapsed, Object* pRootGameObject)
{
	m_fTime += fTimeElapsed;
	if (m_pAnimationTracks)
	{

		for (int j = 0; j < m_pAnimationSets->m_nAnimatedBoneFrames; j++) m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = Matrix4x4::Zero();

		for (int k = 0; k < m_nAnimationTracks; k++) {
			
			if (m_pAnimationTracks[k].m_bEnable)
			{
				bool IsAnimationEnd = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[k].m_nAnimationSet]->SetPosition(fTimeElapsed * m_pAnimationTracks[k].m_fSpeed);
				if (IsAnimationEnd)
				{
					if (m_pAnimationTracks[0].m_nAnimationSet == m_pAnimationTracks[2].m_nAnimationSet)
					{
						SetTrackAnimationSet(1, 0);
						SetTrackAnimationSet(2, 0);
						if(!NoMoreAnimation)
							SetTrackEnable(1, true);
						m_BlendingWeight = 0.0f;
						break;
					}
					else
					{
						SetTrackAnimationSet(1, m_pAnimationTracks[2].m_nAnimationSet);
						SetTrackAnimationSet(2, m_pAnimationTracks[2].m_nAnimationSet);
						if(!NoMoreAnimation)
							SetTrackEnable(1, true);
						m_BlendingWeight = 0.0f;

					}
				}
			}

		}

		if(!m_pAnimationTracks[1].m_bEnable) {										//�Ϲ� ���
			for (int k = 0; k < m_nAnimationTracks; k++)
			{
				if (m_pAnimationTracks[k].m_bEnable)
				{
					AnimationSet* pAnimationSet = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[k].m_nAnimationSet];
					float fPosition = m_pAnimationTracks[k].UpdatePosition(m_pAnimationTracks[k].m_fPosition, fTimeElapsed, pAnimationSet->m_Length);
					for (int j = 0; j < m_pAnimationSets->m_nAnimatedBoneFrames; j++)
					{
						XMFLOAT4X4 xmf4x4Transform = m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent;
						XMFLOAT4X4 xmf4x4TrackTransform = pAnimationSet->GetSRT(j, fPosition);
						xmf4x4Transform = Matrix4x4::Add(xmf4x4Transform, Matrix4x4::Scale(xmf4x4TrackTransform, m_pAnimationTracks[k].m_fWeight));
						m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = xmf4x4Transform;
					}
				}
			}
		}
		else if(m_pAnimationTracks[0].m_bEnable)															//�����
		{
			m_BlendingWeight += fTimeElapsed*3.5f;
			m_BlendingWeight = max(0.0f, min(1.0f, m_BlendingWeight));
			//std::cout << "Animation A : " << m_pAnimationTracks[0].m_nAnimationSet << ", Animation B : " << m_pAnimationTracks[1].m_nAnimationSet << ", Weight : " << m_BlendingWeight << std::endl;
			if (m_BlendingWeight == 1.0f)
			{
				SetTrackAnimationSet(0, m_pAnimationTracks[1].m_nAnimationSet);

				SetTrackEnable(1, false);

				if (m_pAnimationTracks[1].m_nAnimationSet != m_pAnimationTracks[2].m_nAnimationSet)
				{
					SetTrackAnimationSet(1, m_pAnimationTracks[2].m_nAnimationSet);
					SetTrackEnable(1, true);
					m_BlendingWeight = 0.0f;
				}
			}


			AnimationSet* pCurrentAnimationSet = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[0].m_nAnimationSet];
			AnimationSet* pNextAnimationSet = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[1].m_nAnimationSet];
			float fPosition_Current = m_pAnimationTracks[0].UpdatePosition(m_pAnimationTracks[0].m_fPosition, fTimeElapsed, pCurrentAnimationSet->m_Length);
			float fPosition_Next = m_pAnimationTracks[1].UpdatePosition(m_pAnimationTracks[1].m_fPosition, fTimeElapsed, pNextAnimationSet->m_Length);
			for (int j = 0; j < m_pAnimationSets->m_nAnimatedBoneFrames; j++)
			{
				XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Zero();
				XMFLOAT4X4 CurrentTransform;
				XMFLOAT4X4 NextTransform;
				XMFLOAT4X4 CurrentTrackTransform = pCurrentAnimationSet->GetSRT(j, fPosition_Current);
				XMFLOAT4X4 NextTrackTransform = pNextAnimationSet->GetSRT(j, fPosition_Next);
				CurrentTransform = Matrix4x4::Add(xmf4x4Transform, Matrix4x4::Scale(CurrentTrackTransform, m_pAnimationTracks[0].m_fWeight));
				NextTransform = Matrix4x4::Add(xmf4x4Transform, Matrix4x4::Scale(NextTrackTransform, m_pAnimationTracks[1].m_fWeight));

				xmf4x4Transform = Matrix4x4::Interpolate(CurrentTransform, NextTransform, m_BlendingWeight);
				m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = xmf4x4Transform;
			}
		}
		pRootGameObject->UpdateTransform(NULL);
	}
}

void AnimationController::AddAnimationEvent(std::string EventName, int nAnimationSet, float Position, std::function<void()>Callback)
{
	if (m_pAnimationSets->m_pAnimationSets[nAnimationSet]->m_Length < Position || Position < 0)
	{
		std::cerr << "AddAnimationEventError - " << EventName << "Animation Length Over. " << std::endl;
		return;
	}
	else if (m_pAnimationSets->m_nAnimationSets <= nAnimationSet || nAnimationSet < 0)
	{
		std::cerr << "AddAnimationEventError - " << EventName << "No Animation Exist. " << std::endl;
		return;
	}
	m_pAnimationTracks[0].AddAnimationEvent(EventName, nAnimationSet, Position, Callback);
	m_pAnimationTracks[1].AddAnimationEvent(EventName, nAnimationSet, Position, Callback);
}




Object::Object()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	XMStoreFloat4x4(&m_xmf4x4ToParent, XMMatrixIdentity());
	GameScene::MainScene->creationQueue.push(this);
}
Object::Object(bool Push_List)
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	XMStoreFloat4x4(&m_xmf4x4ToParent, XMMatrixIdentity());
	SetNum(OBJNum++);

	if (Push_List) {
		GameScene::MainScene->creationQueue.push(this);
	}
}
Object::Object(OBJECT_TYPE type)
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	XMStoreFloat4x4(&m_xmf4x4ToParent, XMMatrixIdentity());
	SetNum(OBJNum++);

	switch (type) {
	case DEFAULT_OBJECT:
		GameScene::MainScene->creationQueue.push(this);
		break;
	case BLEND_OBJECT:
		GameScene::MainScene->creationBlendQueue.push(this);
		break;
	case UI_OBJECT:
		GameScene::MainScene->creationUIQueue.push(this);
		break;
	case BOUNDING_OBJECT:
		GameScene::MainScene->creationBoundingQueue.push(this);
		break;
	case FORWARD_OBJECT:
		GameScene::MainScene->creationForwardQueue.push(this);
		break;

	}
}

Object::~Object()
{
	if (m_pSkinnedAnimationController)
		delete m_pSkinnedAnimationController;
	
	if (m_pMesh)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}
	if (m_pMaterial) 
		m_pMaterial->Release();
	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			if (m_ppMaterials[i])
			{
				m_ppMaterials[i]->Release();
				m_ppMaterials[i] = NULL;
			}
		}
	}
	if (!components.empty())
	{
		for (auto component : components)
		{
			delete component;
		}
	}
	if (m_pSibling) 
		m_pSibling->Release();
	if (m_pChild)
		m_pChild->Release();
}

void Object::start()
{
	for (auto component : components)
		component->start();
}

void Object::update()
{
	for (auto component : components)
		component->update();
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

void Object::SetNum(int num)
{
	Num = num;
	if (m_pSibling) m_pSibling->SetNum(num);
	if (m_pChild) m_pChild->SetNum(num);
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
void Object::SetMaterials(int nMaterial, Material* pMaterial)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->Release();
	m_ppMaterials[nMaterial] = pMaterial;
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->AddRef();
}

void Object::ChangeShader(Shader* pShader)
{
	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			if (m_ppMaterials[i])
				m_ppMaterials[i]->SetShader(pShader);
		}
	}
	if (m_pSibling) m_pSibling->ChangeShader(pShader);
	if (m_pChild) m_pChild->ChangeShader(pShader);
}
Mesh* Object::FindFirstMesh()
{
	Mesh* pMesh = GetMesh();
	if (pMesh)
	{
		return pMesh;
	}
	else
	{
		if (m_pSibling)
		{
			return m_pSibling->FindFirstMesh();
		}
		if (m_pChild)
		{
			return m_pChild->FindFirstMesh();
		}
	}
	return NULL;
}
bool Object::IsVisible(Camera* pCamera)
{
	OnPrepareRender();
	bool bIsVisible = false;
	BoundingOrientedBox xmBoundingBox;
	Mesh* pMesh = FindFirstMesh();
	if (pMesh)
	{
		xmBoundingBox = pMesh->GetBoundingBox();
		xmBoundingBox.Transform(xmBoundingBox, XMLoadFloat4x4(&m_xmf4x4World));
		xmBoundingBox.Center.x += pCamera->GetLookVector().x*40;
		xmBoundingBox.Center.y += pCamera->GetLookVector().y*40;
		xmBoundingBox.Center.z += pCamera->GetLookVector().z*40;
		if (pCamera) bIsVisible = pCamera->IsInFrustum(xmBoundingBox);
		return(bIsVisible);
	}
	return false;
}

void Object::SetMesh(Mesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void Object::SetMesh(int i, Mesh* pMesh)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
		m_ppMeshes[i] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
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
	if (GameScene::MainScene->GetTerrain())
	{
		float width = GameScene::MainScene->GetTerrain()->GetWidth();
		float length = GameScene::MainScene->GetTerrain()->GetLength();
		m_xmf4x4ToParent._42 = GameScene::MainScene->GetTerrain()->GetHeight(m_xmf4x4ToParent._41 + (width/2), m_xmf4x4ToParent._43 + (length /2));
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
		
	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

void Object::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	//UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256�� ���
	//m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbGameObjectBytes * m_nObjects, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	//m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);
}

void Object::ReleaseShaderVariables()
{
	if (m_pMaterial)
		m_pMaterial->m_pShader->ReleaseShaderVariables();
	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			m_ppMaterials[i]->m_pShader->ReleaseShaderVariables();
		}
	}
}

void Object::FindAndSetSkinnedMesh(SkinnedMesh** ppSkinnedMeshes, int* pnSkinnedMesh)
{
	if (m_pMesh)
	{
		ppSkinnedMeshes[(*pnSkinnedMesh)++] = (SkinnedMesh*)m_pMesh;
		//m_pMesh->AddRef();
	}

	if (m_pSibling)
		m_pSibling->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
	if (m_pChild)
		m_pChild->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
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

CTexture* Object::FindReplicatedTexture(_TCHAR* pstrTextureName)
{
	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i])
		{
			for (int j = 0; j < m_ppMaterials[i]->m_nTextures; j++)
			{
				if (m_ppMaterials[i]->m_ppTextures[j])
				{
					if (!_tcsncmp(m_ppMaterials[i]->m_ppstrTextureNames[j], pstrTextureName, _tcslen(pstrTextureName))) return(m_ppMaterials[i]->m_ppTextures[j]);
				}
			}
		}
	}
	CTexture* pTexture = NULL;
	if (m_pSibling) if (pTexture = m_pSibling->FindReplicatedTexture(pstrTextureName)) return(pTexture);
	if (m_pChild) if (pTexture = m_pChild->FindReplicatedTexture(pstrTextureName)) return(pTexture);

	return(NULL);
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

void Object::SetDo_Render(bool b)
{
	Do_Render = b;

}

// -------------- �� & �ִϸ��̼� �ε� --------------

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

void Object::LoadMapData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, Shader* boundshader)
{
	char pstrToken[64] = { '\0' };
	CubeMesh* BoundMesh = new CubeMesh(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);
	SphereMesh* SphereMes = new SphereMesh(pd3dDevice, pd3dCommandList, 1.0f, 10, 10);

	int nMesh = 0;
	UINT nReads = 0;
	Object* pObject = NULL;
	FILE* OpenedFile = NULL;
	::fopen_s(&OpenedFile, pstrFileName, "rb");
	::rewind(OpenedFile);
	::ReadStringFromFile(OpenedFile, pstrToken);
	if (!strcmp(pstrToken, "<Objects>:"))
	{
		while (true)
		{
			::ReadStringFromFile(OpenedFile, pstrToken);
			if (!strcmp(pstrToken, "<Mesh>:"))
			{
				BYTE Length = ::ReadStringFromFile(OpenedFile, pstrToken);
				if (pstrToken[0] == '@') // Mesh�̸��� �´� Mesh�� �̹� �ε尡 �Ǿ��ٸ� true -> �ִ� �� ���� ��
				{
					std::string str(pstrToken + 1);

					pObject = new ModelObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, GameScene::MainScene->ModelMap[str]);

				}
				else
				{
					std::string str(pstrToken);
					if (GameScene::MainScene->ModelMap.find(str) != GameScene::MainScene->ModelMap.end())
					{
						pObject = new ModelObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, GameScene::MainScene->ModelMap[str]);
					}
					else
					{
						char pstrFilePath[64] = { '\0' };
						strcpy_s(pstrFilePath, 64, "Model/");
						strcpy_s(pstrFilePath + 6, 64 - 6, pstrToken);
						strcpy_s(pstrFilePath + 6 + Length, 64 - 6 - Length, ".bin");


						LoadedModelInfo* pLoadedModel = LoadAnimationModel(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pstrFilePath, NULL);


						GameScene::MainScene->ModelMap.insert(std::pair<std::string, LoadedModelInfo*>(str, pLoadedModel)); // ���� ���� map�� ����
						if (pLoadedModel->m_pRoot)
							pLoadedModel->m_pRoot->AddRef();
						if (pLoadedModel->m_pAnimationSets)
							pLoadedModel->m_pAnimationSets->AddRef();
						pObject = new ModelObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, GameScene::MainScene->ModelMap[str]);
					}



				}

			}
			if (!strcmp(pstrToken, "<Position>:"))
			{
				BoundBox* bb = new BoundBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, BoundMesh, boundshader);
				BoundSphere* bs = new BoundSphere(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, SphereMes, boundshader);
				bb->SetNum(0);
				bs->SetNum(4);
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



void Object::LoadMapData_Blend(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, Shader* pBlendShader)
{
	char pstrToken[64] = { '\0' };
	CubeMesh* BoundMesh = new CubeMesh(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);
	SphereMesh* SphereMes = new SphereMesh(pd3dDevice, pd3dCommandList, 1.0f, 10, 10);
	int nMesh = 0;
	UINT nReads = 0;
	Object* pObject = NULL;
	FILE* OpenedFile = NULL;
	::fopen_s(&OpenedFile, pstrFileName, "rb");
	::rewind(OpenedFile);
	::ReadStringFromFile(OpenedFile, pstrToken);
	if (!strcmp(pstrToken, "<Objects>:"))
	{
		while (true)
		{
			::ReadStringFromFile(OpenedFile, pstrToken);
			if (!strcmp(pstrToken, "<Mesh>:"))
			{
				BYTE Length = ::ReadStringFromFile(OpenedFile, pstrToken);
				if (pstrToken[0] == '@') // Mesh�̸��� �´� Mesh�� �̹� �ε尡 �Ǿ��ٸ� true -> �ִ� �� ���� ��
				{
					std::string str(pstrToken + 1);
					pObject = new TestModelBlendObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, GameScene::MainScene->ModelMap[str], pBlendShader);

				}
				else
				{
					std::string str(pstrToken);
					char pstrFilePath[64] = { '\0' };
					strcpy_s(pstrFilePath, 64, "Model/");
					strcpy_s(pstrFilePath + 6, 64 - 6, pstrToken);
					strcpy_s(pstrFilePath + 6 + Length, 64 - 6 - Length, ".bin");


					LoadedModelInfo* pLoadedModel = LoadAnimationModel(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pstrFilePath, NULL);

					GameScene::MainScene->ModelMap.insert(std::pair<std::string, LoadedModelInfo*>(str, pLoadedModel)); // ���� ���� map�� ����
					if (pLoadedModel->m_pRoot)
						pLoadedModel->m_pRoot->AddRef();
					if (pLoadedModel->m_pAnimationSets)
						pLoadedModel->m_pAnimationSets->AddRef();

					pObject = new TestModelBlendObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pLoadedModel, pBlendShader);

				}
				
			}
			if (!strcmp(pstrToken, "<Position>:"))
			{
				nReads = (UINT)::fread(&pObject->m_xmf4x4ToParent, sizeof(float), 16, OpenedFile);
			}
			if (!strcmp(pstrToken, "</Objects>"))
			{
				break;
			}
		}
	}
}

void Object::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, Object* pParent, FILE* OpenedFile, Shader* pShader)
{
	char pstrToken[64] = { '\0' };
	int nMaterial = 0;
	UINT nReads = 0;

	m_nMaterials = ::ReadIntegerFromFile(OpenedFile);
	m_ppMaterials = new Material * [m_nMaterials];
	for (int i = 0; i < m_nMaterials; ++i) m_ppMaterials[i] = NULL;

	Material* pMaterial = NULL;
	CTexture* pTexture = NULL;

	while (true)
	{
		::ReadStringFromFile(OpenedFile, pstrToken);
		if (!strcmp(pstrToken, "<Material>:"))
		{
			nMaterial = ReadIntegerFromFile(OpenedFile);

			pMaterial = new Material(7); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal

			if (!pShader)
			{
				UINT nMeshType = GetMeshType();
				if (nMeshType & VERTEXT_NORMAL_TANGENT_TEXTURE)
				{
					if (nMeshType & VERTEXT_BONE_INDEX_WEIGHT)
					{
						pMaterial->SetSkinnedAnimationShader();
					}
					else
					{
						pMaterial->SetStandardShader();
					}
				}
			}
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
			pMaterial->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_ALBEDO_MAP, 8, pMaterial->m_ppstrTextureNames[0], &(pMaterial->m_ppTextures[0]), pParent, OpenedFile, pShader);
		}
		/*else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_SPECULAR_MAP, 9, pMaterial->m_ppstrTextureNames[1], &(pMaterial->m_ppTextures[1]), pParent, OpenedFile, pShader);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_NORMAL_MAP, 10, pMaterial->m_ppstrTextureNames[2], &(pMaterial->m_ppTextures[2]), pParent, OpenedFile, pShader);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_METALLIC_MAP, 11, pMaterial->m_ppstrTextureNames[3], &(pMaterial->m_ppTextures[3]), pParent, OpenedFile, pShader);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_EMISSION_MAP, 12, pMaterial->m_ppstrTextureNames[4], &(pMaterial->m_ppTextures[4]), pParent, OpenedFile, pShader);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_DETAIL_ALBEDO_MAP, 13, pMaterial->m_ppstrTextureNames[5], &(pMaterial->m_ppTextures[5]), pParent, OpenedFile, pShader);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_DETAIL_NORMAL_MAP, 14, pMaterial->m_ppstrTextureNames[6], &(pMaterial->m_ppTextures[6]), pParent, OpenedFile, pShader);
		}*/
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

	Object* pObject = new Object(false);

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
			pObject->CreateShaderVariables(pd3dDevice, pd3dCommandList);
			LoadMesh* pMesh = new LoadMesh(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, OpenedFile);
			pObject->SetMesh(pMesh);
		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{
			if (pnSkinnedMeshes) (*pnSkinnedMeshes)++;
			pObject->CreateShaderVariables(pd3dDevice, pd3dCommandList);
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
			int nChilds = ::ReadIntegerFromFile(OpenedFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					Object* pChild = Object::LoadHierarchy(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pObject, OpenedFile, pShader, pnSkinnedMeshes);
					if (pChild) pObject->SetChild(pChild,true);
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return(pObject);
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
		else if (!strcmp(pstrToken, "</AnimationSets>"))
		{
			break;
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
	::fclose(OpenedFile);
	return pLoadedModel;
}

void Object::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&GetWorld())));
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 1, &Num, 16);
}

UINT Object::GetMeshType()
{
	{ return((m_pMesh) ? m_pMesh->GetType() : 0x00); }
}

void Object::ReleaseUploadBuffers()
{
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��.
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}
void Object::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);
	UpdateTransform(NULL);
}
void Object::Animate(float fTimeElapsed)
{
	OnPrepareRender();

	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->AdvanceTime(fTimeElapsed, this);

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);

}
void Object::OnPrepareRender()
{
}
void Object::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	if (Do_Render && IsVisible(pCamera))
	{
		OnPrepareRender();
		if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);
		if (m_pMesh)
		{
			UpdateShaderVariables(pd3dCommandList);

			if (m_nMaterials > 0)
			{
				for (int i = 0; i < m_nMaterials; ++i)
				{
					if (m_ppMaterials[i])
					{

						if (m_ppMaterials[i]->m_pShader) m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
						m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
					}
					m_pMesh->Render(pd3dCommandList, i);
				}
			}
		}


		if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
		if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
	}
}
void Object::RenderOnlyOneFrame(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{

	OnPrepareRender();
	if (m_pMesh)
	{
		UpdateShaderVariables(pd3dCommandList);

		if (m_nMaterials > 0)
		{
			for (int i = 0; i < m_nMaterials; ++i)
			{
				if (m_ppMaterials[i])
				{

					if (m_ppMaterials[i]->m_pShader) m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
					m_ppMaterials[i]->UpdateShaderVariables(pd3dCommandList);
				}
				m_pMesh->Render(pd3dCommandList, i);
			}
		}
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
	//ī�޶� ��ǥ���� ������ �� ��ǥ��� ��ȯ�Ѵ�.
	*pxmf3PickRayOrigin = Vector3::TransformCoord(xmf3CameraOrigin, xmf4x4Inverse);
	//ī�޶� ��ǥ���� ��(���콺 ��ǥ�� ����ȯ�Ͽ� ���� ��)�� �� ��ǥ��� ��ȯ�Ѵ�.
	*pxmf3PickRayDirection = Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse);
	//������ ���� ���͸� ���Ѵ�.
	*pxmf3PickRayDirection = Vector3::Normalize(Vector3::Subtract(*pxmf3PickRayDirection, *pxmf3PickRayOrigin));
}

int Object::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance)
{
	int nIntersected = 0;
	if (m_pMesh)
	{
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		//�� ��ǥ���� ������ �����Ѵ�.
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, &xmf3PickRayDirection);
		//�� ��ǥ���� ������ �޽��� ������ �˻��Ѵ�.
		nIntersected = m_pMesh->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, pfHitDistance);
	}
	return(nIntersected);
}

ModelObject::ModelObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel) : Object(DEFAULT_OBJECT)
{
	LoadedModelInfo* pLoadedModel = pModel;
	if (pLoadedModel)
	{
		SetChild(pLoadedModel->m_pRoot, true);
	}
}


TestModelBlendObject::TestModelBlendObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel, Shader* pShader) : Object(BLEND_OBJECT)
{
	LoadedModelInfo* pLoadedModel = pModel;
	if (pLoadedModel)
	{
		SetChild(pLoadedModel->m_pRoot, true);
	}
	ChangeShader(pShader);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkyBox::SkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : Object(false)
{
}

SkyBox::~SkyBox()
{
}

void SkyBox::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	OnPrepareRender();

	UpdateShaderVariables(pd3dCommandList);

	if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
	if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

	if (m_pMesh)
	{
		m_pMesh->Render(pd3dCommandList, 0);
	}

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DaySkyBox::DaySkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : SkyBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	SkyBoxMesh* pSkyBoxMesh = new SkyBoxMesh(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);
	SetMesh(pSkyBoxMesh);

	CTexture* pSkyBoxTexture = new CTexture(1, RESOURCE_TEXTURE_CUBE, 0, 1);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"SkyBox/Day_Sky.dds", RESOURCE_TEXTURE_CUBE, 0);

	SkyBoxShader* pSkyBoxShader = new SkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature,1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 17, false);

	Material* pSkyBoxMaterial = new Material();
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	SetMaterial(pSkyBoxMaterial);
}

DaySkyBox::~DaySkyBox()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NightSkyBox::NightSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : SkyBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	SkyBoxMesh* pSkyBoxMesh = new SkyBoxMesh(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);
	SetMesh(pSkyBoxMesh);

	CTexture* pSkyBoxTexture = new CTexture(1, RESOURCE_TEXTURE_CUBE, 0, 1);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"SkyBox/Night_Sky.dds", RESOURCE_TEXTURE_CUBE, 0);

	SkyBoxShader* pSkyBoxShader = new SkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 17, false);

	Material* pSkyBoxMaterial = new Material();
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	SetMaterial(pSkyBoxMaterial);
}

NightSkyBox::~NightSkyBox()
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SunsetSkyBox::SunsetSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : SkyBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	SkyBoxMesh* pSkyBoxMesh = new SkyBoxMesh(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 1.0f);
	SetMesh(pSkyBoxMesh);

	CTexture* pSkyBoxTexture = new CTexture(1, RESOURCE_TEXTURE_CUBE, 0, 1);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"SkyBox/Sunset_Sky.dds", RESOURCE_TEXTURE_CUBE, 0);

	SkyBoxShader* pSkyBoxShader = new SkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 17, false);

	Material* pSkyBoxMaterial = new Material();
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	SetMaterial(pSkyBoxMaterial);
}

SunsetSkyBox::~SunsetSkyBox()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


HeightMapTerrain::HeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int BlockWidth, int BlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : Object(DEFAULT_OBJECT)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new HeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	
#ifdef TESSELLATION
	int cxQuadsPerBlock = BlockWidth - 1;
	int czQuadsPerBlock = BlockLength - 1;

	long cxBlocks = (nWidth - 1) / cxQuadsPerBlock;
	long czBlocks = (nLength - 1) / czQuadsPerBlock;

	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes = new Mesh * [m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)	m_ppMeshes[i] = NULL;
	HeightMapGridMeshTess* pHeightMapGridMesh = NULL;
	for (int z = 0, zStart = 0; z < czBlocks; z++)
	{
		for (int x = 0, xStart = 0; x < cxBlocks; x++)
		{
			xStart = x * (BlockWidth - 1);
			zStart = z * (BlockLength - 1);
			pHeightMapGridMesh = new HeightMapGridMeshTess(pd3dDevice, pd3dCommandList, xStart, zStart, BlockWidth, BlockLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
			SetMesh(x + (z * cxBlocks), pHeightMapGridMesh);
		}
	}
	TessTerrainShader* pTerrainShader = new TessTerrainShader();
	
#else
	HeightMapGridMesh* pMesh = new HeightMapGridMesh(pd3dDevice, pd3dCommandList, 0, 0, nWidth, nLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
	SetMesh(pMesh);
	TerrainShader* pTerrainShader = new TerrainShader();

#endif	

	DXGI_FORMAT pdxgiRtvFormats[MRT] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,  DXGI_FORMAT_R8G8B8A8_UNORM };
	pTerrainShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, MRT, pdxgiRtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);

	CTexture* Terrain_Texture = new CTexture(10, RESOURCE_TEXTURE2D, 0, 1);

	Terrain_Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Terrain/TFF_Terrain_Dirt_1A_D.dds", RESOURCE_TEXTURE2D, 0);
	Terrain_Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Terrain/TFF_Terrain_Dirt_Road_1A_D.dds", RESOURCE_TEXTURE2D, 1);
	Terrain_Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Terrain/TFF_Terrain_Earth_1A_D.dds", RESOURCE_TEXTURE2D, 2);
	Terrain_Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Terrain/TFF_Terrain_Earth_2A_D.dds", RESOURCE_TEXTURE2D, 3);
	Terrain_Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Terrain/TFF_Terrain_Earth_3A_D.dds", RESOURCE_TEXTURE2D, 4);
	Terrain_Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Terrain/TFF_Terrain_Grass_1A_D.dds", RESOURCE_TEXTURE2D, 5);
	Terrain_Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Terrain/TFF_Terrain_Grass_2A_D.dds", RESOURCE_TEXTURE2D, 6);
	Terrain_Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Terrain/TFF_Terrain_Sand_1A_D.dds", RESOURCE_TEXTURE2D, 7);
	Terrain_Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Terrain/TestLevel_splatmap_0.dds", RESOURCE_TEXTURE2D, 8);
	Terrain_Texture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Terrain/TestLevel_splatmap_1.dds", RESOURCE_TEXTURE2D, 9);


	GameScene::CreateShaderResourceViews(pd3dDevice, Terrain_Texture, 18, false);

	m_nMaterials = 1;
	m_ppMaterials = new Material * [1];
	m_ppMaterials[0] = NULL;
	Material* pTerrainMaterial = new Material(1);
	pTerrainMaterial->SetTexture(Terrain_Texture, 0);
	pTerrainMaterial->SetShader(pTerrainShader);


	SetMaterials(0, pTerrainMaterial);
}

HeightMapTerrain::~HeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

float HeightMapTerrain::GetHeight(float x, float z, bool bReverseQuad) 
{ 
	return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad) * m_xmf3Scale.y); 
}
XMFLOAT3 HeightMapTerrain::GetNormal(float x, float z) {
	return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); 
}

int HeightMapTerrain::GetHeightMapWidth() { 
	return(m_pHeightMapImage->GetHeightMapWidth()); 
}
int HeightMapTerrain::GetHeightMapLength() {
	return(m_pHeightMapImage->GetHeightMapLength()); 
}

void HeightMapTerrain::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	Object::Render(pd3dCommandList, pCamera);
}
bool HeightMapTerrain::IsVisible(Camera* pCamera)
{
	return true;
}

/////////////////////////////////////////////////////////////////////////////

BoundBox::BoundBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CubeMesh* BoundMesh, Shader* pBoundingShader) : Object(BOUNDING_OBJECT)
{
	SetMesh(BoundMesh);
	Material* pBoundingMaterial = new Material();
	pBoundingMaterial->SetShader(pBoundingShader);

	SetMaterial(pBoundingMaterial);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

BoundBox::~BoundBox()
{
}

void BoundBox::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	OnPrepareRender();
	UpdateShaderVariables(pd3dCommandList);

	if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
	if (m_pMesh)
	{
		m_pMesh->Render(pd3dCommandList, 0);
	}
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


BoundSphere::BoundSphere(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, SphereMesh* SphereMesh, Shader* pBoundingShader) : Object(BOUNDING_OBJECT)
{
	SetMesh(SphereMesh);

	Material* pBoundingMaterial = new Material();
	pBoundingMaterial->SetShader(pBoundingShader);

	SetMaterial(pBoundingMaterial);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void BoundSphere::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	OnPrepareRender();
	UpdateShaderVariables(pd3dCommandList);

	if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
	if (m_pMesh)
	{
		m_pMesh->Render(pd3dCommandList, 0);
	}
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

FireBall::FireBall(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : Object(FORWARD_OBJECT)
{
	ParticleMesh* pMesh = new ParticleMesh(pd3dDevice, pd3dCommandList, 1);
	SetMesh(pMesh);

	CTexture* pParticleTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pParticleTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Particle/Fire_Particle.dds", RESOURCE_TEXTURE2D, 0);

	ParticleShader* pShader = new ParticleShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pParticleTexture, 20, false);

	Material* pMaterial = new Material();
	pMaterial->SetTexture(pParticleTexture);
	pMaterial->SetShader(pShader);

	m_pBoundingShader = new BoundingShader();
	m_pBoundingShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	explode = new Explosion(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	SphereMesh* SphereMes = new SphereMesh(pd3dDevice, pd3dCommandList, 1.0f, 10, 10);
	BoundSphere* bs = new BoundSphere(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, SphereMes, m_pBoundingShader);
	bs->SetNum(1);
	AddComponent<SphereCollideComponent>();
	GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
	GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.0, 0.0), 0.1);

	SetMaterial(pMaterial);
	SetNum(0);
}

FireBall::~FireBall() {
	/*if (m_pBoundingShader) {
		m_pBoundingShader->ReleaseShaderVariables();
		m_pBoundingShader->Release();
	}*/
}

void FireBall::OnPrepareRender()
{
	SetPosition(GetPosition().x + Direction.x, GetPosition().y + Direction.y, GetPosition().z + Direction.z);
	GetComponent<SphereCollideComponent>()->update();
	for (auto& o : GameScene::MainScene->gameObjects)
	{
		if (o->GetComponent<BoxCollideComponent>())
		{
			if (GetComponent<SphereCollideComponent>()->GetBoundingObject()->Intersects(*o->GetComponent<BoxCollideComponent>()->GetBoundingObject()))
			{
				Sound* s = new Sound("Sound/Mage_Blast.mp3", false);
				GameScene::MainScene->AddSound(s);
				explode->Active = true;
				explode->SetPosition(GetPosition());
				Active = false;
				break;
			}
		}
	}
	for (auto& o : GameScene::MainScene->MonsterObjects)
	{
		if (o->GetRemainHP() <= 0.0f)
			continue;
		if (o->GetComponent<SphereCollideComponent>())
		{
			if (GetComponent<SphereCollideComponent>()->GetBoundingObject()->Intersects(*o->GetComponent<SphereCollideComponent>()->GetBoundingObject()))
			{
				Sound* s = new Sound("Sound/Mage_Blast.mp3", false);
				GameScene::MainScene->AddSound(s);
				explode->Active = true;
				explode->SetPosition(GetPosition());
				Active = false;
				if (ownerID == NetworkMGR::id) {
					if (NetworkMGR::b_isNet) {
						CS_TEMP_HIT_MONSTER_PACKET send_packet;
						send_packet.size = sizeof(CS_TEMP_HIT_MONSTER_PACKET);
						send_packet.type = E_PACKET::E_PACKET_CS_TEMP_HIT_MONSTER_PACKET;
						send_packet.monster_id = o->GetNum();
						send_packet.hit_damage = GameFramework::MainGameFramework->m_pPlayer->GetAttack() * (o->GetDefense() / (o->GetDefense() + 100));
						PacketQueue::AddSendPacket(&send_packet);
					}
					else {
						o->GetHit(GameFramework::MainGameFramework->m_pPlayer->GetAttack() * (o->GetDefense() / (o->GetDefense() + 100)));
						if (dynamic_cast<Shield*>(o))
						{

						}
						else
						{
							o->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_M_HIT);
						}
						o->HitSound();
					}
				}
				break;
			}
		}
	}
	
}

void FireBall::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	if (Active)
	{
		OnPrepareRender();

		UpdateShaderVariables(pd3dCommandList);

		if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
		if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

		if (m_pMesh)
		{
			m_pMesh->Render(pd3dCommandList, 0);
		}
	}
}

void FireBall::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	Object::UpdateShaderVariables(pd3dCommandList);
	pd3dCommandList->SetGraphicsRoot32BitConstants(21, 3, &Direction, 0);
}


Explosion::Explosion(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : Object(FORWARD_OBJECT)
{
	ParticleMesh* pMesh = new ParticleMesh(pd3dDevice, pd3dCommandList, 200);
	SetMesh(pMesh);

	CTexture* pParticleTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pParticleTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Particle/Fire_Particle.dds", RESOURCE_TEXTURE2D, 0);

	ParticleShader* pShader = new ParticleShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pParticleTexture, 20, false);

	Material* pMaterial = new Material();
	pMaterial->SetTexture(pParticleTexture);
	pMaterial->SetShader(pShader);

	m_pBoundingShader = new BoundingShader();
	m_pBoundingShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	Active = false;

	SphereMesh* SphereMes = new SphereMesh(pd3dDevice, pd3dCommandList, 1.0f, 10, 10);
	BoundSphere* bs = new BoundSphere(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, SphereMes, m_pBoundingShader);
	bs->SetNum(1);
	AddComponent<SphereCollideComponent>();
	GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
	GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.0, 0.0), 0.0);
	GetComponent<SphereCollideComponent>()->GetBoundingObject()->SetNum(6);

	SetMaterial(pMaterial);
	SetNum(1);
}

void Explosion::OnPrepareRender()
{
	GetComponent<SphereCollideComponent>()->Radius += 0.05;
	
	for (auto& o : GameScene::MainScene->MonsterObjects)
	{
		if (o->GetRemainHP() <= 0.0f)
			continue;
		if (o->GetComponent<SphereCollideComponent>())
		{
			if (GetComponent<SphereCollideComponent>()->GetBoundingObject()->Intersects(*o->GetComponent<SphereCollideComponent>()->GetBoundingObject()))
			{
				if (!o->MageDamage)
				{
					if (ownerID == NetworkMGR::id) {
						if (NetworkMGR::b_isNet) {
							CS_TEMP_HIT_MONSTER_PACKET send_packet;
							send_packet.size = sizeof(CS_TEMP_HIT_MONSTER_PACKET);
							send_packet.type = E_PACKET::E_PACKET_CS_TEMP_HIT_MONSTER_PACKET;
							send_packet.monster_id = o->GetNum();
							send_packet.hit_damage = GameFramework::MainGameFramework->m_pPlayer->GetAttack() * (o->GetDefense() / (o->GetDefense() + 100));
							PacketQueue::AddSendPacket(&send_packet);
						}
						else {
							o->GetHit(GameFramework::MainGameFramework->m_pPlayer->GetAttack() * (o->GetDefense() / (o->GetDefense() + 100)));
						}
						o->MageDamage = true;
					}
				}

			}
		}
	}
	if (GetComponent<SphereCollideComponent>()->Radius >= 2.5)
	{
		GetComponent<SphereCollideComponent>()->Radius = 0;
		Active = false;
		for (auto& o : GameScene::MainScene->MonsterObjects)
		{
			o->MageDamage = false;
		}
	}
}

void Explosion::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	if (Active)
	{
		OnPrepareRender();

		UpdateShaderVariables(pd3dCommandList);

		if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
		if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

		if (m_pMesh)
		{
			m_pMesh->Render(pd3dCommandList, 0);
		}
		GetComponent<SphereCollideComponent>()->GetBoundingObject()->Render(pd3dCommandList, pCamera);
	}
}

void Explosion::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	float time = Timer::GetTotalTime();
	XMFLOAT3 dir;
	Object::UpdateShaderVariables(pd3dCommandList);
	pd3dCommandList->SetGraphicsRoot32BitConstants(21, 3, &dir, 0);
	pd3dCommandList->SetGraphicsRoot32BitConstants(22, 1, &time, 0);
}

/////////////////////////////////////////////////////////////////////////

Item::Item(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, int itemnum) : Object(BLEND_OBJECT)
{
	ParticleMesh* pMesh = new ParticleMesh(pd3dDevice, pd3dCommandList, 1);
	SetMesh(pMesh);

	CTexture* pParticleTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	ItemID = itemnum;
	if(ItemID == 0) // ATK
		pParticleTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/ATK.dds", RESOURCE_TEXTURE2D, 0);
	else if (ItemID == 1) // DEF
		pParticleTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/DEFend.dds", RESOURCE_TEXTURE2D, 0);
	else if (ItemID == 2) // HP
		pParticleTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/HPPlus.dds", RESOURCE_TEXTURE2D, 0);

	IconShader* pShader = new IconShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pParticleTexture, 20, false);

	Material* pMaterial = new Material();
	pMaterial->SetTexture(pParticleTexture);
	pMaterial->SetShader(pShader);

	SetMaterial(pMaterial);

	m_pBoundingShader = new BoundingShader();
	m_pBoundingShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);

	SphereMesh* SphereMes = new SphereMesh(pd3dDevice, pd3dCommandList, 1.0f, 10, 10);
	BoundSphere* bs = new BoundSphere(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, SphereMes, m_pBoundingShader);
	bs->SetNum(1);
	AddComponent<SphereCollideComponent>();
	GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
	GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.0, 0.0), 0.4);
	GetComponent<SphereCollideComponent>()->GetBoundingObject()->SetNum(6);

	SetNum(2);
}

void Item::OnPrepareRender()
{
	if (GetComponent<SphereCollideComponent>()->GetBoundingObject()->Intersects(*GameFramework::MainGameFramework->m_pPlayer->GetComponent<SphereCollideComponent>()->GetBoundingObject()))
	{
		if (NetworkMGR::b_isNet) {
			
		}
		else {
			ItemEffect();
		}
	}
}

void Item::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	OnPrepareRender();

	UpdateShaderVariables(pd3dCommandList);

	if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
	if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

	if (m_pMesh)
	{
		m_pMesh->Render(pd3dCommandList, 0);
	}
}

void Item::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	float time = Timer::GetTotalTime();
	XMFLOAT3 dir;
	Object::UpdateShaderVariables(pd3dCommandList);
	pd3dCommandList->SetGraphicsRoot32BitConstants(21, 3, &dir, 0);
	pd3dCommandList->SetGraphicsRoot32BitConstants(22, 1, &time, 0);
}

void Item::ItemEffect()
{
	if (ItemID == 0) // ATK
	{
		
	}

	else if (ItemID == 1) // DEF
	{
		
	}

	else if (ItemID == 2) // HP
	{
		if (GameFramework::MainGameFramework->m_pPlayer->m_Health < 2000)
		{
			GameFramework::MainGameFramework->m_pPlayer->m_Health += 100;
			GameFramework::MainGameFramework->m_pPlayer->m_RemainHP += 100;
			GameFramework::MainGameFramework->m_pPlayer->m_pHP_Dec_UI->Dec_HP = GameFramework::MainGameFramework->m_pPlayer->m_RemainHP;
			GameFramework::MainGameFramework->m_pPlayer->m_pOverHP_Dec_UI->Dec_HP = GameFramework::MainGameFramework->m_pPlayer->m_RemainHP;
			GameFramework::MainGameFramework->m_pPlayer->m_pHP_Dec_UI->HP = GameFramework::MainGameFramework->m_pPlayer->m_RemainHP;
			GameFramework::MainGameFramework->m_pPlayer->m_pOverHP_Dec_UI->HP = GameFramework::MainGameFramework->m_pPlayer->m_RemainHP;
		}
	}
}

/// /////////////////////////////////////////////////////////////////////////////


Water::Water(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float h, float w) : Object(FORWARD_OBJECT)
{
	RectMesh* pWaterMesh = new RectMesh(pd3dDevice, pd3dCommandList, h, w);
	SetMesh(pWaterMesh);

	CTexture* pWaterTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pWaterTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Model/Textures/Water.dds", RESOURCE_TEXTURE2D, 0);

	WaterShader* pWaterShader = new WaterShader();
	pWaterShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	pWaterShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	GameScene::CreateShaderResourceViews(pd3dDevice, pWaterTexture, 19, false);

	//m_nMaterials = 1;
	//m_ppMaterials = new Material * [1];
	//m_ppMaterials[0] = NULL;
	Material* pWaterMaterial = new Material(1);
	pWaterMaterial->SetTexture(pWaterTexture);
	pWaterMaterial->SetShader(pWaterShader);

	SetMaterial(pWaterMaterial);
}
Water::~Water()
{

}

void Water::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	//Object::Render(pd3dCommandList, pCamera);
	OnPrepareRender();

	UpdateShaderVariables(pd3dCommandList);

	if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
	if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

	if (m_pMesh)
	{
		m_pMesh->Render(pd3dCommandList, 0);
	}
}