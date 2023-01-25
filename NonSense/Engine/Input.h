#pragma once

#include <conio.h>

enum KeyCode
{
	Space = ' ',
	Left = 0x25, Up, Right, Down,
	Alpha0 = '0', Alpha1, Alpha2, Alpha3, Alpha4, Alpha5, Alpha6, Alpha7, Alpha8, Alpha9,
	A = 'A', B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
};

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

	static bool keys[256];
	static bool keyUp[256];
	static bool keyDown[256];

	static void update();
};
