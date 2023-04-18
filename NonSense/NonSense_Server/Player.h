#pragma once
#include "Object.h"
#include "RemoteClients/RemoteClient.h"

class Player :
    public Object
{
public:
	RemoteClient* remoteClient;

public:
	Player();
	virtual ~Player();

	virtual void start();
	virtual void update();
};