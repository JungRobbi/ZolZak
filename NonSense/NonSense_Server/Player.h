#pragma once
#include "Object.h"

class Player :
    public Object
{
public:
	class RemoteClient* remoteClient;
public:
	E_PLAYER_ANIMATION_TYPE OldAniType;
	E_PLAYER_ANIMATION_TYPE PresentAniType;

public:
	Player();
	virtual ~Player();

	virtual void start();
	virtual void update();
};