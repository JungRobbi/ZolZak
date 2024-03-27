#pragma once
#include "Component.h"
#include "Timer.h"
#include "Characters.h";
class MoveForwardComponent : public Component
{
private:
	XMFLOAT3 Direction = { 0,0,0 };
	float Speed = 10.0f;
	float MoveTime = 1.5f;
public:
	float MoveTimeLeft = 0.0f;
	void start();
	void update();

	void ReadyToMove(XMFLOAT3& look);
};