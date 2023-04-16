#include "CollideComponent.h"

void CollideComponent::start()
{
	if (&m_BoundingBox)
	{
		m_BoundingBox.Center = gameObject->FindFirstMesh()->GetBoundingBox().Center;
		m_BoundingBox.Extents = gameObject->FindFirstMesh()->GetBoundingBox().Extents;
		m_BoundingBox.Orientation = gameObject->FindFirstMesh()->GetBoundingBox().Orientation;
		printf("¸ðµ¨ÁÂÇ¥ - %f, %f, %f\n", m_BoundingBox.Center.x, m_BoundingBox.Center.y, m_BoundingBox.Center.z);
		m_BoundingBox.Transform(m_BoundingBox, XMLoadFloat4x4(&gameObject->GetWorld()));
		printf("¿ùµåÁÂÇ¥ - %f, %f, %f\n", m_BoundingBox.Center.x, m_BoundingBox.Center.y, m_BoundingBox.Center.z);
	}
	if (m_BoundingObject)
	{
		m_BoundingObject->m_xmf4x4ToParent = gameObject->GetWorld();
		m_BoundingObject->SetScale(m_BoundingBox.Extents.x, m_BoundingBox.Extents.y, m_BoundingBox.Extents.z);
		m_BoundingObject->UpdateTransform(NULL);
		printf("±×·ÁÁö´Â ÁÂÇ¥ - %f, %f, %f\n", m_BoundingObject->GetPosition().x, m_BoundingObject->GetPosition().y, m_BoundingObject->GetPosition().z);
	}
}

void CollideComponent::update()
{
	if (MoveAble)
	{
		if (&m_BoundingBox)
		{
			//m_BoundingBox.Center = gameObject->FindFirstMesh()->GetBoundingBox().Center;
			//m_BoundingBox.Extents = gameObject->FindFirstMesh()->GetBoundingBox().Extents;
			//m_BoundingBox.Orientation = gameObject->FindFirstMesh()->GetBoundingBox().Orientation;
			m_BoundingBox.Transform(m_BoundingBox, XMLoadFloat4x4(&gameObject->GetWorld()));
			//printf("ÇÃ·¹ÀÌ¾î ÁÂÇ¥ - %f, %f, %f\n", m_BoundingBox.Center.x, m_BoundingBox.Center.y, m_BoundingBox.Center.z);
		}
		if (m_BoundingObject)
		{
			m_BoundingObject->m_xmf4x4ToParent = gameObject->GetWorld();
			m_BoundingObject->SetScale(m_BoundingBox.Extents.x, m_BoundingBox.Extents.y, m_BoundingBox.Extents.z);
			m_BoundingObject->UpdateTransform(NULL);
		}
	}
}

void CollideComponent::SetBoundingObject(BoundBox* bd)
{
	m_BoundingObject = bd;																				
}

void CollideComponent::SetCenterExtents(XMFLOAT3 ct, XMFLOAT3 ex)
{
	m_BoundingBox.Center = ct;
	m_BoundingBox.Extents = ex;
}