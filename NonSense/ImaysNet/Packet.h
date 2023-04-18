#pragma once
#include "../Globals.h"
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

constexpr int NAME_SIZE = 20;

constexpr int MAX_BUFSIZE_CLIENT = 1024;

enum E_PACKET
{
	E_PACKET_NONE = 0,

	// Client -> Server packet
	E_PACKET_CS_LOGIN, E_PACKET_CS_KEYDOWN, E_PACKET_CS_KEYUP, E_PACKET_CS_MOVE,

	// Server -> Client packet
	E_PACKET_SC_LOGIN_INFO, E_PACKET_SC_ADD_PLAYER, E_PACKET_SC_REMOVE_PLAYER, E_PACKET_SC_MOVE_PLAYER
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
	char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
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
	short	x, y;
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
	unsigned long	direction;
};

#pragma pack (pop)