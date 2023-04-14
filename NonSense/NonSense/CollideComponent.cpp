#include "CollideComponent.h"

void CollideComponent::start()
{

}

void CollideComponent::update()
{
	m_BoundingBox.Transform(m_BoundingBox, XMLoadFloat4x4(&gameObject->GetWorld()));
	if (m_BoundingObject)
	{
		m_BoundingObject->m_xmf4x4ToParent = gameObject->GetWorld();
		m_BoundingObject->UpdateTransform(&gameObject->GetWorld());
	}
		
}

void CollideComponent::SetBoundingObject(BoundBox* bd)
{
	m_BoundingObject = bd;
}