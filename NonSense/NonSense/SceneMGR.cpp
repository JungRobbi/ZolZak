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
			// �ڽ��� ID �� ���
			

			// �ߺ��� ID�� ����� ��
			
			// �� �ܿ��� Player�� �߰��Ѵ�.
		
		}
		else if (dm.msg == E_MSG_REQUEST_PLAYERS) {
			
		}
		else {

			// �ٸ� �÷��̾��� ������ ��
		


			if (dm.msg == E_MSG_POSITIONING) { // ��ġ �̵�
				
			}

		}
	}
}
