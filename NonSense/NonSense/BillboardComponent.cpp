#include "BillboardComponent.h"
#include "GameScene.h"
#include "Object.h"

void BillboardComponent::start()
{

}

void BillboardComponent::update()
{
	XMFLOAT4X4 Billboard = Matrix4x4::Identity();
	XMFLOAT4X4 view = GameScene::MainScene->m_pPlayer->GetCamera()->GetViewMatrix();
	Billboard._11 = view._11;
	Billboard._13 = view._13;
	Billboard._31 = view._31;
	Billboard._33 = view._33;
	XMMatrixInverse(NULL, XMLoadFloat4x4(&Billboard));
	gameObject->m_xmf4x4World = Matrix4x4::Multiply(gameObject->GetWorld(), view);
}
