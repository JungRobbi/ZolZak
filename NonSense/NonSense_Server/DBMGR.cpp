#include <iostream>
#include <wchar.h>
#include <string>
#include <random>
#include "DBMGR.h"


std::default_random_engine dre;
std::uniform_int_distribution<int> uid(0, 400);

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

DBMGR::DBMGR()
{
    setlocale(LC_ALL, "korean");
    connect();
}

DBMGR::~DBMGR()
{
    disconnect();
}

void DBMGR::connect()
{
    // Allocate environment handle  
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

    // Set the ODBC version environment attribute  
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

        // Allocate connection handle  
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
            retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

            // Set login timeout to 5 seconds  
            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

                // Connect to data source  
                retcode = SQLConnect(hdbc, (SQLWCHAR*)L"mssql", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

                // Allocate statement handle  
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

void DBMGR::Get_SELECT_ALL()
{
    SQLLEN cbName{}, cb_password{}, cb_level{};
    SQLLEN cbx{}, cby{}, cbhp{};
    SQLLEN cbexp{}, cbstate{};

    wchar_t wstr[100] = L"EXEC [dbo].[Select_All]";

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    retcode = SQLExecDirect(hstmt, (SQLWCHAR*)wstr, SQL_NTS);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

        // Bind columns 1, 2, and 3  
        retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, szName, NAME_LEN, &cbName);
        retcode = SQLBindCol(hstmt, 2, SQL_C_WCHAR, player_password, NAME_LEN, &cb_password);
        retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &player_x, 10, &cbx);
        retcode = SQLBindCol(hstmt, 4, SQL_C_LONG, &player_y, 10, &cby);
        retcode = SQLBindCol(hstmt, 5, SQL_C_LONG, &player_hp, 10, &cbhp);
        retcode = SQLBindCol(hstmt, 6, SQL_C_LONG, &player_level, 10, &cb_level);
        retcode = SQLBindCol(hstmt, 7, SQL_C_LONG, &player_exp, 10, &cbexp);
        retcode = SQLBindCol(hstmt, 8, SQL_C_LONG, &player_state, 10, &cbstate);

        // Fetch and print each row of data. On an error, display a message and exit.  
        for (int i = 0; ; i++) {
            retcode = SQLFetch(hstmt);
            if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
                show_error(hstmt, SQL_HANDLE_STMT, retcode);
            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
            {
                //replace wprintf with printf
                //%S with %ls
                //warning C4477: 'wprintf' : format string '%S' requires an argument of type 'char *'
                //but variadic argument 2 has type 'SQLWCHAR *'
                //wprintf(L"%d: %S %S %S\n", i + 1, sCustID, szName, szPhone);  
                printf("%d: name: %ls password: %ls x: %d y: %d hp: %d level: %d exp: %d state: %d\n",
                    i + 1, szName, player_password, player_x, player_y, player_hp, player_level, player_exp, player_state);
            }
            else
                break;
        }

        SQLCancel(hstmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    }
    else  if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
        show_error(hstmt, SQL_HANDLE_STMT, retcode);
}

bool DBMGR::Get_SELECT_PLAYER(const wchar_t* id)
{
    wchar_t wstr[100] = L"EXEC [dbo].[Select_player] ";
    wcscat(wstr, id);
    SQLLEN cbName{}, cb_password{}, cb_level{};
    SQLLEN cbx{}, cby{}, cbhp{};
    SQLLEN cbexp{}, cbstate{};

    bool result{ false };

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    retcode = SQLExecDirect(hstmt, (SQLWCHAR*)wstr, SQL_NTS);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

        // Bind columns 1, 2, and 3  
        retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, szName, NAME_LEN, &cbName);
        retcode = SQLBindCol(hstmt, 2, SQL_C_WCHAR, player_password, NAME_LEN, &cb_password);
        retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &player_x, 10, &cbx);
        retcode = SQLBindCol(hstmt, 4, SQL_C_LONG, &player_y, 10, &cby);
        retcode = SQLBindCol(hstmt, 5, SQL_C_LONG, &player_hp, 10, &cbhp);
        retcode = SQLBindCol(hstmt, 6, SQL_C_LONG, &player_level, 10, &cb_level);
        retcode = SQLBindCol(hstmt, 7, SQL_C_LONG, &player_exp, 10, &cbexp);
        retcode = SQLBindCol(hstmt, 8, SQL_C_LONG, &player_state, 10, &cbstate);

        // Fetch and print each row of data. On an error, display a message and exit.  
        retcode = SQLFetch(hstmt);
        if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
            show_error(hstmt, SQL_HANDLE_STMT, retcode);
            result = false;
        }
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
        {
            //replace wprintf with printf
            //%S with %ls
            //warning C4477: 'wprintf' : format string '%S' requires an argument of type 'char *'
            //but variadic argument 2 has type 'SQLWCHAR *'
            //wprintf(L"%d: %S %S %S\n", i + 1, sCustID, szName, szPhone);  
            //printf("name: %ls password: %ls x: %d y: %d hp: %d level: %d exp: %d state: %d\n",
            //   szName, player_password, player_x, player_y, player_hp, player_level, player_exp, player_state);
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

void DBMGR::Set_INSERT_ID(const wchar_t* id, const wchar_t* password)
{    
    std::wstring wstr{ L"EXEC [dbo].[Insert_player] " };
    wchar_t buf[20];

    wstr += id;
    wstr += L", ";
    wstr += password;
    wstr += L", ";

    int randX = uid(dre);
    int randY = uid(dre);

    _itow(randX, buf, 10);
    wstr += buf;
    wstr += L", ";
    wcsset(buf, 0);

    _itow(randY, buf, 10);
    wstr += buf;
    wstr += L", ";
    wcsset(buf, 0);

    wstr += L"100, 1, 0, 0";

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

void DBMGR::Set_UPDATE_PLAYER(const wchar_t* id, int x, int y, int hp, int level, int exp, int state)
{
    std::wstring wstr{ L"EXEC [dbo].[Update_player] " };
    wchar_t buf[20];

    wstr += id;
    wstr += L", ";
    wcsset(buf, 0);

    _itow(x, buf, 10);
    wstr += buf;
    wstr += L", ";
    wcsset(buf, 0);

    _itow(y, buf, 10);
    wstr += buf;
    wstr += L", ";
    wcsset(buf, 0);

    _itow(hp, buf, 10);
    wstr += buf;
    wstr += L", ";
    wcsset(buf, 0);    
    
    _itow(level, buf, 10);
    wstr += buf;
    wstr += L", ";
    wcsset(buf, 0);

    _itow(exp, buf, 10);
    wstr += buf;
    wstr += L", ";
    wcsset(buf, 0);

    _itow(state, buf, 10);
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
