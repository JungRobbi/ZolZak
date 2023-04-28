#pragma once
#include <list>
#include <queue>
#include <deque>
#include <atomic>
#include <concurrent_queue.h>

#include "Object.h"
#include "Terrain.h"


class Scene
{
	concurrency::concurrent_queue<Object*> creationQueue;
//	std::queue<Object*> creationQueue;
	concurrency::concurrent_queue<Object*> deletionQueue;
//	std::deque<Object*> deletionQueue;

	std::list<Object*> gameObjects;

public:
	static Scene* scene;
	static HeightMapTerrain* terrain;

protected:
	Object* CreateEmpty();

public:
	Scene();
	virtual ~Scene()
	{
		for (auto object : gameObjects)
			delete object;
		gameObjects.clear();
	}
	virtual void update();

	void PushDelete(Object* Object)
	{
	//	if (std::find(deletionQueue.unsafe_begin(), deletionQueue.unsafe_end(), Object) == deletionQueue.unsafe_end())
		deletionQueue.push(Object);
	}

	friend Object;
};
