#pragma once
#include <string.h>

#define DIR_FORWARD 0x01
#define DIR_BACKWARD 0x02
#define DIR_LEFT 0x04
#define DIR_RIGHT 0x08
#define DIR_UP 0x10
#define DIR_DOWN 0x20

enum KeyCode
{
	Space = ' ',
	Left = 0x25, Up, Right, Down,
	Alpha0 = '0', Alpha1, Alpha2, Alpha3, Alpha4, Alpha5, Alpha6, Alpha7, Alpha8, Alpha9,
	A = 'A', B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	a = 'a', b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z
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

	char keys[300];
	char keyUp[300];
	char keyDown[300];

	Input() {
		memset(keys, 0, 300);
		memset(keyUp, 0, 300);
		memset(keyDown, 0, 300);
	}
};
