#include "BillboardComponent.h"
#include "GameFramework.h"
#include "Object.h"

void BillboardComponent::start()
{

}

void BillboardComponent::update()
{
	XMFLOAT3 xmf3CameraPosition = GameFramework::MainGameFramework->m_pPlayer->GetCamera()->GetPosition();
	gameObject->SetLookAt(xmf3CameraPosition, XMFLOAT3(0.0f, 1.0f, 0.0f));
}
