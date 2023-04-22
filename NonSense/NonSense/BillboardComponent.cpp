#include "BillboardComponent.h"
#include "GameScene.h"
#include "Object.h"

void BillboardComponent::start()
{

}

void BillboardComponent::update()
{
	XMFLOAT3 xmf3CameraPosition = GameScene::MainScene->m_pPlayer->GetCamera()->GetPosition();
	gameObject->SetLookAt(xmf3CameraPosition, XMFLOAT3(0.0f, 1.0f, 0.0f));
}
