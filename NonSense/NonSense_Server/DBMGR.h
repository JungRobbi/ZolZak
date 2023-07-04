#pragma once

#include <windows.h>  
#include <clocale>
#include <sqlext.h> 
#include <wchar.h>

#define NAME_LEN 15

class DBMGR
{
	static wchar_t* DSN_NAME;
	static wchar_t* DSN_USER_ID;
	static wchar_t* DSN_USER_PASSWORD;
public:
	SQLHENV henv{};
	SQLHDBC hdbc{};
	SQLHSTMT hstmt{};
	SQLRETURN retcode{};
	SQLWCHAR szName[NAME_LEN], player_id[NAME_LEN];
	SQLFLOAT player_x{};
	SQLFLOAT player_z{};
	SQLINTEGER player_Maxhp{};
	SQLINTEGER player_hp{};
	SQLINTEGER player_clear_stage{};

	static volatile bool db_connection;
public:
	DBMGR();
	~DBMGR();

	void connect();
	void disconnect();

	void Get_SELECT_ALL();
	bool Get_SELECT_PLAYER(const wchar_t* id);

	void Set_INSERT_ID(const wchar_t* id);
	void Set_UPDATE_PLAYER(const wchar_t* id, int x, int z, int hp, int Maxhp, int clear);

};

wchar_t* ChartoWChar(char* chr);

