#include <algorithm>
#include <iostream>
#include "SceneMGR.h"
#include "NetworkMGR.h"
#include "Player.h"

GameScene* SceneMGR::MainScene;

void SceneMGR::start()
{
}

void SceneMGR::Tick()
{
	MSGprocessing();
}

void SceneMGR::render()
{
	MainScene->render();
}

void SceneMGR::MSGprocessing()
{
	MainScene->update();

	if (MSGQueue::RecvMSGQueue.empty())
		return; 

	while (!MSGQueue::RecvMSGQueue.empty()) {
		DataMSG dm = MSGQueue::PopFrontRecvMSG();

		if (dm.msg == E_MSG_CONNECT) {
			// 자신의 ID 일 경우
			

			// 중복된 ID가 연결될 때
			
			// 그 외에는 Player를 추가한다.
		
		}
		else if (dm.msg == E_MSG_REQUEST_PLAYERS) {
			
		}
		else {

			// 다른 플레이어의 정보일 때
		


			if (dm.msg == E_MSG_POSITIONING) { // 위치 이동
				
			}

		}
	}
}
