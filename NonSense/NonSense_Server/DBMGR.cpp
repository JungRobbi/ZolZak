#include <iostream>
#include <string>
#include <random>
#include <stdio.h> 
#include "DBMGR.h"


/************************************************************************
/* HandleDiagnosticRecord : display error/warning information
/*
/* Parameters:
/* hHandle ODBC handle
/* hType Type of handle (SQL_HANDLE_STMT, SQL_HANDLE_ENV, SQL_HANDLE_DBC)
/* RetCode Return code of failing command
/************************************************************************/

void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
    SQLSMALLINT iRec = 0;
    SQLINTEGER iError;
    WCHAR wszMessage[1000];
    WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
    if (RetCode == SQL_INVALID_HANDLE) {
        fwprintf(stderr, L"Invalid handle!\n");
        return;
    }
    while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
        (SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
        // Hide data truncated..
        if (wcsncmp(wszState, L"01004", 5)) {
            fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
        }
    }
}

wchar_t* ChartoWChar(char* chr)
{
    wchar_t* pWchr = NULL;
    int chrSize = MultiByteToWideChar(CP_ACP, 0, chr, -1, NULL, NULL);
    pWchr = new WCHAR[chrSize];
    MultiByteToWideChar(CP_ACP, 0, chr, strlen(chr) + 1, pWchr, chrSize);
    return pWchr;
}

//////////////////////////
//////////////////////////
//////////////////////////
wchar_t* DBMGR::DSN_NAME = L"NonSense";
volatile bool DBMGR::db_connection = false;

DBMGR::DBMGR()
{
    if (!db_connection)
        return;

    setlocale(LC_ALL, "korean");
    connect();
}

DBMGR::~DBMGR()
{
    if (!db_connection)
        return;

    disconnect();
}

void DBMGR::connect()
{
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
            retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

                retcode = SQLConnect(hdbc, DSN_NAME, SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

                if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
                    show_error(hstmt, SQL_HANDLE_STMT, retcode);
            }
        }
    }
}

void DBMGR::disconnect()
{
    SQLDisconnect(hdbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, henv);
}

bool DBMGR::Get_SELECT_PLAYER(const wchar_t* id)
{
    if (!db_connection)
        return false;

    wchar_t wstr[100] = L"EXEC [dbo].[Select_Player] ";
    wcscat(wstr, id);
    SQLLEN c_Name{};
    SQLLEN temp{};

    bool result{ false };

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    retcode = SQLExecDirect(hstmt, (SQLWCHAR*)wstr, SQL_NTS);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

        retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, szName, NAME_LEN, &c_Name);
        retcode = SQLBindCol(hstmt, 2, SQL_C_LONG, &player_Maxhp, 10, &temp);
        retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &player_hp, 10, &temp);
        retcode = SQLBindCol(hstmt, 4, SQL_C_FLOAT, &player_x, 10, &temp);
        retcode = SQLBindCol(hstmt, 5, SQL_C_FLOAT, &player_z, 10, &temp);
        retcode = SQLBindCol(hstmt, 6, SQL_C_LONG, &player_clear_stage, 10, &temp);


        retcode = SQLFetch(hstmt);
        if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
            show_error(hstmt, SQL_HANDLE_STMT, retcode);
            result = false;
        }
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
        {
            result = true;
        }
       

        SQLCancel(hstmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
    else  if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
        show_error(hstmt, SQL_HANDLE_STMT, retcode);
        result = false;
    }
    return result;
}

void DBMGR::Set_INSERT_ID(const wchar_t* id)
{    
    if (!db_connection)
        return;

    std::wstring wstr{ L"EXEC [dbo].[Insert_Player] " };
    wchar_t buf[20];

    wstr += id;
    wstr += L", ";
    wstr += L"1000, 1000, -16, 103, 0";

    wchar_t wstr_c_str[100];
    wcscpy(wstr_c_str, wstr.c_str());

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    retcode = SQLExecDirect(hstmt,
        (SQLWCHAR*)wstr_c_str
        , SQL_NTS);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        SQLCancel(hstmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
    else  if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
        show_error(hstmt, SQL_HANDLE_STMT, retcode);
}

void DBMGR::Set_UPDATE_PLAYER(const wchar_t* id, int x, int z, int hp, int Maxhp, int clear)
{
    if (!db_connection)
        return;

    std::wstring wstr{ L"EXEC [dbo].[Update_Player] " };
    wchar_t buf[20];
    ZeroMemory(buf, sizeof(buf));

    wstr += id;
    wstr += L", ";
    wcsset(buf, 0);

    _itow(Maxhp, buf, 10);
    wstr += buf;
    wstr += L", ";
    wcsset(buf, 0);

    _itow(hp, buf, 10);
    wstr += buf;
    wstr += L", ";
    wcsset(buf, 0);

    _itow(x, buf, 10);
    wstr += buf;
    wstr += L", ";
    wcsset(buf, 0);    
    
    _itow(z, buf, 10);
    wstr += buf;
    wstr += L", ";
    wcsset(buf, 0);

    _itow(clear, buf, 10);
    wstr += buf;
    wcsset(buf, 0);

    wchar_t wstr_c_str[100];
    wcscpy(wstr_c_str, wstr.c_str());

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    retcode = SQLExecDirect(hstmt,
        (SQLWCHAR*)wstr_c_str
        , SQL_NTS);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {      
        SQLCancel(hstmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
    else  if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
        show_error(hstmt, SQL_HANDLE_STMT, retcode);
}
