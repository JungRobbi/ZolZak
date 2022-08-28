#pragma once

#include <stdio.h>
#include <memory.h>
#include <Windows.h>

//const int OUTPUT_WIDTH{ 15 };
//const int OUTPUT_HEIGHT{ 13 };

class Output
{
	//static char screen[OUTPUT_HEIGHT][OUTPUT_WIDTH * 2 + 1];

public:
	static void draw(int x, int y, char const* const str, int size = 2)
	{
		//memcpy(&screen[y][x * 2], str, size);
	}
	static void render()
	{
		/*for (int y = 0; y < OUTPUT_HEIGHT; ++y)
			Output::draw(OUTPUT_WIDTH, y, "\n", 1);
		Output::draw(OUTPUT_WIDTH, OUTPUT_HEIGHT - 1, "\0", 1);

		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });
		printf("%s", (char*)screen);*/
	}
	static void print(const char* string)
	{
		printf("%s", (char*)string);
	}
};
