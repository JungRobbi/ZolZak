#include "Scene.h"
#include <iostream>
#include <fstream>
#include <algorithm>

std::shared_ptr<Scene> Scene::scene{};
HeightMapTerrain* Scene::terrain{ nullptr };

Scene::Scene()
{

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

	for (auto Object : gameObjects) {
		if (false == Object->activate)
			continue;
		Object->update();
	}

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

	for (auto& Monster : MonsterObjects)
		Monster->update();

	while (deletionMonsterQueue.try_pop(Monster))
	{
		delete Monster;
	}
}

void Scene::LoadSceneObb()
{
	std::cout << "Map Obb Loding..." << std::endl;
	std::ifstream in{ "NonSenseMapOBB.txt" };

	XMFLOAT3 center;
	XMFLOAT3 extents;
	XMFLOAT4 orientation;
	
	while (in >> center.x) {
		in >> center.y >> center.z;
		in >> extents.x >> extents.y >> extents.z;
		in >> orientation.x >> orientation.y >> orientation.z >> orientation.w;
		BoundBox* obb = new BoundBox();
		obb->Center = center;
		obb->Extents = extents;
		obb->Orientation = orientation;
		MapBoundingGameObjects.push_back(obb);
	}

	std::cout << "Map Obb Loding Complete!" << std::endl;
}
