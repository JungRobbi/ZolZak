#pragma once

#include <Windows.h>

class Time
{
	static int lastTime;
public:
	static int elapsed;

	static void start()
	{
		lastTime = GetTickCount64();
	}
	static void update()
	{
		int current = GetTickCount64();
		elapsed = current - lastTime;
		lastTime = current;
	}
};

