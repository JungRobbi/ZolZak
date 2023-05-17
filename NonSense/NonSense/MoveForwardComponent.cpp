#include "MoveForwardComponent.h"
#include "RotateComponent.h"
void MoveForwardComponent::start()
{
}

void MoveForwardComponent::update()
{
	if (0 < MoveTimeLeft) {
		MoveTimeLeft -= Timer::GetTimeElapsed();
		XMFLOAT3 NewPos = Vector3::Add(gameObject->GetPosition(), Direction, Speed * Timer::GetTimeElapsed());
		gameObject->SetPosition(NewPos);
	}
	else
	{
		if(gameObject->GetComponent<RotateComponent>())
			gameObject->GetComponent<RotateComponent>()->RotateOn = false;
		gameObject->SetDo_Render(false);
	}

}

void MoveForwardComponent::ReadyToMove(XMFLOAT3& look)
{
	Direction = look;
	MoveTimeLeft = MoveTime;
}


