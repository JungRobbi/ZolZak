#include "Input.h"

UCHAR Input::InputKeyBuffer[256];

void Input::update()
{
	::GetKeyboardState(InputKeyBuffer);
}