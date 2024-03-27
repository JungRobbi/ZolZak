#include "Input.h"

char Input::keys[300];
char Input::keyUp[300];
char Input::keyDown[300];
UCHAR Input::InputKeyBuffer[256];

void Input::update()
{
	::GetKeyboardState(InputKeyBuffer);
}