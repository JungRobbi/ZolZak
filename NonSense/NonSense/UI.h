//#pragma once
//#include "Object.h"
//#include "Shader.h"
///////////////////////////////////////////////////
//
//class UI : public Object
//{
//public:
//	UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
//	virtual ~UI();
//
//	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
//	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
//	virtual void ReleaseShaderVariables();
//	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
//	virtual void SetParentUI(UI* Parent) { ParentUI = Parent; }
//	virtual void SetPosition(float x, float y, float w, float h);
//	virtual void SetPos(float x, float y, float w, float h);
//
//private:
//	ID3D12Resource* m_pd3dcbUI = NULL;
//	CB_PLAYER_INFO* m_pcbMappedUI = NULL;
//	UI* ParentUI = NULL;
//};
////
//////////////////////////////////////////////////////
//
//class Player_State_UI : public UI
//{
//public:
//	Player_State_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
//	virtual ~Player_State_UI() {};
//};
