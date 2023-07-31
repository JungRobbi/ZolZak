#pragma once
#include <vector>
#include <list>
#include <atomic>
#include <memory>
#include <string>
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>
#include <unordered_map>

#include "remoteClients/RemoteClient.h"
#include "Scene.h"

class Room
{
	std::shared_ptr<Scene> scene;
public:
	concurrency::concurrent_unordered_map<int, std::shared_ptr<RemoteClient>> Clients;
	static concurrency::concurrent_unordered_map<int, shared_ptr<Room>> roomlist;
	static int g_roomNum;
	std::atomic<int> m_roomNum = 0;
	int Ready_cnt = 0;
	bool b_Accessible = true;
	std::atomic<bool> clear = false;

	std::string roomName{""};
	std::string hostName{""};
	int joinPlayerNum{};

public:
	Room();
	~Room();

	std::shared_ptr<Scene> GetScene() { return scene; }

	void start();
	void update();

	void CreateHearing();
	void CreateTouch();
	void CreateBoss();

	void DeleteScene()
	{
		scene.reset();
	}
};