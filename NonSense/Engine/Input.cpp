#include "Input.h"

bool Input::keys[256];
bool Input::keyUp[256];
bool Input::keyDown[256];

void Input::update()
{
	for (auto& d : keys)
		d = false;

	unsigned char input;
	if (kbhit())
	{
		switch (input = _getch())
		{
		case EXT_KEY:
			switch (input = _getch())
			{
			case LEFT:
				keys[KeyCode::Left] = true;
				break;
			case RIGHT:
				keys[KeyCode::Right] = true;
				break;
			case UP:
				keys[KeyCode::Up] = true;
				break;
			case DOWN:
				keys[KeyCode::Down] = true;
				break;
			}
		case LEFT:
		case RIGHT:
		case UP:
		case DOWN:
			break;
		default:
			keys[input] = true;
			break;
		}
	}
}