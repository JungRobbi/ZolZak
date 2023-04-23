#pragma once
#include "Object.h"

class Player :
    public Object
{
public:
	class RemoteClient* remoteClient;

public:
	Player();
	virtual ~Player();

	virtual void start();
	virtual void update();
};