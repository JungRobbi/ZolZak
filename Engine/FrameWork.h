#pragma once
#include "Scene.h"

#define MS_PER_UPDATE (1'000'000 / 60) // microsec

class Framework
{
	Scene* scene;
	std::list<Scene*> scenes;

	void render() {}
public:
	Framework(Scene* scene) : scene{ scene } {}
	void run();
};
