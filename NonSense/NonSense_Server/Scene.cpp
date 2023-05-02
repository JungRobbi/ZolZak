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
	Object* object;
	while (creationQueue.try_pop(object))
	{
		object->start();
		gameObjects.push_back(object);
	}

	for (auto Object : gameObjects)
		Object->update();

	while (deletionQueue.try_pop(object))
	{
		delete object;
	}

	Character* Monster;
	while (creationMonsterQueue.try_pop(Monster))
	{
		Monster->start();
		MonsterObjects.push_back(Monster);
	}

	for (auto Monster : gameObjects)
		Monster->update();

	while (deletionMonsterQueue.try_pop(Monster))
	{
		delete Monster;
	}
}