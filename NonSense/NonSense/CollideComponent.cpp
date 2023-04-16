#include "CollideComponent.h"
#include "GameScene.h"

void CollideComponent::start()
{
	if (&m_BoundingBox)
	{
		if (Extents.x == 0 && Extents.y == 0 && Extents.z == 0)
		{
			m_BoundingBox.Center = gameObject->FindFirstMesh()->GetBoundingBox().Center;
			m_BoundingBox.Extents = gameObject->FindFirstMesh()->GetBoundingBox().Extents;
		}
		else
		{
			m_BoundingBox.Center = Center;
			m_BoundingBox.Extents = Extents;
		}
		m_BoundingBox.Orientation = gameObject->FindFirstMesh()->GetBoundingBox().Orientation;
		m_BoundingBox.Transform(m_BoundingBox, XMLoadFloat4x4(&gameObject->GetWorld()));
	}
	if (m_BoundingObject)
	{
		m_BoundingObject->m_xmf4x4ToParent = gameObject->GetWorld();
		m_BoundingObject->SetScale(m_BoundingBox.Extents.x, m_BoundingBox.Extents.y, m_BoundingBox.Extents.z);
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
		if (&m_BoundingBox)
		{
			if (Extents.x == 0 && Extents.y == 0 && Extents.z == 0)
			{
				m_BoundingBox.Center = gameObject->FindFirstMesh()->GetBoundingBox().Center;
				m_BoundingBox.Extents = gameObject->FindFirstMesh()->GetBoundingBox().Extents;
			}
			else
			{
				m_BoundingBox.Center = Center;
				m_BoundingBox.Extents = Extents;
			}
			m_BoundingBox.Orientation = gameObject->FindFirstMesh()->GetBoundingBox().Orientation;
			m_BoundingBox.Transform(m_BoundingBox, XMLoadFloat4x4(&gameObject->GetWorld()));
		}
		if (m_BoundingObject)
		{
			m_BoundingObject->m_xmf4x4ToParent = gameObject->GetWorld();
			m_BoundingObject->SetScale(m_BoundingBox.Extents.x, m_BoundingBox.Extents.y, m_BoundingBox.Extents.z);
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
				if (gameObject->GetComponent<CollideComponent>()->GetBoundingBox().Intersects(o->GetComponent<CollideComponent>()->GetBoundingBox()))
					printf("Ãæµ¹\n");
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