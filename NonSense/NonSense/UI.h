#pragma once
#include <list>
#include "Object.h"
#include "UILayer.h"

struct CB_PLAYER_INFO;

class UI : public Object
{
public:
	UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~UI();
	XMFLOAT4X4 XYWH;
	bool CanClick = false;
	bool MouseOn = false;

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

class Heal_UI : public UI
{
public:
	Heal_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
	virtual ~Heal_UI() {};
private:
	ID3D12Resource* m_pd3dcbUI = NULL;
	CB_PLAYER_INFO* m_pcbMappedUI = NULL;
};

class Buff_UI : public UI
{
public:
	Buff_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
	virtual ~Buff_UI() {};
private:
	ID3D12Resource* m_pd3dcbUI = NULL;
	CB_PLAYER_INFO* m_pcbMappedUI = NULL;
};

class Signal_UI : public UI
{
public:
	Signal_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
	virtual ~Signal_UI() {};
private:
	ID3D12Resource* m_pd3dcbUI = NULL;
	CB_PLAYER_INFO* m_pcbMappedUI = NULL;
};

class Player_State_UI : public UI
{
public:
	Player_State_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool ismage);
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

class Player_Over_HP_UI : public UI
{
public:
	Player_Over_HP_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Player_Over_HP_UI() {};
	float HP = 1.0;
};

class Player_Over_DEC_HP_UI : public UI
{
public:
	Player_Over_DEC_HP_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Player_Over_DEC_HP_UI() {};
	virtual void update();
	float HP = 0.0;
	float Dec_HP = 0.0;
};

class Option_UI : public UI
{
public:
	Option_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Option_UI() {};
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
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
	virtual void OnClick();
	virtual ~Make_Room_UI() {};
};

class Join_Room_UI : public UI
{
public:
	Join_Room_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual ~Join_Room_UI() {};
};

class Back_UI : public UI
{
public:
	Back_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual ~Back_UI() {};
};

class Right_UI : public UI
{
public:
	Right_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual ~Right_UI() {};
};

class Left_UI : public UI
{
public:
	Left_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual ~Left_UI() {};
};

class Title_UI : public UI
{
public:
	Title_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Title_UI() {};
};

class Players_UI : public UI
{
public:
	Players_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Players_UI() {};
};

class Make_Title_UI : public UI
{
public:
	Make_Title_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Make_Title_UI() {};
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
};

class Real_Make_Room_UI : public UI
{
public:
	Real_Make_Room_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Real_Make_Room_UI() {};
	virtual void OnClick();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
};

class Room_UI : public UI
{
public:
	Room_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, int num, std::string name, std::string owner);
	virtual ~Room_UI() {};
	virtual void OnClick();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
	int RoomNum = -1;
	std::string RoomName = "";
	std::string RoomOwner = "";
};

class Line_Right_UI : public UI
{
public:
	Line_Right_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual ~Line_Right_UI() {};
};

class Line_Left_UI : public UI
{
public:
	Line_Left_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual ~Line_Left_UI() {};
};

class Color_Right_UI : public UI
{
public:
	Color_Right_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual ~Color_Right_UI() {};
};

class Color_Left_UI : public UI
{
public:
	Color_Left_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual ~Color_Left_UI() {};
};

class Toon_Right_UI : public UI
{
public:
	Toon_Right_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual ~Toon_Right_UI() {};
};

class Toon_Left_UI : public UI
{
public:
	Toon_Left_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual ~Toon_Left_UI() {};
};

class Shadow_Right_UI : public UI
{
public:
	Shadow_Right_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual ~Shadow_Right_UI() {};
};

class Shadow_Left_UI : public UI
{
public:
	Shadow_Left_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual ~Shadow_Left_UI() {};
};

class Mouse_Right_UI : public UI
{
public:
	Mouse_Right_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual ~Mouse_Right_UI() {};
};

class Mouse_Left_UI : public UI
{
public:
	Mouse_Left_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual ~Mouse_Left_UI() {};
};

class Mage_UI : public UI
{
public:
	Mage_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual ~Mage_UI() {};
};

class Warrior_UI : public UI
{
public:
	Warrior_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual ~Warrior_UI() {};
};

class Leave_Room_UI : public UI
{
public:
	Leave_Room_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual ~Leave_Room_UI() {};
};

class Ready_UI : public UI
{
public:
	Ready_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void OnClick();
	virtual ~Ready_UI() {};

	bool b_Ready = false;
};

class Room_Back_UI : public UI
{
public:
	Room_Back_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Room_Back_UI() {};
};

class Loading_UI : public UI
{
public:
	Loading_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual ~Loading_UI() {};
};

class GameOver_UI : public UI
{
public:
	GameOver_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual ~GameOver_UI() {};
};

class Stat_UI : public UI
{
public:
	Stat_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Stat_UI() {};
};

class ATK_UI : public UI
{
public:
	ATK_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~ATK_UI() {};
};

class DEF_UI : public UI
{
public:
	DEF_UI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~DEF_UI() {};
};