#pragma once
#include <vector>
#include <atomic>
#include <memory>
#include <string>
#include <concurrent_unordered_map.h>
#include <unordered_map>

#include "remoteClients/RemoteClient.h"
#include "Scene.h"

class Room
{
	std::shared_ptr<Scene> scene;
public:
	std::unordered_map<int, std::shared_ptr<RemoteClient>> Clients;
	static std::unordered_map<int, shared_ptr<Room>> roomlist;
	static int g_roomNum;
	std::atomic<int> m_roomNum = 0;

	std::string roomName{""};
	std::string hostName{""};
	int joinPlayerNum{};

public:
	Room();
	~Room();

	std::shared_ptr<Scene> GetScene() { return scene; }

	void start();
	void update();
};