#include "Framework.h"

Framework::Framework(Scene* scene) : scene{ scene }
{
}

Framework::~Framework()
{
	delete scene;
}

void Framework::run()
{
	//SceneMGR::setScene(scene);
	//NetworkMGR::setScene(scene);

	//SceneMGR::start();
	//NetworkMGR::start();

	while (true)
	{
		Time::start();
		while (gameState)
		{
			Time::update();
			Input::update();

			scene->update();
			
			//NetworkMGR::Tick();
			//SceneMGR::Tick();
		}

		Output::print("\nGame Over! Press A Key To Restart!\n");
		_getch();
		system("cls");
	}
}