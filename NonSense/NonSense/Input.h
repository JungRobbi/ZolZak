#pragma once
#include <conio.h>
#include "stdafx.h"

class Input
{
public:
	enum KEY_CODE
	{
		SPACE = 32, EXT_KEY = 0xE0
	};

	enum KEY_CODE_EXT
	{
		F1 = 59, F2, F3, F4, F5, F6, F7, F8, F9, F10,
		F11 = 133, F12,
		UP = 72, DOWN = 80, LEFT = 75, RIGHT = 77
	};

	static char keys[300];
	static char keyUp[300];
	static char keyDown[300];
};
	static UCHAR InputKeyBuffer[256];
	static void update();
};