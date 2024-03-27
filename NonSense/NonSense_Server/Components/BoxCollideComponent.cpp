#include "../stdafx.h"
#include "BoxCollideComponent.h"

void BoxCollideComponent::start()
{
}

void BoxCollideComponent::update()
{
	if (MoveAble)
	{
		if (m_BoundingObject)
		{
			m_BoundingObject->Transform(*m_BoundingObject, XMLoadFloat4x4(&gameObject->GetWorld()));
		}
	}
}
void BoxCollideComponent::SetCenterExtents(XMFLOAT3 ct, XMFLOAT3 ex)
{
	Center = ct;
	Extents = ex;
}