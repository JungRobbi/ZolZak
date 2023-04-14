#include "CollideComponent.h"

void CollideComponent::start()
{
	m_BoundingBox.Center = XMFLOAT3(0.f, 0.f, 0.f);
	m_BoundingBox.Extents = XMFLOAT3(0.f, 0.f, 0.f);
	m_BoundingObject = NULL;
}

void CollideComponent::update()
{
	m_BoundingBox.Center = Vector3::Add(gameObject->GetPosition(), XMFLOAT3(0,m_BoundingBox.Extents.y,0));
	m_BoundingObject->SetPosition(m_BoundingBox.Center);
}
void CollideComponent::SetExtents(XMFLOAT3 extents)
{
	m_BoundingBox.Extents = extents;
	m_BoundingObject->SetScale(extents.x, extents.y, extents.z);
}