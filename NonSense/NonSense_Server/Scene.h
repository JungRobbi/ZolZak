#pragma once
#include <list>
#include <queue>
#include <deque>
#include <atomic>
#include <concurrent_queue.h>
#include <concurrent_vector.h>

#include "Object.h"
#include "Terrain.h"
#include "Characters.h"


class Scene
{
	concurrency::concurrent_queue<Object*> creationQueue;
//	std::queue<Object*> creationQueue;
	concurrency::concurrent_queue<Object*> deletionQueue;
//	std::deque<Object*> deletionQueue;

	concurrency::concurrent_queue<Character*> creationMonsterQueue;
	concurrency::concurrent_queue<Character*> deletionMonsterQueue;

	std::list<Object*> gameObjects;

public:
	static Scene* scene;
	static HeightMapTerrain* terrain;

	concurrency::concurrent_vector<Character*> MonsterObjects;
protected:
	Object* CreateEmpty();

public:
	Scene();
	virtual ~Scene()
	{
		for (auto object : gameObjects)
			delete object;
		gameObjects.clear();

		for (auto object : MonsterObjects)
			delete object;
		MonsterObjects.clear();
	}
	virtual void update();

	void PushDelete(Object* Object)
	{
	//	if (std::find(deletionQueue.unsafe_begin(), deletionQueue.unsafe_end(), Object) == deletionQueue.unsafe_end())
		deletionQueue.push(Object);
	}

	friend Object;
	friend Character;
};
