#pragma once
#include "Scene.h"
#include "Output.h"
#include "MSGQueue.h"

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

