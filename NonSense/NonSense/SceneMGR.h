#pragma once
#include "MSGQueue.h"

#include "GameScene.h"

class SceneMGR
{

public:
	static GameScene* MainScene;
public:
	static void start();
	static void Tick();
	static void render();

	static void MSGprocessing();

	static void setScene(GameScene* input)
	{
		MainScene = input;
	}
};

