#pragma once
#include "Scene.h"

#include "Input.h"
#include "Output.h"
#include "Time.h"

#include "GameObject.h"
#include "GameState.h"



#define MS_PER_UPDATE (1'000'000 / 60) // microsec

class Framework
{
protected:
	Scene* scene{};
	std::list<Scene*> scenes{};

public:
	Framework() { };
	Framework(Scene* scene);
	~Framework();
	virtual void run();
};
