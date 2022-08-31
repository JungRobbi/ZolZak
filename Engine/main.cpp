#include "../Engine/FrameWork.h"

int main()
{
	Framework framework{ new Scene() }; 	// 기본 Framework esc 키 - 종료
	framework.run();
}