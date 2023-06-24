#include "RotateComponent.h"
#include "Object.h"
#include "Timer.h"

void RotateComponent::start()
{
}

void RotateComponent::update()
{
	if(RotateOn)
		gameObject->Object::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * Timer::GetTimeElapsed());
}
