#pragma once
#include "Object.h"
#include "../AnimationType.h"

class Player :
    public Object
{
public:
	class RemoteClient* remoteClient;
public:
	E_PLAYER_ANIMATION_TYPE OldAniType = E_PLAYER_ANIMATION_TYPE::E_IDLE;
	E_PLAYER_ANIMATION_TYPE PresentAniType = E_PLAYER_ANIMATION_TYPE::E_IDLE;

public:
	Player();
	virtual ~Player();

	virtual void start();
	virtual void update();
};