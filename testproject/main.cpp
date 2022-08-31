#include "../Engine/FrameWork.h"

int main()
{
	Framework framework{ new Scene() };
	framework.run();
}