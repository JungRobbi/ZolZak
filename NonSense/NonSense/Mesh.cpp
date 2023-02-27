#include "stdafx.h"
#include "Mesh.h"
#include "Object.h"

Mesh::Mesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
Mesh::~Mesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	if (m_pVertices) delete[] m_pVertices;
if (m_pnIndices) delete[] m_pnIndices;
}
void Mesh::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
};

void Mesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		//인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링한다.
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

void Mesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0);
	}
}
void Mesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances,
	D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView)
{
	//정점 버퍼 뷰와 인스턴싱 버퍼 뷰를 입력-조립 단계에 설정한다.
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[] = { m_d3dVertexBufferView, d3dInstancingBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, _countof(pVertexBufferViews), pVertexBufferViews);
	Render(pd3dCommandList, nInstances);
}

int Mesh::CheckRayIntersection(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection, float* pfNearHitDistance)
{
	//하나의 메쉬에서 광선은 여러 개의 삼각형과 교차할 수 있다. 교차하는 삼각형들 중 가장 가까운 삼각형을 찾는다.
	int nIntersections = 0;
	BYTE* pbPositions = (BYTE*)m_pVertices;
	int nOffset = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	/*메쉬의 프리미티브(삼각형)들의 개수이다. 삼각형 리스트인 경우 (정점의 개수 / 3) 또는 (인덱스의 개수 / 3), 삼각형 스트립의 경우 (정점의 개수 - 2) 또는 (인덱스의 개수 – 2)이다.*/
	int nPrimitives = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);
	//광선은 모델 좌표계로 표현된다.
	XMVECTOR xmRayOrigin = XMLoadFloat3(&xmf3RayOrigin);
	XMVECTOR xmRayDirection = XMLoadFloat3(&xmf3RayDirection);
	//모델 좌표계의 광선과 메쉬의 바운딩 박스(모델 좌표계)와의 교차를 검사한다.
	bool bIntersected = m_xmBoundingBox.Intersects(xmRayOrigin, xmRayDirection, *pfNearHitDistance);
	//모델 좌표계의 광선이 메쉬의 바운딩 박스와 교차하면 메쉬와의 교차를 검사한다.
	if (bIntersected)
	{
		float fNearHitDistance = FLT_MAX;
		/*메쉬의 모든 프리미티브(삼각형)들에 대하여 픽킹 광선과의 충돌을 검사한다. 충돌하는 모든 삼각형을 찾아 광선의
		시작점(실제로는 카메라 좌표계의 원점)에 가장 가까운 삼각형을 찾는다.*/
		for (int i = 0; i < nPrimitives; i++)
		{
			XMVECTOR v0 = XMLoadFloat3((XMFLOAT3*)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i * nOffset) + 0]) : ((i * nOffset) + 0)) * m_nStride));
			XMVECTOR v1 = XMLoadFloat3((XMFLOAT3*)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i * nOffset) + 1]) : ((i * nOffset) + 1)) * m_nStride));
			XMVECTOR v2 = XMLoadFloat3((XMFLOAT3*)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i * nOffset) + 2]) : ((i * nOffset) + 2)) * m_nStride));
			float fHitDistance;
			BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);
			if (bIntersected)
			{
				if (fHitDistance < fNearHitDistance)
				{
					*pfNearHitDistance = fNearHitDistance = fHitDistance;
				}
				nIntersections++;
			}
		}
	}
	return(nIntersections);
}

TriangleMesh::TriangleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : Mesh(pd3dDevice, pd3dCommandList)
{
	//삼각형 메쉬를 정의한다.
	m_nVertices = 3;
	m_nStride = sizeof(DiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	/*정점(삼각형의 꼭지점)의 색상은 시계방향 순서대로 빨간색, 녹색, 파란색으로 지정한다. RGBA(Red, Green, Blue,
	Alpha) 4개의 파라메터를 사용하여 색상을 표현한다. 각 파라메터는 0.0~1.0 사이의 실수값을 가진다.*/
	DiffusedVertex pVertices[3];
	pVertices[0] = DiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f,1.0f));
	pVertices[1] = DiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f,1.0f));
	pVertices[2] = DiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));
	//삼각형 메쉬를 리소스(정점 버퍼)로 생성한다.
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
	m_nStride* m_nVertices, D3D12_HEAP_TYPE_DEFAULT,D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, & m_pd3dVertexUploadBuffer);
	//정점 버퍼 뷰를 생성한다.
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CubeMesh::CubeMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) : Mesh(pd3dDevice, pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다.
	m_nVertices = 8;
	m_nStride = sizeof(DiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다.
	m_pVertices = new DiffusedVertex[m_nVertices];
	m_pVertices[0] = DiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	m_pVertices[1] = DiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	m_pVertices[2] = DiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	m_pVertices[3] = DiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	m_pVertices[4] = DiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	m_pVertices[5] = DiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	m_pVertices[6] = DiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	m_pVertices[7] = DiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	/*인덱스 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다. 삼각형 리스트로 직육면체를 표현할 것이므로 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 정점이 필요하다. 즉, 인덱스 버퍼는 전체 36(=6*2*3)개의 인덱스를 가져야 한다.*/
	m_nIndices = 36;
	m_pnIndices = new UINT[m_nIndices];
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	m_pnIndices[0] = 3; m_pnIndices[1] = 1; m_pnIndices[2] = 0;
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	m_pnIndices[3] = 2; m_pnIndices[4] = 1; m_pnIndices[5] = 3;
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	m_pnIndices[6] = 0; m_pnIndices[7] = 5; m_pnIndices[8] = 4;
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	m_pnIndices[9] = 1; m_pnIndices[10] = 5; m_pnIndices[11] = 0;
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	m_pnIndices[12] = 3; m_pnIndices[13] = 4; m_pnIndices[14] = 7;
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	m_pnIndices[15] = 0; m_pnIndices[16] = 4; m_pnIndices[17] = 3;
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	m_pnIndices[18] = 1; m_pnIndices[19] = 6; m_pnIndices[20] = 5;
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	m_pnIndices[21] = 2; m_pnIndices[22] = 6; m_pnIndices[23] = 1;
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	m_pnIndices[24] = 2; m_pnIndices[25] = 7; m_pnIndices[26] = 6;
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	m_pnIndices[27] = 3; m_pnIndices[28] = 7; m_pnIndices[29] = 2;
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	m_pnIndices[30] = 6; m_pnIndices[31] = 4; m_pnIndices[32] = 5;
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	m_pnIndices[33] = 7; m_pnIndices[34] = 4; m_pnIndices[35] = 6;
	//인덱스 버퍼를 생성한다.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices,
	sizeof(UINT)* m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, & m_pd3dIndexUploadBuffer);
	//인덱스 버퍼 뷰를 생성한다.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy,fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CubeMesh::~CubeMesh()
{
}

SphereMesh::SphereMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fRadius, int nSlices, int nStacks) : Mesh(pd3dDevice, pd3dCommandList)
{
	m_nStride = sizeof(DiffusedVertex);
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nVertices = 2 + (nSlices * (nStacks - 1));
	m_pVertices = new DiffusedVertex[m_nVertices];
	//180도를 nStacks 만큼 분할한다.
	float fDeltaPhi = float(XM_PI / nStacks);
	//360도를 nSlices 만큼 분할한다.
	float fDeltaTheta = float((2.0f * XM_PI) / nSlices);
	int k = 0;
	//구의 위(북극)를 나타내는 정점이다.
	m_pVertices[k++] = DiffusedVertex(0.0f, +fRadius, 0.0f, RANDOM_COLOR);
	float theta_i, phi_j;
	//원기둥 표면의 정점이다.
	for (int j = 1; j < nStacks; j++)
	{
		phi_j = fDeltaPhi * j;
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			m_pVertices[k++] = DiffusedVertex(fRadius * sinf(phi_j) * cosf(theta_i),
				fRadius * cosf(phi_j), fRadius * sinf(phi_j) * sinf(theta_i), RANDOM_COLOR);
		}
	}
	//구의 아래(남극)를 나타내는 정점이다.
	m_pVertices[k] = DiffusedVertex(0.0f, -fRadius, 0.0f, RANDOM_COLOR);
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	m_pnIndices = new UINT[m_nIndices];
	k = 0;
	//구의 위쪽 원뿔의 표면을 표현하는 삼각형들의 인덱스이다.
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = 0;
		m_pnIndices[k++] = 1 + ((i + 1) % nSlices);
		m_pnIndices[k++] = 1 + i;
	}
	//구의 원기둥의 표면을 표현하는 삼각형들의 인덱스이다.
	for (int j = 0; j < nStacks-2; j++)
	{
		for (int i = 0; i < nSlices; i++)
		{
			//사각형의 첫 번째 삼각형의 인덱스이다.
			m_pnIndices[k++] = 1 + (i + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			//사각형의 두 번째 삼각형의 인덱스이다.
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		}
	}
	//구의 아래쪽 원뿔의 표면을 표현하는 삼각형들의 인덱스이다.
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = (m_nVertices - 1);
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + i;
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	m_xmBoundingBox = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fRadius, fRadius, fRadius), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}
SphereMesh::~SphereMesh()
{
}

IlluminatedMesh::IlluminatedMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : Mesh(pd3dDevice, pd3dCommandList)
{
}
IlluminatedMesh::~IlluminatedMesh()
{
}

void IlluminatedMesh::CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals,XMFLOAT3* pxmf3Positions, int nVertices)
{
	int nPrimitives = nVertices / 3;
	UINT nIndex0, nIndex1, nIndex2;
	for (int i = 0; i < nPrimitives; i++)
	{
		nIndex0 = i * 3 + 0;
		nIndex1 = i * 3 + 1;
		nIndex2 = i * 3 + 2;
		XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1],pxmf3Positions[nIndex0]);
		XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2],pxmf3Positions[nIndex0]);
		pxmf3Normals[nIndex0] = pxmf3Normals[nIndex1] = pxmf3Normals[nIndex2] = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
	}
}

void IlluminatedMesh::CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices / 3) : (nVertices / 3);
	XMFLOAT3 xmf3SumOfNormal, xmf3Edge01, xmf3Edge02, xmf3Normal;
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = pnIndices[i * 3 + 0];
			nIndex1 = pnIndices[i * 3 + 1];
			nIndex2 = pnIndices[i * 3 + 2];
			if (pnIndices && ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)))
			{
				xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1],pxmf3Positions[nIndex0]);
				xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2],pxmf3Positions[nIndex0]);
				xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, false);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void IlluminatedMesh::CalculateTriangleStripVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, UINT nVertices, UINT* pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices - 2) : (nVertices - 2);
	XMFLOAT3 xmf3SumOfNormal(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = ((i % 2) == 0) ? (i + 0) : (i + 1);
			if (pnIndices) nIndex0 = pnIndices[nIndex0];
			nIndex1 = ((i % 2) == 0) ? (i + 1) : (i + 0);
			if (pnIndices) nIndex1 = pnIndices[nIndex1];
			nIndex2 = (pnIndices) ? pnIndices[i + 2] : (i + 2);
			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j))
			{
				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1],pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2],pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void IlluminatedMesh::CalculateVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices, UINT* pnIndices, int nIndices)
{
	switch (m_d3dPrimitiveTopology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (pnIndices) CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		else CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices);
		break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		CalculateTriangleStripVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		break;
	default:
		break;
	}
}

CubeMeshIlluminated::CubeMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) : IlluminatedMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 8;
	m_nStride = sizeof(IlluminatedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nIndices = 36;
	UINT pnIndices[36];
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices,sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,&m_pd3dIndexUploadBuffer);
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	XMFLOAT3 pxmf3Positions[8];
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	pxmf3Positions[0] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[1] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[2] = XMFLOAT3(+fx, +fy, +fz);
	pxmf3Positions[3] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[4] = XMFLOAT3(-fx, -fy, -fz);
	pxmf3Positions[5] = XMFLOAT3(+fx, -fy, -fz);
	pxmf3Positions[6] = XMFLOAT3(+fx, -fy, +fz);
	pxmf3Positions[7] = XMFLOAT3(-fx, -fy, +fz);
	XMFLOAT3 pxmf3Normals[8];
	for (int i = 0; i < 8; i++) pxmf3Normals[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);
	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, pnIndices,m_nIndices);
	IlluminatedVertex pVertices[8];
	for (int i = 0; i < 8; i++) pVertices[i] = IlluminatedVertex(pxmf3Positions[i],pxmf3Normals[i]);
	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}
CubeMeshIlluminated::~CubeMeshIlluminated()
{
}

// LoadMesh
LoadMesh::LoadMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : IlluminatedMesh(pd3dDevice, pd3dCommandList)
{

}
LoadMesh::~LoadMesh()
{

}

void LoadMesh::LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* OpenedFile)
{
	char pstrToken[64] = { '\0' };
	int nPositions = 0, nColors = 0, nNormals = 0, nTangents = 0, nBiTangents = 0, nTextureCoords = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	UINT nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, OpenedFile);

	::ReadStringFromFile(OpenedFile, m_pstrMeshName);

	while (true)
	{
		::ReadStringFromFile(OpenedFile, pstrToken);
		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&m_xmBoundingBox.Center, sizeof(XMFLOAT3), 1, OpenedFile);
			nReads = (UINT)::fread(&m_xmBoundingBox.Extents, sizeof(XMFLOAT3), 1, OpenedFile);

		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			nReads = (UINT)::fread(&nPositions, sizeof(int), 1, OpenedFile);
			if (nPositions > 0)
			{
				m_pxmf3Positions = new XMFLOAT3[nPositions];
				nReads = (UINT)::fread(m_pxmf3Positions, sizeof(XMFLOAT3), nPositions, OpenedFile);

				m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

				m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
				m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

			}

		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nReads = (UINT)::fread(&nColors, sizeof(int), 1, OpenedFile);
			if (nColors > 0)
			{
				m_pxmf4Colors = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(m_pxmf4Colors, sizeof(XMFLOAT4), nColors, OpenedFile);
			}

		}

		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, OpenedFile);
			if (nTextureCoords > 0)
			{
				m_pxmf2TextureCoords0 = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(m_pxmf2TextureCoords0, sizeof(XMFLOAT2), nTextureCoords, OpenedFile);

				m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);

				m_d3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
				m_d3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_d3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;

			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, OpenedFile);
			if (nTextureCoords > 0)
			{
				m_pxmf2TextureCoords0 = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(m_pxmf2TextureCoords0, sizeof(XMFLOAT2), nTextureCoords, OpenedFile);

				m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);

				m_d3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
				m_d3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_d3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;

			}
		}

		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nReads = (UINT)::fread(&nNormals, sizeof(int), 1, OpenedFile);
			if (nNormals > 0)
			{
				m_pxmf3Normals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, OpenedFile);

				m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

				m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
				m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			nReads = (UINT)::fread(&nTangents, sizeof(int), 1, OpenedFile);
			if (nTangents > 0)
			{
				m_pxmf3Tangents = new XMFLOAT3[nTangents];
				nReads = (UINT)::fread(m_pxmf3Tangents, sizeof(XMFLOAT3), nTangents, OpenedFile);

				m_pd3dTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Tangents, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTangentUploadBuffer);

				m_d3dTangentBufferView.BufferLocation = m_pd3dTangentBuffer->GetGPUVirtualAddress();
				m_d3dTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{
			nReads = (UINT)::fread(&nBiTangents, sizeof(int), 1, OpenedFile);
			if (nBiTangents > 0)
			{
				m_pxmf3BiTangents = new XMFLOAT3[nBiTangents];
				nReads = (UINT)::fread(m_pxmf3BiTangents, sizeof(XMFLOAT3), nBiTangents, OpenedFile);

				m_pd3dBiTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3BiTangents, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBiTangentUploadBuffer);

				m_d3dBiTangentBufferView.BufferLocation = m_pd3dBiTangentBuffer->GetGPUVirtualAddress();
				m_d3dBiTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dBiTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			nReads = (UINT)::fread(&(m_nSubMeshes), sizeof(int), 1, OpenedFile);
			if (m_nSubMeshes > 0)
			{
				m_pnSubSetIndices = new int[m_nSubMeshes];
				m_ppnSubSetIndices = new UINT * [m_nSubMeshes];

				m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
				m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];
				m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];

				for (int i = 0; i < m_nSubMeshes; i++)
				{
					::ReadStringFromFile(OpenedFile, pstrToken);
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = 0;
						nReads = (UINT)::fread(&nIndex, sizeof(int), 1, OpenedFile); //i
						nReads = (UINT)::fread(&(m_pnSubSetIndices[i]), sizeof(int), 1, OpenedFile);
						if (m_pnSubSetIndices[i] > 0)
						{
							m_ppnSubSetIndices[i] = new UINT[m_pnSubSetIndices[i]];
							nReads = (UINT)::fread(m_ppnSubSetIndices[i], sizeof(UINT), m_pnSubSetIndices[i], OpenedFile);

							m_ppd3dSubSetIndexBuffers[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[i], sizeof(UINT) * m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[i]);

							m_pd3dSubSetIndexBufferViews[i].BufferLocation = m_ppd3dSubSetIndexBuffers[i]->GetGPUVirtualAddress();
							m_pd3dSubSetIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
							m_pd3dSubSetIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
						}
					}
				}
			}

		}

		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}



	}
}




// SkinMesh
SkinnedMesh::SkinnedMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : LoadMesh(pd3dDevice, pd3dCommandList)
{

}

SkinnedMesh::~SkinnedMesh()
{
	if (m_pBoneIndices) delete[] m_pBoneIndices;
	if (m_pBoneWeight) delete[] m_pBoneWeight;

	if (m_ppSkinningBoneFrameCaches) delete[] m_ppSkinningBoneFrameCaches;
	if (m_ppstrSkinningBoneNames) delete[] m_ppstrSkinningBoneNames;

	if (m_pBindPoseBoneOffsets) delete[] m_pBindPoseBoneOffsets;
	if (m_pd3dcbBindPoseBoneOffsets) m_pd3dcbBindPoseBoneOffsets->Release();

	if (m_pd3dBoneIndexBuffer) m_pd3dBoneIndexBuffer->Release();
	if (m_pd3dBoneWeightBuffer) m_pd3dBoneWeightBuffer->Release();

	//ReleaseShaderVariables();
}

void SkinnedMesh::PrepareSkinning(Object* pRoot)
{
	for (int j = 0; j < m_nSkinningBones; j++)
	{
		m_ppSkinningBoneFrameCaches[j] = pRoot->FindFrame(m_ppstrSkinningBoneNames[j]);
	}
}

void SkinnedMesh::LoadSkinInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* OpenedFile)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	::ReadStringFromFile(OpenedFile, m_pstrMeshName);


	while (true)
	{
		::ReadStringFromFile(OpenedFile, pstrToken);
		if (!strcmp(pstrToken, "<BonesPerVertex>:"))
		{
			m_nBonesPerVertex = ::ReadIntegerFromFile(OpenedFile);
		}
		else if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&m_xmBoundingBox.Center, sizeof(XMFLOAT3), 1, OpenedFile);
			nReads = (UINT)::fread(&m_xmBoundingBox.Extents, sizeof(XMFLOAT3), 1, OpenedFile);
		}
		else if (!strcmp(pstrToken, "<BoneNames>:"))
		{
			m_nSkinningBones = ::ReadIntegerFromFile(OpenedFile);
			if (m_nSkinningBones > 0)
			{
				m_ppstrSkinningBoneNames = new char[m_nSkinningBones][64];
				m_ppSkinningBoneFrameCaches = new Object * [m_nSkinningBones];
				for (int i = 0; i < m_nSkinningBones; i++)
				{
					::ReadStringFromFile(OpenedFile, m_ppstrSkinningBoneNames[i]);
					m_ppSkinningBoneFrameCaches[i] = NULL;
				}
			}
		}
		else if (!strcmp(pstrToken, "<BoneOffsets>:"))
		{
			m_nSkinningBones = ::ReadIntegerFromFile(OpenedFile);
			if (m_nSkinningBones > 0)
			{
				m_pBindPoseBoneOffsets = new XMFLOAT4X4[m_nSkinningBones];
				nReads = (UINT)::fread(m_pBindPoseBoneOffsets, sizeof(XMFLOAT4X4), m_nSkinningBones, OpenedFile);

				UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
				m_pd3dcbBindPoseBoneOffsets = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
				m_pd3dcbBindPoseBoneOffsets->Map(0, NULL, (void**)&m_pcbxmf4x4MappedBindPoseBoneOffsets);

				for (int i = 0; i < m_nSkinningBones; i++)
				{
					XMStoreFloat4x4(&m_pcbxmf4x4MappedBindPoseBoneOffsets[i], XMMatrixTranspose(XMLoadFloat4x4(&m_pBindPoseBoneOffsets[i])));
				}
			}
		}
		else if (!strcmp(pstrToken, "<BoneIndices>:"))
		{
			m_nVertices = ::ReadIntegerFromFile(OpenedFile);
			if (m_nVertices > 0)
			{
				m_pBoneIndices = new XMINT4[m_nVertices];

				nReads = (UINT)::fread(m_pBoneIndices, sizeof(XMINT4), m_nVertices, OpenedFile);
				m_pd3dBoneIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pBoneIndices, sizeof(XMINT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoneIndexUploadBuffer);

				m_d3dBoneIndexBufferView.BufferLocation = m_pd3dBoneIndexBuffer->GetGPUVirtualAddress();
				m_d3dBoneIndexBufferView.StrideInBytes = sizeof(XMINT4);
				m_d3dBoneIndexBufferView.SizeInBytes = sizeof(XMINT4) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<BoneWeights>:"))
		{
			m_nVertices = ::ReadIntegerFromFile(OpenedFile);
			if (m_nVertices > 0)
			{
				m_pBoneWeight = new XMFLOAT4[m_nVertices];

				nReads = (UINT)::fread(m_pBoneWeight, sizeof(XMFLOAT4), m_nVertices, OpenedFile);
				m_pd3dBoneWeightBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pBoneWeight, sizeof(XMFLOAT4) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBoneWeightUploadBuffer);

				m_d3dBoneWeightBufferView.BufferLocation = m_pd3dBoneWeightBuffer->GetGPUVirtualAddress();
				m_d3dBoneWeightBufferView.StrideInBytes = sizeof(XMFLOAT4);
				m_d3dBoneWeightBufferView.SizeInBytes = sizeof(XMFLOAT4) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "</SkinningInfo>"))
		{
			break;
		}





	}
}

