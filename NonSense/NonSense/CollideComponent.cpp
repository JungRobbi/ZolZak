#include "CollideComponent.h"

void CollideComponent::start()
{

}

void CollideComponent::update()
{
	if (&m_BoundingBox)
	{
		m_BoundingBox.Transform(m_BoundingBox, XMLoadFloat4x4(&gameObject->GetWorld()));
		m_BoundingBox.Center = gameObject->FindFirstMesh()->GetBoundingBox().Center;
		m_BoundingBox.Extents = gameObject->FindFirstMesh()->GetBoundingBox().Extents;
	}
	if (m_BoundingObject)
	{
		m_BoundingObject->m_xmf4x4ToParent = gameObject->GetWorld();
		m_BoundingObject->UpdateTransform(&gameObject->GetWorld());
		m_BoundingObject->SetScale(m_BoundingBox.Extents.x, m_BoundingBox.Extents.y, m_BoundingBox.Extents.z);
	}
		
}

void CollideComponent::SetBoundingObject(BoundBox* bd)
{
	m_BoundingObject = bd;
}

void CollideComponent::SetExtents(XMFLOAT3 ex)
{
	m_BoundingObject->SetScale(ex.x,ex.y,ex.z);
}