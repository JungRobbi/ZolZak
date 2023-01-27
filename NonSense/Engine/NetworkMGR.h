#pragma once
#include "Common.h"
#include "../Globals.h"

#include "Scene.h"
#include "DataMSG.h"
#include "MSGQueue.h"

class NetworkMGR
{
	static Scene* scene;


public:
	static char* SERVERIP;
	static SOCKET sock;

	static HANDLE ConnectEvent;

public:
	static void start();
	static void Tick();
	static void setScene(Scene* input)
	{
		scene = input;
	}

	static DWORD WINAPI RecvThread(LPVOID lParam);
	static DWORD WINAPI ConnectThread(LPVOID lParam); // 서버와의 연결
};