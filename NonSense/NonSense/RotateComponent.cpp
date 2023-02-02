#include "RotateComponent.h"
#include "Object.h"

void RotateComponent::start()
{
}

void RotateComponent::update()
{
	gameObject->Object::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * 0.001);
}
