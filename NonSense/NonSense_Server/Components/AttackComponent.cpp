#include "AttackComponent.h"
#include "../Input.h"
#include "../RemoteClients/RemoteClient.h"
#include "../Player.h"
#include "../Scene.h"
#include "BoxCollideComponent.h"
#include "SphereCollideComponent.h"

void AttackComponent::Attack()
{
	AttackTimeLeft = AttackDuration + NextAttackInputTime;
	During_Attack = true;

	if (dynamic_cast<Player*>(gameObject)) {
		if (AttackRange) {
			for (auto& monster : Scene::scene->MonsterObjects)
			{
				if (AttackRange->Intersects(*monster->GetComponent<BoxCollideComponent>()->GetBoundingObject()))monster->GetHit(dynamic_cast<Player*>(gameObject)->GetAttack() * (monster->GetDefense()/(monster->GetDefense() + 100)));
			//	printf("%f -> %f = %f", dynamic_cast<Player*>(gameObject)->GetAttack(), monster->GetDefense(), dynamic_cast<Goblin*>(monster)->GetRemainHP());
			}
		}
	}
	else {
		if (AttackRange) {
			auto firstPlayer = RemoteClient::remoteClients.begin()->second->m_pPlayer;
			if (AttackRange->Intersects(*firstPlayer->GetComponent<SphereCollideComponent>()->GetBoundingObject()))
			{
				firstPlayer->GetHit(dynamic_cast<Goblin*>(gameObject)->GetAttack() * (firstPlayer->GetDefense() / (firstPlayer->GetDefense() + 100)));
				printf("%f -> %f = %f", dynamic_cast<Goblin*>(gameObject)->GetAttack(), firstPlayer->GetDefense(), firstPlayer->GetRemainHP());
			}
		}
	}

	if (!Type_ComboAttack)
	{
		/*gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, AttackCombo1_AnineSetNum);
		gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(1, AttackCombo1_AnineSetNum);
		gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(2, AttackCombo1_AnineSetNum);
		gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);*/
	}
	else
	{

		switch (type)
		{
		case Combo1:
			/*gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, AttackCombo1_AnineSetNum);
			gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(1, AttackCombo1_AnineSetNum);
			gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(2, AttackCombo1_AnineSetNum);
			gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);*/
			type = Combo2;
			break;
		case Combo2:
			/*gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, AttackCombo2_AnineSetNum);
			gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(1, AttackCombo2_AnineSetNum);
			gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(2, AttackCombo2_AnineSetNum);
			gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);*/
			type = Combo3;
			break;
		case Combo3:
			/*gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(0, AttackCombo3_AnineSetNum);
			gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(1, AttackCombo3_AnineSetNum);
			gameObject->m_pSkinnedAnimationController->SetTrackAnimationSet(2, AttackCombo3_AnineSetNum);
			gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);	*/		
			type = Combo1;
			break;
		}

	}
}

void AttackComponent::start()
{
}

void AttackComponent::update()
{
	if (AttackRange)
	{
		AttackRange->Center = XMFLOAT3(0, 0.3, 1.0);
		AttackRange->Extents = XMFLOAT3(1, 0.3, 0.5);
		AttackRange->Orientation = XMFLOAT4(0, 0, 0, 1);

		AttackRange->Transform(*AttackRange, XMLoadFloat4x4(&gameObject->GetWorld()));

		/// 그리기 위한 코드
		AttackRange->m_xmf4x4ToParent = gameObject->GetWorld();
		AttackRange->SetScale(AttackRange->Extents.x, AttackRange->Extents.y, AttackRange->Extents.z);
		AttackRange->SetPosition(AttackRange->Center.x, AttackRange->Center.y, AttackRange->Center.z);
	}

	if (dynamic_cast<Player*>(gameObject)) {
		//if (((Player*)gameObject)->m_pSkinnedAnimationController)
		//{
		//	if ((Input::InputKeyBuffer[VK_LBUTTON] & 0xF0))
		//	{
		//		if (!During_Attack)
		//		{
		//			Attack();
		//		}
		//	}
		//}
	}
	if (AttackTimeLeft > 0.0f)
	{
		if (AttackTimeLeft > 0.0)
		{
			AttackTimeLeft -= Timer::GetTimeElapsed();
		}

		if (AttackTimeLeft < NextAttackInputTime)
		{
			During_Attack = false;
		}
	}
	else
	{
		type = Combo1;
	}
}

void AttackComponent::SetAttackSpeed(float speed)
{
	AttackDuration = speed;
}


