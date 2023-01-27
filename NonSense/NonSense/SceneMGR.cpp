#include "SceneMGR.h"

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
}

void SceneMGR::MSGprocessing()
{
	scene->update();

	if (MSGQueue::RecvMSGQueue.empty())
		return;

	while (!MSGQueue::RecvMSGQueue.empty()) {
		DataMSG dm = MSGQueue::PopFrontRecvMSG();

		if (dm.msg == E_MSG_POSITIONING) { // 위치 이동
			
		}
	}
}
