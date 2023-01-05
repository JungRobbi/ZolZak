#include "stdafx.h"
#include "Framework.h"
#include "Time.h"

void Framework::run()
{
	Time::initialize();
	while (true) {
		Time::update();

		while (Time::lag >= MS_PER_UPDATE) {
			scene->update();
			Time::lag -= MS_PER_UPDATE;
		}

		render();
	}
}