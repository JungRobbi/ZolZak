#include "Transform3DComponent.h"


void Transform3DComponent::start()
{
}

void Transform3DComponent::update()
{
	Vector3::Add(position, velocity);
	gameObject->SetPosition(position);
	gameObject->Rotate(pitch, yaw, roll);
	gameObject->SetScale(scale.x, scale.y, scale.z);
}