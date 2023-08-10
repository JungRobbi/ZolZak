#pragma once
#include "../Globals.h"
#include "../AnimationType.h"
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

constexpr int NAME_SIZE = 10;
constexpr int CHAT_SIZE = 30;

constexpr int MAX_BUFSIZE_CLIENT = 1024;

enum E_PACKET
{
	E_PACKET_NONE = 0,

	// Client -> Server packet
	E_PACKET_CS_LOGIN, E_PACKET_CS_KEYDOWN, E_PACKET_CS_KEYUP, E_PACKET_CS_MOVE, E_PACKET_CS_ROTATE,

	E_PACKET_CS_TEMP_HIT_MONSTER_PACKET,
	E_PACKET_CS_TEMP_HIT_PLAYER_PACKET, 
	E_PACKET_CS_CHAT_PACKET,
	E_PACKET_CS_ROOM_CREATE_PACKET, E_PACKET_CS_ROOM_JOIN_PACKET,
	E_PACKET_CS_ROOM_READY_PACKET, E_PACKET_CS_ROOM_UNREADY_PACKET,
	E_PACKET_CS_ROOM_START_PACKET,
	E_PACKET_CS_PLAYERS_REQUEST_PACKET,
	E_PACKET_CS_JOIN_COMPLETE_PACKET,
	E_PACKET_CS_CAMERA_LOOK_PACKET,
	E_PACKET_CS_DIE_PACKET, E_PACKET_CS_CLEAR_PACKET,
	E_PACKET_CS_EAT_ITEM_PACKET,
	E_PACKET_CS_SKILL_HEAL_PACKET,
	E_PACKET_CS_SKILL_HEALTHUP_PACKET,

	// Server -> Client packet
	E_PACKET_SC_LOGIN_INFO, E_PACKET_SC_ADD_PLAYER, E_PACKET_SC_REMOVE_PLAYER,
	E_PACKET_SC_MOVE_PLAYER, E_PACKET_SC_LOOK_PLAYER, E_PACKET_SC_ANIMATION_TYPE_PLAYER,
	E_PACKET_SC_ANIMATION_TYPE_MONSTER,
	E_PACKET_SC_MOVE_MONSTER_PACKET, E_PACKET_SC_LOOK_MONSTER_PACKET,
	E_PACKET_SC_AGGRO_PLAYER_PACKET,

	E_PACKET_SC_TEMP_HIT_MONSTER_PACKET,
	E_PACKET_SC_TEMP_HIT_PLAYER_PACKET,

	E_PACKET_SC_TEMP_WANDER_MONSTER_PACKET,

	E_PACKET_SC_CHAT_PACKET,
	E_PACKET_SC_LOGIN_OK_PACKET,
	E_PACKET_SC_LOGIN_FAIL_PACKET,
	E_PACKET_SC_ROOM_CREATE_PACKET,
	E_PACKET_SC_ROOM_JOIN_FAIL_PACKET, E_PACKET_SC_ROOM_JOIN_OK_PACKET,
	E_PACKET_SC_ROOM_READY_PACKET, E_PACKET_SC_ROOM_UNREADY_PACKET,
	E_PACKET_SC_JOIN_GAME_PACKET, E_PACKET_SC_EVERYONE_JOIN_PACKET,
	E_PACKET_SC_PROJECTILE_ATTACK_PACKET, E_PACKET_SC_PLAYER_DIE_PACKET,
	E_PACKET_SC_CLEAR_PACKET,
	E_PACKET_SC_EAT_ITEM_PACKET,
	E_PACKET_SC_CREATE_ITEM_PACKET,
	E_PACKET_SC_SKILL_HEAL_PACKET,
	E_PACKET_SC_SKILL_HEALTHUP_PACKET
};

#pragma pack (push, 1)

class PACKET_HEAD
{
public:
	unsigned char size;
	unsigned char type;
};

class CS_LOGIN_PACKET : public PACKET_HEAD {
public:
	char	name[NAME_SIZE];
};

class CS_MOVE_PACKET : public PACKET_HEAD {
public:
	float	dirX, dirY, dirZ;
};

class CS_ROTATE_PACKET : public PACKET_HEAD {
public:
	float	Add_Pitch, Add_Yaw, Add_Roll;
};

class CS_KEYDOWN_PACKET : public PACKET_HEAD {
public:
	short	key;
};

class CS_KEYUP_PACKET : public PACKET_HEAD {
public:
	short	key;
};

class CS_TEMP_HIT_MONSTER_PACKET : public PACKET_HEAD {
public:
	unsigned int	monster_id;
	unsigned int	hit_damage;
};

class CS_TEMP_HIT_PLAYER_PACKET : public PACKET_HEAD {
public:
	unsigned int	player_id;
	unsigned int	hit_damage;
};

class CS_CHAT_PACKET : public PACKET_HEAD {
public:
	char    chat[CHAT_SIZE];
};

class CS_ROOM_CREATE_PACKET : public PACKET_HEAD {
public:
	char roomName[NAME_SIZE] = "TEST ROOM";
};

class CS_ROOM_JOIN_PACKET : public PACKET_HEAD {
public:
	int roomNum;
};

class CS_ROOM_READY_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
	char	name[NAME_SIZE];
	int playerType; // 0 : 마법사 , 1 : 전사
};

class CS_ROOM_UNREADY_PACKET : public PACKET_HEAD {
public:
};

class CS_PLAYERS_REQUEST_PACKET : public PACKET_HEAD {
public:
};

class CS_ROOM_START_PACKET : public PACKET_HEAD {
public:
};

class CS_JOIN_COMPLETE_PACKET : public PACKET_HEAD {
public:
};

class CS_CAMERA_LOOK_PACKET : public PACKET_HEAD {
public:
	float x;
	float y;
	float z;
};

class CS_DIE_PACKET : public PACKET_HEAD {
public:
}; 

class CS_CLEAR_PACKET : public PACKET_HEAD {
public:
	int ClearScene;
};

class CS_EAT_ITEM_PACKET : public PACKET_HEAD {
public:
	int itemID;
	int itemNum;
};

class CS_SKILL_HEAL_PACKET : public PACKET_HEAD {
public:
};

class CS_SKILL_HEALTHUP_PACKET : public PACKET_HEAD {
public:
};

class SC_SKILL_HEAL_PACKET : public PACKET_HEAD {
public:
	int player_id;
};

class SC_SKILL_HEALTHUP_PACKET : public PACKET_HEAD {
public:
	int player_id;
};

class SC_CREATE_ITEM_PACKET : public PACKET_HEAD {
public:
	float x;
	float y;
	float z;
	int itemID;
	int itemNum;
};

class SC_EAT_ITEM_PACKET : public PACKET_HEAD {
public:
	int player_id;
	int itemNum;
};

class SC_CLEAR_PACKET : public PACKET_HEAD {
public:
	int ClearScene;
};

class SC_PLAYER_DIE_PACKET : public PACKET_HEAD {
public:
	int player_id;
};

class SC_ROOM_READY_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
	char	name[NAME_SIZE];
	int playerType; // 0 : 마법사 , 1 : 전사
};

class SC_ROOM_UNREADY_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
	char	name[NAME_SIZE];
};


class SC_TEMP_HIT_MONSTER_PACKET : public PACKET_HEAD {
public:
	unsigned int	monster_id;
	int				remain_hp;
};

class SC_TEMP_HIT_PLAYER_PACKET : public PACKET_HEAD {
public:
	unsigned int	player_id;
	int				remain_hp;
	bool b_heal = false;
};

class SC_CHAT_PACKET : public PACKET_HEAD {
public:
	char    chat[CHAT_SIZE];
};

class SC_TEMP_WANDER_MONSTER_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
};

class SC_LOGIN_INFO_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
	char	name[NAME_SIZE];
	int 	maxHp;
	float	remainHp;
	float	x;
	float	y;
	float	z;
	int	clearStage;
};

class SC_LOGIN_OK_PACKET : public PACKET_HEAD {
public:
};

class SC_LOGIN_FAIL_PACKET : public PACKET_HEAD {
public:
};

class SC_ADD_PLAYER_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
	char	name[NAME_SIZE];
	int 	maxHp;
	float	remainHp;
	float	x;
	float	y;
	float	z;
	int	clearStage;
	int playerType; // 0 : 마법사 , 1 : 전사
};

class SC_REMOVE_PLAYER_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
};

class SC_MOVE_PLAYER_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
	float	x, y, z;
};

class SC_LOOK_PLAYER_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
	float	x, y, z;
};

class SC_PLAYER_ANIMATION_TYPE_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
	char Anitype;
};

class SC_MONSTER_ANIMATION_TYPE_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
	char Anitype;
};

class SC_MONSTER_FSM_STATE_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
	char state;
};

class SC_MOVE_MONSTER_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
	float	x, y, z;
};

class SC_AGGRO_PLAYER_PACKET : public PACKET_HEAD {
public:
	unsigned int	monster_id;
	unsigned int	player_id;
};

class SC_LOOK_MONSTER_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
	float	x, y, z;
};

class SC_ROOM_CREATE_PACKET : public PACKET_HEAD {
public:
	int roomNum;
	char roomName[NAME_SIZE];
	char hostName[NAME_SIZE];
	int connection_playerNum;
};

class SC_ROOM_JOIN_OK_PACKET : public PACKET_HEAD {
public:
	int id;
};

class SC_ROOM_JOIN_FAIL_PACKET : public PACKET_HEAD {
public:
};

class SC_JOIN_GAME_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
	char	name[NAME_SIZE];
	int 	maxHp;
	float	remainHp;
	float	x;
	float	y;
	float	z;
	int	clearStage;
	int playerType; // 0 : 마법사 , 1 : 전사
};

class SC_EVERYONE_JOIN_PACKET : public PACKET_HEAD {
public:
};

class SC_PROJECTILE_ATTACK_PACKET : public PACKET_HEAD {
public:
	int id;
	float x;
	float y;
	float z;
};

#pragma pack (pop)