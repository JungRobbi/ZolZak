#pragma once
#include <vector>
#include <atomic>
#include <memory>

#include "remoteClients/RemoteClient.h"
#include "Scene.h"

class Room
{
	std::vector<std::shared_ptr<RemoteClient>> Clients;
	std::shared_ptr<Scene> scene;
public: 
	std::atomic<int> m_roomNum = 0;
public:
	Room();
	~Room();

	std::shared_ptr<RemoteClient> GetClients(int index) { return Clients.at(index); }

	void start();
	void update();
};