#include "BossAttackComponent.h"
#include "Input.h"
#include "GameFramework.h"
#include "BoxCollideComponent.h"
#include "SphereCollideComponent.h"
#include "NetworkMGR.h"
void BossAttackComponent::start()
{
}

void BossAttackComponent::update()
{
}

void BossAttackComponent::SetAttackSpeed(float speed)
{
	AttackDuration = speed;
}

void BossAttackComponent::AttackAnimation()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_B_ATTACK);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
}

void BossAttackComponent::Attack()
{
	if (AttackRange) {
		if (AttackRange->Intersects(*GameFramework::MainGameFramework->m_pPlayer->GetComponent<SphereCollideComponent>()->GetBoundingObject()))
		{
			if (!NetworkMGR::b_isNet) {
				GameFramework::MainGameFramework->m_pPlayer->GetHit(dynamic_cast<Goblin*>(gameObject)->GetAttack() * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100)));
			}
			else {
				CS_TEMP_HIT_PLAYER_PACKET send_packet;
				send_packet.size = sizeof(CS_TEMP_HIT_PLAYER_PACKET);
				send_packet.type = E_PACKET::E_PACKET_CS_TEMP_HIT_PLAYER_PACKET;
				send_packet.player_id = NetworkMGR::id;
				send_packet.hit_damage = dynamic_cast<Goblin*>(gameObject)->GetAttack() * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100));
				PacketQueue::AddSendPacket(&send_packet);
			}
			printf("%f -> %f = %f", dynamic_cast<Goblin*>(gameObject)->GetAttack(), GameFramework::MainGameFramework->m_pPlayer->GetDefense(), GameFramework::MainGameFramework->m_pPlayer->GetRemainHP());
			//	GameFramework::MainGameFramework->m_pPlayer->GetHit(dynamic_cast<Goblin*>(gameObject)->GetAttack() * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100)));
			GameFramework::MainGameFramework->m_pPlayer->Sight_DeBuff(5);
		}
	}
}

void BossAttackComponent::StealSenseAnimation()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_B_ROAR);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
}

void BossAttackComponent::StealSense()
{
}

void BossAttackComponent::SummonAnimation()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_B_SUMMON);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
}

void BossAttackComponent::Summon()
{
}

void BossAttackComponent::DefenceAnimation()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_B_DEFENCE);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
}

void BossAttackComponent::Defence()
{
}

void BossAttackComponent::JumpAttackAnimation()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_B_JUMPATTACK);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
}

void BossAttackComponent::JumpAttack()
{
}

void BossAttackComponent::TornadoAnimation()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_B_TORNADO);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
}

void BossAttackComponent::Tornado()
{
}






