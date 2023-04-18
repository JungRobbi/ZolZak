#pragma once
#include <list>
#include <queue>
#include <deque>

#include "Object.h"

class Scene
{
	std::queue<Object*> creationQueue;
	std::deque<Object*> deletionQueue;

	std::list<Object*> gameObjects;

public:
	static Scene* scene;

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
		if (std::find(deletionQueue.begin(), deletionQueue.end(), Object) == deletionQueue.end());
		deletionQueue.push_back(Object);
	}

	friend Object;
};
