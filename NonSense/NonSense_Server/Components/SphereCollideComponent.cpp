#include "../stdafx.h"
#include "SphereCollideComponent.h"

void SphereCollideComponent::start()
{
	if (m_BoundingObject)
	{
		m_BoundingObject->Center = Center;
		m_BoundingObject->Radius = Radius;

		m_BoundingObject->Transform(*m_BoundingObject, XMLoadFloat4x4(&gameObject->GetWorld()));

		m_BoundingObject->m_xmf4x4ToParent = gameObject->GetWorld();
		m_BoundingObject->SetScale(m_BoundingObject->Radius, m_BoundingObject->Radius, m_BoundingObject->Radius);
		m_BoundingObject->SetPosition(m_BoundingObject->GetPosition().x + Center.x, m_BoundingObject->GetPosition().y + Center.y, m_BoundingObject->GetPosition().z + Center.z);
	}
}

void SphereCollideComponent::update()
{
	if (m_BoundingObject)
	{
		m_BoundingObject->Center = Center;
		m_BoundingObject->Radius = Radius;

		m_BoundingObject->Transform(*m_BoundingObject, XMLoadFloat4x4(&gameObject->GetWorld()));

		m_BoundingObject->m_xmf4x4ToParent = gameObject->GetWorld();
		m_BoundingObject->SetScale(m_BoundingObject->Radius, m_BoundingObject->Radius, m_BoundingObject->Radius);
		m_BoundingObject->SetPosition(m_BoundingObject->GetPosition().x + Center.x, m_BoundingObject->GetPosition().y + Center.y, m_BoundingObject->GetPosition().z + Center.z);
	}
}
void SphereCollideComponent::SetCenterRadius(XMFLOAT3 ct, float rs)
{
	Center = ct;
	Radius = rs;
}