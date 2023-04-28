#include "Scene.h"

#include <algorithm>

Scene* Scene::scene{ nullptr };
HeightMapTerrain* Scene::terrain{ nullptr };

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
		auto Object = creationQueue.unsafe_begin();
		(*Object)->start();
		gameObjects.push_back(*Object);
		creationQueue.try_pop(*Object);
	}

	for (auto Object : gameObjects)
		Object->update();

	while (!deletionQueue.empty())
	{
		auto Object = deletionQueue.unsafe_begin();
		deletionQueue.try_pop(*Object);
		delete *Object;
	}
}