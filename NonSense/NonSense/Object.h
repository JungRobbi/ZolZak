#pragma once
#include "Mesh.h"
class Shader;

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
protected:
	XMFLOAT4X4 m_xmf4x4World;
	Mesh* m_pMesh = NULL;
	Shader* m_pShader = NULL;
public:
	void ReleaseUploadBuffers();
	virtual void SetMesh(Mesh* pMesh);
	virtual void SetShader(Shader* pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};

