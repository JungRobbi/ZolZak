#pragma once

#include <windows.h>  
#include <stdio.h>  
#include <clocale>
#include <sqlext.h>  

#define NAME_LEN 10

constexpr wchar_t* DSN_NAME = L"mssql";

class DBMGR
{
public:
	SQLHENV henv{};
	SQLHDBC hdbc{};
	SQLHSTMT hstmt{};
	SQLRETURN retcode{};
	SQLWCHAR szName[NAME_LEN], player_id[NAME_LEN], player_password[NAME_LEN];
	SQLINTEGER player_x{};
	SQLINTEGER player_y{};
	SQLINTEGER player_hp{};
	SQLINTEGER player_level{};
	SQLINTEGER player_exp{};
	SQLINTEGER player_state{};
public:
	DBMGR();
	~DBMGR();

	void connect();
	void disconnect();

    void Get_SELECT_ALL();
    bool Get_SELECT_PLAYER(const wchar_t* id);

    void Set_INSERT_ID(const wchar_t* id, const wchar_t* password);
    void Set_UPDATE_PLAYER(const wchar_t* id, int x, int y, int hp, int level, int exp, int state);

};

wchar_t* ChartoWChar(char* chr);

