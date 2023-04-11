#include "Object.h"
#include "Shader.h"
#include "GameScene.h"
#include "stdafx.h"

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
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
		delete[] m_ppd3dTextures;
	}
	if (m_pnResourceTypes) delete[] m_pnResourceTypes;
	if (m_pdxgiBufferFormats) delete[] m_pdxgiBufferFormats;
	if (m_pnBufferElements) delete[] m_pnBufferElements;

	if (m_pnRootParameterIndices) delete[] m_pnRootParameterIndices;
	if (m_pd3dSrvGpuDescriptorHandles) delete[] m_pd3dSrvGpuDescriptorHandles;

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
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
	if (m_pTexture) m_pTexture->Release();
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
			*ppTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0 , nRootParameter);
			(*ppTexture)->LoadTextureFromFile(pd3dDevice, pd3dCommandList, pwstrTextureName, RESOURCE_TEXTURE2D, 0);
			if (*ppTexture) (*ppTexture)->AddRef();

			GameScene::CreateShaderResourceViews(pd3dDevice, *ppTexture, nRootParameter, false);
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
				Object* pRootGameObject = pParent;
				*ppTexture = pRootGameObject->FindReplicatedTexture(pwstrTextureName);
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
		if (m_Position >= m_Length) m_Position = 0.0f;
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

XMFLOAT4X4 AnimationSet::GetSRT(int nBone)
{
	XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Identity();
#ifdef _WITH_ANIMATION_SRT
	XMVECTOR S, R, T;
	for (int i = 0; i < (m_nKeyFrameTranslations - 1); i++)
	{
		if ((m_pfKeyFrameTranslationTimes[i] <= m_fPosition) && (m_fPosition <= m_pfKeyFrameTranslationTimes[i + 1]))
		{
			float t = (m_fPosition - m_pfKeyFrameTranslationTimes[i]) / (m_pfKeyFrameTranslationTimes[i + 1] - m_pfKeyFrameTranslationTimes[i]);
			T = XMVectorLerp(XMLoadFloat3(&m_ppxmf3KeyFrameTranslations[i][nBone]), XMLoadFloat3(&m_ppxmf3KeyFrameTranslations[i + 1][nBone]), t);
			break;
		}
	}
	for (UINT i = 0; i < (m_nKeyFrameScales - 1); i++)
	{
		if ((m_pfKeyFrameScaleTimes[i] <= m_fPosition) && (m_fPosition <= m_pfKeyFrameScaleTimes[i + 1]))
		{
			float t = (m_fPosition - m_pfKeyFrameScaleTimes[i]) / (m_pfKeyFrameScaleTimes[i + 1] - m_pfKeyFrameScaleTimes[i]);
			S = XMVectorLerp(XMLoadFloat3(&m_ppxmf3KeyFrameScales[i][nBone]), XMLoadFloat3(&m_ppxmf3KeyFrameScales[i + 1][nBone]), t);
			break;
		}
	}
	for (UINT i = 0; i < (m_nKeyFrameRotations - 1); i++)
	{
		if ((m_pfKeyFrameRotationTimes[i] <= m_fPosition) && (m_fPosition <= m_pfKeyFrameRotationTimes[i + 1]))
		{
			float t = (m_fPosition - m_pfKeyFrameRotationTimes[i]) / (m_pfKeyFrameRotationTimes[i + 1] - m_pfKeyFrameRotationTimes[i]);
			R = XMQuaternionSlerp(XMLoadFloat4(&m_ppxmf4KeyFrameRotations[i][nBone]), XMLoadFloat4(&m_ppxmf4KeyFrameRotations[i + 1][nBone]), t);
			break;
		}
	}

	XMStoreFloat4x4(&xmf4x4Transform, XMMatrixAffineTransformation(S, NULL, R, T));
#else   
	for (int i = 0; i < (m_KeyFrames - 1); i++)
	{
		if ((m_pKeyFrameTimes[i] <= m_Position) && (m_Position < m_pKeyFrameTimes[i + 1]))
		{
			float t = (m_Position - m_pKeyFrameTimes[i]) / (m_pKeyFrameTimes[i + 1] - m_pKeyFrameTimes[i]);
			xmf4x4Transform = Matrix4x4::Interpolate(m_ppKeyFrameTransforms[i][nBone], m_ppKeyFrameTransforms[i + 1][nBone], t);
			break;
		}
	}
	if (m_Position >= m_pKeyFrameTimes[m_KeyFrames - 1]) xmf4x4Transform = m_ppKeyFrameTransforms[m_KeyFrames - 1][nBone];

#endif
	return(xmf4x4Transform);
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

	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbSkinningBoneTransforms[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Map(0, NULL, (void**)&m_ppcbxmf4x4MappedSkinningBoneTransforms[i]);
	}
}

AnimationController::~AnimationController()
{
	if (m_pAnimationTracks) delete[] m_pAnimationTracks;

	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbSkinningBoneTransforms[i]->Unmap(0, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Release();
	}
	if (m_ppd3dcbSkinningBoneTransforms) delete[] m_ppd3dcbSkinningBoneTransforms;
	if (m_ppcbxmf4x4MappedSkinningBoneTransforms) delete[] m_ppcbxmf4x4MappedSkinningBoneTransforms;

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

void AnimationController::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pAnimationTracks)
	{
		m_pAnimationTracks[nAnimationTrack].m_nAnimationSet = nAnimationSet;
		m_pAnimationSets->m_pAnimationSets[nAnimationSet]->m_Position = 0.0f;
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
		//		for (int k = 0; k < m_nAnimationTracks; k++) m_pAnimationTracks[k].m_fPosition += (fTimeElapsed * m_pAnimationTracks[k].m_fSpeed);
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
						SetTrackEnable(1, true);
						m_BlendingWeight = 0.0f;
						break;
					}
					else
					{
						SetTrackAnimationSet(1, m_pAnimationTracks[2].m_nAnimationSet);
						SetTrackAnimationSet(2, m_pAnimationTracks[2].m_nAnimationSet);
						SetTrackEnable(1, true);
						m_BlendingWeight = 0.0f;

					}
				}
			}

		}

		if (!m_pAnimationTracks[1].m_bEnable) {										//일반 재생
			for (int j = 0; j < m_pAnimationSets->m_nAnimatedBoneFrames; j++)
			{
				XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Zero();
				for (int k = 0; k < m_nAnimationTracks; k++)
				{
					if (m_pAnimationTracks[k].m_bEnable)
					{
						AnimationSet* pAnimationSet = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[k].m_nAnimationSet];
						XMFLOAT4X4 xmf4x4TrackTransform = pAnimationSet->GetSRT(j);
						xmf4x4Transform = Matrix4x4::Add(xmf4x4Transform, Matrix4x4::Scale(xmf4x4TrackTransform, m_pAnimationTracks[k].m_fWeight));
					}
				}
				m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = xmf4x4Transform;
			}
		}
		else																		//블렌딩
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
					m_pAnimationTracks[1].m_nAnimationSet = m_pAnimationTracks[2].m_nAnimationSet;
					SetTrackEnable(1, true);
					m_BlendingWeight = 0.0f;
				}
			}

			for (int j = 0; j < m_pAnimationSets->m_nAnimatedBoneFrames; j++)
			{
				XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Zero();

				AnimationSet* pCurrentAnimationSet = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[0].m_nAnimationSet];
				XMFLOAT4X4 CurrentTrackTransform = pCurrentAnimationSet->GetSRT(j);

				AnimationSet* pNextAnimationSet = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[1].m_nAnimationSet];
				XMFLOAT4X4 NextTrackTransform = pNextAnimationSet->GetSRT(j);

				xmf4x4Transform = Matrix4x4::Interpolate(CurrentTrackTransform, NextTrackTransform, m_BlendingWeight);
				m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = xmf4x4Transform;
			}
		}
		pRootGameObject->UpdateTransform(NULL);
	}
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
	if (Push_List) {
		GameScene::MainScene->creationQueue.push(this);
	}
}
Object::Object(bool Push_List, bool isBlendObject)
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	XMStoreFloat4x4(&m_xmf4x4ToParent, XMMatrixIdentity());
	if (isBlendObject)
	{
		if (Push_List) {
			GameScene::MainScene->creationBlendQueue.push(this);
		}
	}
	else 
	{
		if (Push_List) {
			GameScene::MainScene->creationQueue.push(this);
		}
	}
	
}
Object::~Object()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pMaterial) m_pMaterial->Release();
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
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수

	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);
}



void Object::ReleaseShaderVariables()
{
}

void Object::FindAndSetSkinnedMesh(SkinnedMesh** ppSkinnedMeshes, int* pnSkinnedMesh)
{
	if (m_pMesh)
		ppSkinnedMeshes[(*pnSkinnedMesh)++] = (SkinnedMesh*)m_pMesh;

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

void Object::LoadMapData(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName)
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
					pObject = new ModelObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, ModelMap[str]);

				}
				else
				{
					std::string str(pstrToken);
					char pstrFilePath[64] = { '\0' };
					strcpy_s(pstrFilePath, 64, "Model/");
					strcpy_s(pstrFilePath + 6, 64 - 6, pstrToken);
					strcpy_s(pstrFilePath + 6 + Length, 64 - 6 - Length, ".bin");


					LoadedModelInfo* pLoadedModel = LoadAnimationModel(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pstrFilePath, NULL);

					ModelMap.insert(std::pair<std::string, LoadedModelInfo*>(str, pLoadedModel)); // 읽은 모델은 map에 저장

			
					pObject = new ModelObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pLoadedModel);

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



void Object::LoadMapData_Blend(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, Shader* pBlendShader)
{
	char pstrToken[64] = { '\0' };
	int nMesh = 0;
	UINT nReads = 0;
	Object* pObject = NULL;
	FILE* OpenedFile = NULL;
	::fopen_s(&OpenedFile, pstrFileName, "rb");
	::rewind(OpenedFile);
	std::map<std::string, LoadedModelInfo*> ModelMap;
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
					pObject = new TestModelBlendObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, ModelMap[str], pBlendShader);

				}
				else
				{
					std::string str(pstrToken);
					char pstrFilePath[64] = { '\0' };
					strcpy_s(pstrFilePath, 64, "Model/");
					strcpy_s(pstrFilePath + 6, 64 - 6, pstrToken);
					strcpy_s(pstrFilePath + 6 + Length, 64 - 6 - Length, ".bin");


					LoadedModelInfo* pLoadedModel = LoadAnimationModel(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pstrFilePath, NULL);

					ModelMap.insert(std::pair<std::string, LoadedModelInfo*>(str, pLoadedModel)); // 읽은 모델은 map에 저장


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
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
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
					if (pChild) pObject->SetChild(pChild);
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

	return pLoadedModel;
}

void Object::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//XMFLOAT4X4 xmf4x4World;
	//D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pd3dcbGameObjects->GetGPUVirtualAddress();
	//
	//XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&GetWorld())));
	//CB_GAMEOBJECT_INFO* pbMappedcbGameObject = (CB_GAMEOBJECT_INFO*)((UINT8*)m_pcbMappedGameObjects);
	//::memcpy(&pbMappedcbGameObject->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
	//pbMappedcbGameObject->m_nObjectID = Num;
	//
	//pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_OBJECT, d3dcbGameObjectGpuVirtualAddress);

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
	if (IsVisible(pCamera))
	{
		OnPrepareRender();
		if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);
		if (m_pMesh)
		{
			UpdateShaderVariables(pd3dCommandList);
			//if (m_pMaterial)
			//{
			//	if (m_pMaterial->m_pShader)
			//	{
			//		m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			//		m_pMaterial->m_pShader->UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
			//	}
			//
			//	m_pMesh->Render(pd3dCommandList);
			//}
			
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
	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);

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
	*pxmf3PickRayDirection = Vector3::TransformCoord(xmf3PickPosition, xmf4x4Inverse);
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

ModelObject::ModelObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel) : Object(true, false)
{
	LoadedModelInfo* pLoadedModel = pModel;
	if (pLoadedModel)
	{
		SetChild(pLoadedModel->m_pRoot, true);
	}
}


TestModelBlendObject::TestModelBlendObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LoadedModelInfo* pModel, Shader* pShader) : ModelObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pModel)
{
	ChangeShader(pShader);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkyBox::SkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : Object(false)
{
	SkyBoxMesh* pSkyBoxMesh = new SkyBoxMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 20.0f);
	SetMesh(pSkyBoxMesh);

	CTexture* pSkyBoxTexture = new CTexture(1, RESOURCE_TEXTURE_CUBE, 0, 1);
	pSkyBoxTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"SkyBox/SkyBox_1.dds", RESOURCE_TEXTURE_CUBE, 0);

	SkyBoxShader* pSkyBoxShader = new SkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature,1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 17, false);

	Material* pSkyBoxMaterial = new Material();
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	SetMaterial(pSkyBoxMaterial);
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
	if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList,0);

	if (m_pMesh)
	{
		m_pMesh->Render(pd3dCommandList, 0);
	}

}

////////////////////////////////////////////////////////////////////////////////////////

UI::UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : Object(false)
{
}

UI::~UI()
{
}

void UI::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_PLAYER_INFO) + 255) & ~255); //256의 배수

	m_pd3dcbUI = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbUI->Map(0, NULL, (void**)&m_pcbMappedUI);
}

void UI::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbUIGpuVirtualAddress = m_pd3dcbUI->GetGPUVirtualAddress();
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&GetWorld())));
	CB_PLAYER_INFO* pbMappedcbUI = (CB_PLAYER_INFO*)((UINT8*)m_pcbMappedUI);
	::memcpy(&pbMappedcbUI->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));

	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dcbUIGpuVirtualAddress);
}

void UI::ReleaseShaderVariables()
{
}
void UI::SetMyPos(float x, float y, float w, float h)
{
	XMStoreFloat4x4(&XYWH, XMMatrixIdentity());
	XYWH._11 = w;
	XYWH._22 = h;
	XYWH._41 = x;
	XYWH._42 = y;
}
void UI::OnPreRender()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
	m_xmf4x4World = XYWH;
	if (ParentUI) {
		m_xmf4x4World = Matrix4x4::Multiply(XYWH, ParentUI->XYWH);
	}
	m_xmf4x4World._11 = m_xmf4x4World._11 * 2; // 0 ~ 1 -> 0 ~ 2
	m_xmf4x4World._22 = m_xmf4x4World._22 * 2;
	m_xmf4x4World._41 = m_xmf4x4World._41 * 2 - 1; // 0 ~ 1 -> -1 ~ 1
	m_xmf4x4World._42 = m_xmf4x4World._42 * 2 - 1;
};

void UI::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	update();
	OnPreRender();
	UpdateShaderVariables(pd3dCommandList);

	if (m_pMaterial->m_pShader) m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
	if (m_pMaterial->m_pTexture)m_pMaterial->m_pTexture->UpdateShaderVariable(pd3dCommandList, 0);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dCommandList->DrawInstanced(6, 1, 0, 0);

}

Player_State_UI::Player_State_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) :UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/Player_State.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);

	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.02, 0.02, 0.5, 0.2);
}

Player_HP_UI::Player_HP_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/HP.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);

	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetMyPos(0.2, 0.04, 0.8, 0.32);
}

Player_HP_DEC_UI::Player_HP_DEC_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : Player_HP_UI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature)
{
	CTexture* pUITexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUITexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UI/HP_Decrease.dds", RESOURCE_TEXTURE2D, 0);

	UIShader* pUIShader = new UIShader();
	pUIShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D24_UNORM_S8_UINT);
	GameScene::CreateShaderResourceViews(pd3dDevice, pUITexture, 19, false);

	Material* pUIMaterial = new Material();
	pUIMaterial->SetTexture(pUITexture);
	pUIMaterial->SetShader(pUIShader);
	SetMaterial(pUIMaterial);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void Player_HP_DEC_UI::update() {
	if (Dec_HP < HP) {
		HP -= (HP - Dec_HP) / 65;
		if (HP < Dec_HP) HP = Dec_HP;
	}
	SetMyPos(0.2, 0.04, 0.8 * HP, 0.32);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


HeightMapTerrain::HeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : Object(true,false)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new HeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	HeightMapGridMesh* pMesh = new HeightMapGridMesh(pd3dDevice, pd3dCommandList, 0, 0, nWidth, nLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
	SetMesh(pMesh);


	CTexture* Terrain_Texture = new CTexture(10, RESOURCE_TEXTURE2D, 0, 18);

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

	DXGI_FORMAT pdxgiRtvFormats[MRT] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,  DXGI_FORMAT_R8G8B8A8_UNORM };

	TerrainShader* pTerrainShader = new TerrainShader();
	pTerrainShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, MRT, pdxgiRtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);

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