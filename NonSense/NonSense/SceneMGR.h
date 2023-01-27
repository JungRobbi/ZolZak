#pragma once
#include "../Engine/Scene.h"
#include "../Engine/Output.h"
#include "../Engine/MSGQueue.h"

#include "GameScene.h"

class SceneMGR
{
	static Scene* scene;

public:
	static void start();
	static void Tick();
	static void render();

	static void MSGprocessing();

	static void setScene(Scene* input)
	{
		scene = input;
	}
};

