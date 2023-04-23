#include "Scene.h"

#include <algorithm>

Scene* Scene::scene{ nullptr };

Scene::Scene()
{
	scene = this;
}

Object* Scene::CreateEmpty()
{
	return new Object();
}

void Scene::update()
{
	while (!creationQueue.empty())
	{
		auto Object = creationQueue.front();
		Object->start();
		gameObjects.push_back(Object);
		creationQueue.pop();
	}

	for (auto Object : gameObjects)
		Object->update();

	while (!deletionQueue.empty())
	{
		auto Object = deletionQueue.front();
		gameObjects.erase(std::find(gameObjects.begin(), gameObjects.end(), Object));
		deletionQueue.pop_front();

		delete Object;
	}
}