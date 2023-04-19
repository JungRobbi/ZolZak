#pragma once
#include "Object.h"

struct CB_PLAYER_INFO;

class UI : public Object
{
public:
	UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~UI();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
	virtual void SetParentUI(UI* Parent) { ParentUI = Parent; }
	virtual void SetMyPos(float x, float y, float w, float h);
	virtual void OnPreRender();
private:
	ID3D12Resource* m_pd3dcbUI = NULL;
	CB_PLAYER_INFO* m_pcbMappedUI = NULL;
	UI* ParentUI = NULL;
	XMFLOAT4X4 XYWH;
};

class Monster_HP_UI : public Object
{
public:
	Monster_HP_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Monster_HP_UI() {};
	float HP = 1.0;
};

class Monster_HP_DEC_UI : public Monster_HP_UI
{
public:
	Monster_HP_DEC_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Monster_HP_DEC_UI() {};
	virtual void update() {};
	float Dec_HP = 1.0;
};

class Player_State_UI : public UI
{
public:
	Player_State_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Player_State_UI() {};
};

class Player_HP_UI : public UI
{
public:
	Player_HP_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Player_HP_UI() {};
	float HP = 1.0;
};

class Player_HP_DEC_UI : public Player_HP_UI
{
public:
	Player_HP_DEC_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Player_HP_DEC_UI() {};
	virtual void update();
	float Dec_HP = 1.0;
};


class Option_UI : public UI
{
public:
	Option_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Option_UI() {};
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
};

class Game_Option_UI : public Option_UI
{
public:
	Game_Option_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Game_Option_UI() {};
};

class Graphic_Option_UI : public Option_UI
{
public:
	Graphic_Option_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Graphic_Option_UI() {};
};

class Sound_Option_UI : public Option_UI
{
public:
	Sound_Option_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Sound_Option_UI() {};
};
