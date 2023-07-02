#pragma once
#include "Object.h"

struct CB_PLAYER_INFO;

class UI : public Object
{
public:
	UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~UI();
	XMFLOAT4X4 XYWH;
	bool CanClick = false;

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
	virtual void SetParentUI(UI* Parent) { ParentUI = Parent; }
	virtual void SetMyPos(float x, float y, float w, float h);
	virtual void OnPreRender();
	virtual void OnClick() {};
private:
	ID3D12Resource* m_pd3dcbUI = NULL;
	CB_PLAYER_INFO* m_pcbMappedUI = NULL;
	UI* ParentUI = NULL;
};

class Aim : public UI
{
public:
	Aim(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Aim() {};
};

class NPCScript : public UI
{
public:
	NPCScript(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~NPCScript() {};
};

class Monster_HP_UI : public UI
{
public:
	Monster_HP_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Monster_HP_UI() {};
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
	float HP = 1.0;
private:
	ID3D12Resource* m_pd3dcbUI = NULL;
	CB_PLAYER_INFO* m_pcbMappedUI = NULL;
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

class Player_HP_DEC_UI : public UI
{
public:
	Player_HP_DEC_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Player_HP_DEC_UI() {};
	virtual void update();
	float HP = 1.0;
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

class Login_BackGround_UI : public UI
{
public:
	Login_BackGround_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Login_BackGround_UI() {};
};

class WhiteRect_UI : public UI
{
public:
	WhiteRect_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~WhiteRect_UI() {};
};

class Login_UI : public UI
{
public:
	Login_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Login_UI() {};
	virtual void OnClick();
};

class Lobby_BackGround_UI : public UI
{
public:
	Lobby_BackGround_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Lobby_BackGround_UI() {};
};

class Lobby_UI : public UI
{
public:
	Lobby_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Lobby_UI() {};
};

class Make_Room_UI : public UI
{
public:
	Make_Room_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Make_Room_UI() {};
};

class Join_Room_UI : public UI
{
public:
	Join_Room_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Join_Room_UI() {};
};

class Back_UI : public UI
{
public:
	Back_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Back_UI() {};
};

class Right_UI : public UI
{
public:
	Right_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Right_UI() {};
};

class Left_UI : public UI
{
public:
	Left_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Left_UI() {};
};

class Title_UI : public UI
{
public:
	Title_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Title_UI() {};
};