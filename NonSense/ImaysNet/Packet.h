#pragma once
#include "../Globals.h"
#include "../AnimationType.h"
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

constexpr int NAME_SIZE = 20;

constexpr int MAX_BUFSIZE_CLIENT = 1024;

enum E_PACKET
{
	E_PACKET_NONE = 0,

	// Client -> Server packet
	E_PACKET_CS_LOGIN, E_PACKET_CS_KEYDOWN, E_PACKET_CS_KEYUP, E_PACKET_CS_MOVE, E_PACKET_CS_ROTATE,

	// Server -> Client packet
	E_PACKET_SC_LOGIN_INFO, E_PACKET_SC_ADD_PLAYER, E_PACKET_SC_REMOVE_PLAYER, 
	E_PACKET_SC_MOVE_PLAYER, E_PACKET_SC_LOOK_PLAYER

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

class SC_LOGIN_INFO_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
};

class SC_ADD_PLAYER_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
	char	name[NAME_SIZE];
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

class SC_PLAYER_CHANGE_PACKET : public PACKET_HEAD {
public:
	unsigned int	id;
	char Anitype;
};

#pragma pack (pop)