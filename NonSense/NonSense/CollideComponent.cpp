#include "CollideComponent.h"
#include "GameScene.h"

void CollideComponent::start()
{
	if (m_BoundingObject)
	{
		if (Extents.x == 0 && Extents.y == 0 && Extents.z == 0)
		{
			m_BoundingObject->Center = gameObject->FindFirstMesh()->GetBoundingBox().Center;
			m_BoundingObject->Extents = gameObject->FindFirstMesh()->GetBoundingBox().Extents;
		}
		else
		{
			m_BoundingObject->Center = Center;
			m_BoundingObject->Extents = Extents;
		}
		m_BoundingObject->Orientation = gameObject->FindFirstMesh()->GetBoundingBox().Orientation;
		m_BoundingObject->Transform(*m_BoundingObject, XMLoadFloat4x4(&gameObject->GetWorld()));

		m_BoundingObject->m_xmf4x4ToParent = gameObject->GetWorld();
		m_BoundingObject->SetScale(m_BoundingObject->Extents.x, m_BoundingObject->Extents.y, m_BoundingObject->Extents.z);
		if (Extents.x == 0 && Extents.y == 0 && Extents.z == 0)
		{
			m_BoundingObject->SetPosition(m_BoundingObject->GetPosition().x + gameObject->FindFirstMesh()->GetBoundingBox().Center.x, m_BoundingObject->GetPosition().y + gameObject->FindFirstMesh()->GetBoundingBox().Center.y, m_BoundingObject->GetPosition().z + gameObject->FindFirstMesh()->GetBoundingBox().Center.z);
		}
		else
		{
			m_BoundingObject->SetPosition(m_BoundingObject->GetPosition().x + Center.x, m_BoundingObject->GetPosition().y + Center.y, m_BoundingObject->GetPosition().z + Center.z);
		}
	}
}

void CollideComponent::update()
{
	if (MoveAble)
	{
		if (m_BoundingObject)
		{
			if (Extents.x == 0 && Extents.y == 0 && Extents.z == 0)
			{
				m_BoundingObject->Center = gameObject->FindFirstMesh()->GetBoundingBox().Center;
				m_BoundingObject->Extents = gameObject->FindFirstMesh()->GetBoundingBox().Extents;
			}
			else
			{
				m_BoundingObject->Center = Center;
				m_BoundingObject->Extents = Extents;
			}
			m_BoundingObject->Orientation = gameObject->FindFirstMesh()->GetBoundingBox().Orientation;
			m_BoundingObject->Transform(*m_BoundingObject, XMLoadFloat4x4(&gameObject->GetWorld()));

			m_BoundingObject->m_xmf4x4ToParent = gameObject->GetWorld();
			m_BoundingObject->SetScale(m_BoundingObject->Extents.x, m_BoundingObject->Extents.y, m_BoundingObject->Extents.z);
			if (Extents.x == 0 && Extents.y == 0 && Extents.z == 0)
			{
				m_BoundingObject->SetPosition(m_BoundingObject->GetPosition().x + gameObject->FindFirstMesh()->GetBoundingBox().Center.x, m_BoundingObject->GetPosition().y + gameObject->FindFirstMesh()->GetBoundingBox().Center.y, m_BoundingObject->GetPosition().z + gameObject->FindFirstMesh()->GetBoundingBox().Center.z);
			}
			else
			{
				m_BoundingObject->SetPosition(m_BoundingObject->GetPosition().x + Center.x, m_BoundingObject->GetPosition().y + Center.y, m_BoundingObject->GetPosition().z + Center.z);
			}
		}
	}

	if (gameObject == GameScene::MainScene->m_pPlayer)
	{
		for (auto& o : GameScene::MainScene->gameObjects) {
			if (o->GetComponent<CollideComponent>()) {
				if (gameObject->GetComponent<CollideComponent>()->GetBoundingObject()->Intersects(*o->GetComponent<CollideComponent>()->GetBoundingObject()))
					printf("Ãæµ¹");
			}
		}
	}

}

void CollideComponent::SetBoundingObject(BoundBox* bd)
{
	m_BoundingObject = bd;																				
}

void CollideComponent::SetCenterExtents(XMFLOAT3 ct, XMFLOAT3 ex)
{
	Center = ct;
	Extents = ex;
}