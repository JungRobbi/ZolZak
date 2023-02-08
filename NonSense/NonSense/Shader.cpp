#include "stdafx.h"
#include "Shader.h"
#include "GameScene.h"
#include "RotateComponent.h"

Shader::Shader()
{
	m_SRVCPUDescriptorStartHandle.ptr = m_CBVCPUDescriptorStartHandle.ptr = NULL;
	m_SRVGPUDescriptorStartHandle.ptr = m_CBVGPUDescriptorStartHandle.ptr = NULL;
}

Shader::~Shader()
{
	if (m_pPipelineState) m_pPipelineState->Release();
	if (m_pGraphicsRootSignature) m_pGraphicsRootSignature->Release();
	if (m_CBVSRVDescriptorHeap) m_CBVSRVDescriptorHeap->Release();
}

//래스터라이저 상태를 설정하기 위한 구조체를 반환한다.
D3D12_RASTERIZER_DESC Shader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	return(d3dRasterizerDesc);
}

//깊이-스텐실 검사를 위한 상태를 설정하기 위한 구조체를 반환한다.
D3D12_DEPTH_STENCIL_DESC Shader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	return(d3dDepthStencilDesc);
}

//블렌딩 상태를 설정하기 위한 구조체를 반환한다.
D3D12_BLEND_DESC Shader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return(d3dBlendDesc);
}

//입력 조립기에게 정점 버퍼의 구조를 알려주기 위한 구조체를 반환한다.
D3D12_INPUT_LAYOUT_DESC Shader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE Shader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;
	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE Shader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;
	return(d3dShaderByteCode);
}
//셰이더 소스 코드를 컴파일하여 바이트 코드 구조체를 반환한다.
D3D12_SHADER_BYTECODE Shader::CompileShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ID3DBlob* pd3dErrorBlob = NULL;
	HRESULT hResult = ::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, &pd3dErrorBlob);
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	return(d3dShaderByteCode);
}

//그래픽스 파이프라인 상태 객체를 생성한다.
void Shader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, DXGI_FORMAT dxgiDsvFormat)
{
	ID3DBlob* pd3dVertexShaderBlob = NULL, * pd3dPixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = nRenderTargets;
	for (UINT i = 0; i < nRenderTargets; i++) d3dPipelineStateDesc.RTVFormats[i] = (pdxgiRtvFormats) ? pdxgiRtvFormats[i] : DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = dxgiDsvFormat;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_pPipelineState);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void Shader::CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_CBVSRVDescriptorHeap);

	m_CBVCPUDescriptorStartHandle = m_CBVSRVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_CBVGPUDescriptorStartHandle = m_CBVSRVDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_SRVCPUDescriptorStartHandle.ptr = m_CBVCPUDescriptorStartHandle.ptr + (::CBVSRVDescriptorSize * nConstantBufferViews);
	m_SRVGPUDescriptorStartHandle.ptr = m_CBVGPUDescriptorStartHandle.ptr + (::CBVSRVDescriptorSize * nConstantBufferViews);

	m_SRVCPUDescriptorNextHandle = m_SRVCPUDescriptorStartHandle;
	m_SRVGPUDescriptorNextHandle = m_SRVGPUDescriptorStartHandle;
}

void Shader::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex)
{
	m_SRVCPUDescriptorNextHandle.ptr += (::CBVSRVDescriptorSize * nDescriptorHeapIndex);
	m_SRVGPUDescriptorNextHandle.ptr += (::CBVSRVDescriptorSize * nDescriptorHeapIndex);

	int nTextures = pTexture->GetTextures();
	UINT nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource* pShaderResource = pTexture->GetResource(i);
		if (pShaderResource)
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);
			pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_SRVCPUDescriptorNextHandle);
			m_SRVCPUDescriptorNextHandle.ptr += ::CBVSRVDescriptorSize;
			pTexture->SetGpuDescriptorHandle(i, m_SRVGPUDescriptorNextHandle);
			m_SRVGPUDescriptorNextHandle.ptr += ::CBVSRVDescriptorSize;
		}
	}
	int nRootParameters = pTexture->GetRootParameters();
	for (int i = 0; i < nRootParameters; i++) pTexture->SetRootParameterIndex(i, nRootParameterStartIndex + i);
}

void Shader::CreateShaderResourceViews(ID3D12Device* pd3dDevice, int nResources, ID3D12Resource** ppd3dResources, DXGI_FORMAT* pdxgiSrvFormats)
{
	for (int i = 0; i < nResources; i++)
	{
		if (ppd3dResources[i])
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
			d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			d3dShaderResourceViewDesc.Format = pdxgiSrvFormats[i];
			d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			d3dShaderResourceViewDesc.Texture2D.MipLevels = 1;
			d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
			d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
			pd3dDevice->CreateShaderResourceView(ppd3dResources[i], &d3dShaderResourceViewDesc, m_SRVCPUDescriptorNextHandle);
			m_SRVCPUDescriptorNextHandle.ptr += ::CBVSRVDescriptorSize;
			m_SRVGPUDescriptorNextHandle.ptr += ::CBVSRVDescriptorSize;
		}
	}
}
void Shader::CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_CBVCPUDescriptorStartHandle.ptr + (::CBVSRVDescriptorSize * j);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
}
void Shader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void Shader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}
void Shader::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}
void Shader::ReleaseShaderVariables()
{
}

void Shader::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pPipelineState) pd3dCommandList->SetPipelineState(m_pPipelineState);
	pd3dCommandList->SetDescriptorHeaps(1, &m_CBVSRVDescriptorHeap);
}

void Shader::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	OnPrepareRender(pd3dCommandList);
}

DiffusedShader::DiffusedShader()
{
}
DiffusedShader::~DiffusedShader()
{
}

D3D12_INPUT_LAYOUT_DESC DiffusedShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE DiffusedShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE DiffusedShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}

void DiffusedShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, DXGI_FORMAT dxgiDsvFormat)
{
	m_pGraphicsRootSignature = pd3dGraphicsRootSignature;
	m_pGraphicsRootSignature->AddRef();
	Shader::CreateShader(pd3dDevice, m_pGraphicsRootSignature, nRenderTargets, pdxgiRtvFormats, dxgiDsvFormat);
}

Object* ObjectsShader::CreateEmpty()
{
	return new Object();
}

ObjectsShader::ObjectsShader()
{
}
ObjectsShader::~ObjectsShader()
{
}

D3D12_INPUT_LAYOUT_DESC ObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE ObjectsShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "VSObject", "vs_5_1", ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE ObjectsShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "PSObject", "ps_5_1", ppd3dShaderBlob));
}

void ObjectsShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, DXGI_FORMAT dxgiDsvFormat)
{
	m_pGraphicsRootSignature = pd3dGraphicsRootSignature;
	m_pGraphicsRootSignature->AddRef();
	Shader::CreateShader(pd3dDevice, m_pGraphicsRootSignature, nRenderTargets, pdxgiRtvFormats, dxgiDsvFormat);
}

void ObjectsShader::ReleaseObjects()
{

}

void ObjectsShader::AnimateObjects(float fTimeElapsed)
{
	for (auto gameobject : GameScene::MainScene->gameObjects)
	{
		gameobject->Animate(fTimeElapsed);
	}
}

void ObjectsShader::ReleaseUploadBuffers()
{
	for (auto gameobject : GameScene::MainScene->gameObjects)
	{
		gameobject->ReleaseUploadBuffers();
	}
}

void ObjectsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	Shader::OnPrepareRender(pd3dCommandList);
	UpdateShaderVariables(pd3dCommandList);
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbGameObjectGpuVirtualAddress = m_pd3dcbGameObjects->GetGPUVirtualAddress();

	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
	int k = 0;
	for (auto gameobject : GameScene::MainScene->gameObjects)
	{
		pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_OBJECT, d3dcbGameObjectGpuVirtualAddress + (ncbGameObjectBytes * k++));
		gameobject->Render(pd3dCommandList, pCamera);
	}
}

Object* ObjectsShader::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfNearHitDistance)
{
	int nIntersected = 0;
	*pfNearHitDistance = FLT_MAX;
	float fHitDistance = FLT_MAX;
	Object* pSelectedObject = NULL;

	for (auto gameobject : GameScene::MainScene->gameObjects)
	{
		nIntersected = gameobject->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, &fHitDistance);
		if ((nIntersected > 0) && (fHitDistance < *pfNearHitDistance))
		{
			*pfNearHitDistance = fHitDistance;
			pSelectedObject = gameobject;
		}
	}
	return(pSelectedObject);
}

//객체의 정보를 저장하기 위한 리소스를 생성하고 리소스에 대한 포인터를 가져온다.
void ObjectsShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbGameObjectBytes * m_nObjects, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);
}
//객체의 월드변환 행렬과 재질 번호를 상수 버퍼에 쓴다.
void ObjectsShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbGameObjectBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	XMFLOAT4X4 xmf4x4World;

	int k = 0;
	for (auto gameobject : GameScene::MainScene->gameObjects)
	{
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&gameobject->GetWorld())));
		CB_GAMEOBJECT_INFO* pbMappedcbGameObject = (CB_GAMEOBJECT_INFO*)(m_pcbMappedGameObjects + (k * ncbGameObjectBytes));
		::memcpy(&pbMappedcbGameObject->m_xmf4x4World, &xmf4x4World, sizeof(XMFLOAT4X4));
		pbMappedcbGameObject->m_nMaterial = gameobject->GetMaterial()->m_nReflection;
		pbMappedcbGameObject->m_nObjectID = k++;
	}
}
void ObjectsShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
}

void ObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CubeMeshIlluminated* pCubeMesh = new CubeMeshIlluminated(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);
	int xObjects = 1, yObjects = 1, zObjects = 1, i = 0;
	m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);

	CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2DARRAY, 0, 1);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"StonesArray.dds", RESOURCE_TEXTURE2DARRAY, 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, m_nObjects, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pTexture, 0, 5);

	m_pMaterial = new Material();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	float fxPitch = 12.0f * 2.5f;
	float fyPitch = 12.0f * 2.5f;
	float fzPitch = 12.0f * 2.5f;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				auto object = CreateEmpty();
				object->SetMaterial(i % MAX_MATERIALS);
				object->SetMesh(pCubeMesh);
				object->SetPosition(fxPitch * x, fyPitch * y, fzPitch * z);
				object->AddComponent<RotateComponent>();
				object->GetComponent<RotateComponent>()->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				object->GetComponent<RotateComponent>()->SetRotationSpeed(10.0f * ((i++) % 10));
				//object->SetCbvGPUDescriptorHandlePtr(m_CBVGPUDescriptorStartHandle.ptr + (::CBVSRVDescriptorSize * i));
			}
		}
	}

}

ScreenShader::ScreenShader()
{
}

ScreenShader::~ScreenShader()
{
	ReleaseShaderVariables();
}

D3D12_INPUT_LAYOUT_DESC ScreenShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_DEPTH_STENCIL_DESC ScreenShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE ScreenShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "VSScreen", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE ScreenShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(Shader::CompileShaderFromFile(L"Shaders.hlsl", "PSScreen", "ps_5_1", ppd3dShaderBlob));
}

void ScreenShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, DXGI_FORMAT dxgiDsvFormat)
{
	m_pGraphicsRootSignature = pd3dGraphicsRootSignature;
	m_pGraphicsRootSignature->AddRef();

	Shader::CreateShader(pd3dDevice, m_pGraphicsRootSignature, nRenderTargets, pdxgiRtvFormats, dxgiDsvFormat);
}

void ScreenShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void ScreenShader::ReleaseShaderVariables()
{
}

void ScreenShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void ScreenShader::CreateResourcesAndViews(ID3D12Device* pd3dDevice, UINT nResources, DXGI_FORMAT* pdxgiFormats, UINT nWidth, UINT nHeight, D3D12_CPU_DESCRIPTOR_HANDLE m_RTVDescriptorHandle, UINT nShaderResources)
{
	m_pTexture = new CTexture(nResources, RESOURCE_TEXTURE2D, 0, 1);

	D3D12_CLEAR_VALUE d3dClearValue = { DXGI_FORMAT_R8G8B8A8_UNORM, { 0.0f, 0.0f, 1.0f, 1.0f } };
	for (UINT i = 0; i < nResources; i++)
	{
		d3dClearValue.Format = pdxgiFormats[i];
		m_pTexture->CreateTexture(pd3dDevice, nWidth, nHeight, pdxgiFormats[i], D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON, &d3dClearValue, RESOURCE_TEXTURE2D, i);
	}

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, nShaderResources);
	CreateShaderVariables(pd3dDevice, NULL);
	CreateShaderResourceViews(pd3dDevice, m_pTexture, 0, 6);

	D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc;
	d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
	d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;

	m_pm_RTVDescriptorHandles = new D3D12_CPU_DESCRIPTOR_HANDLE[nResources];

	for (UINT i = 0; i < nResources; i++)
	{
		d3dRenderTargetViewDesc.Format = pdxgiFormats[i];
		ID3D12Resource* pd3dTextureResource = m_pTexture->GetResource(i);
		pd3dDevice->CreateRenderTargetView(pd3dTextureResource, &d3dRenderTargetViewDesc, m_RTVDescriptorHandle);
		m_pm_RTVDescriptorHandles[i] = m_RTVDescriptorHandle;
		m_RTVDescriptorHandle.ptr += ::RTVDescriptorSize;
	}
}

void ScreenShader::OnPrepareRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList, int nRenderTargets, D3D12_CPU_DESCRIPTOR_HANDLE* pd3dRtvCPUHandles, D3D12_CPU_DESCRIPTOR_HANDLE d3dDepthStencilBufferDSVCPUHandle)
{
	int nResources = m_pTexture->GetTextures();
	D3D12_CPU_DESCRIPTOR_HANDLE* pd3dAllRtvCPUHandles = new D3D12_CPU_DESCRIPTOR_HANDLE[nRenderTargets + nResources];

	for (int i = 0; i < nRenderTargets; i++)
	{
		pd3dAllRtvCPUHandles[i] = pd3dRtvCPUHandles[i];
		pd3dCommandList->ClearRenderTargetView(pd3dRtvCPUHandles[i], Colors::Yellow, 0, NULL);
	}

	for (int i = 0; i < nResources; i++)
	{
		::ResourceTransition(pd3dCommandList, GetTextureResource(i), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

		D3D12_CPU_DESCRIPTOR_HANDLE m_RTVDescriptorHandle = GetRtvCPUDescriptorHandle(i);
		pd3dCommandList->ClearRenderTargetView(m_RTVDescriptorHandle, Colors::LightSkyBlue, 0, NULL);
		pd3dAllRtvCPUHandles[nRenderTargets + i] = m_RTVDescriptorHandle;
	}
	pd3dCommandList->OMSetRenderTargets(nRenderTargets + nResources, pd3dAllRtvCPUHandles, FALSE, &d3dDepthStencilBufferDSVCPUHandle);

	if (pd3dAllRtvCPUHandles) delete[] pd3dAllRtvCPUHandles;
}

void ScreenShader::OnPostRenderTarget(ID3D12GraphicsCommandList* pd3dCommandList)
{
	int nResources = m_pTexture->GetTextures();
	for (int i = 0; i < nResources; i++)
	{
		::ResourceTransition(pd3dCommandList, GetTextureResource(i), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
	}
}

void ScreenShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	Shader::Render(pd3dCommandList, pCamera);

	if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dCommandList->DrawInstanced(6, 1, 0, 0);
}