#include "SceneMGR.h"
#include "GameScene.h"

Scene* SceneMGR::scene;

void SceneMGR::start()
{
}

void SceneMGR::Tick()
{
	MSGprocessing();
	render();
}

void SceneMGR::render()
{
	scene->render();
	Output::render();
}

void SceneMGR::MSGprocessing()
{
	if (MSGQueue::RecvMSGQueue.empty())
		return;

	while (!MSGQueue::RecvMSGQueue.empty()) {
		DataMSG dm = MSGQueue::PopFrontRecvMSG();

		//if (dm.msg == E_MSG_POSITIONING) { // 위치 이동
		//	int x;
		//	int y;
		//	memcpy(&x, dm.data, sizeof(int));
		//	memcpy(&y, &dm.data[4], sizeof(int));
		//	Vectorint2 pos{ x, y };
		//	dynamic_cast<GameScene*>(Scene::scene)->player->setPosition(pos);
		//}
	}
}
