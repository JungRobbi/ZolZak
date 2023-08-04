#include "AttackComponent.h"
#include "Input.h"
#include "GameFramework.h"
#include "BoxCollideComponent.h"
#include "SphereCollideComponent.h"
#include "NetworkMGR.h"

void AttackComponent::Attack()
{
	if (!Die) {
		AttackTimeLeft = AttackDuration + NextAttackInputTime;
		During_Attack = true;
		AttackAnimate();
		if (dynamic_cast<Monster*>(gameObject))
		{
			return;
		}
		if (dynamic_cast<Player*>(gameObject)->Magical)	// Mage Player
		{
			dynamic_cast<MagePlayer*>(gameObject)->fireball->SetPosition(gameObject->GetPosition().x, gameObject->GetPosition().y + 0.5, gameObject->GetPosition().z);
			dynamic_cast<MagePlayer*>(gameObject)->fireball->Direction = dynamic_cast<Player*>(gameObject)->GetCamera()->GetLookVector();
			dynamic_cast<MagePlayer*>(gameObject)->fireball->Active = true;

			if (NetworkMGR::b_isNet) {
				CS_CAMERA_LOOK_PACKET send_packet;
				send_packet.size = sizeof(CS_CAMERA_LOOK_PACKET);
				send_packet.type = E_PACKET::E_PACKET_CS_CAMERA_LOOK_PACKET;
				send_packet.x = dynamic_cast<MagePlayer*>(gameObject)->fireball->Direction.x;
				send_packet.y = dynamic_cast<MagePlayer*>(gameObject)->fireball->Direction.y;
				send_packet.z = dynamic_cast<MagePlayer*>(gameObject)->fireball->Direction.z;
				PacketQueue::AddSendPacket(&send_packet);
			}
		}
		else if (!dynamic_cast<Player*>(gameObject)->Magical)	// Player
		{
			if (AttackRange) {
				for (auto& monster : GameScene::MainScene->MonsterObjects)
				{
					if (monster->GetRemainHP() <= 0.0f)
						continue;
					if (AttackRange->Intersects(*monster->GetComponent<SphereCollideComponent>()->GetBoundingObject())) {
						if (!NetworkMGR::b_isNet) {
							monster->GetHit(dynamic_cast<Player*>(gameObject)->GetAttack() * (monster->GetDefense() / (monster->GetDefense() + 100)));
							if (dynamic_cast<Shield*>(monster))
							{

							}
							else
							{
								monster->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_M_HIT);
							}
							monster->HitSound();
							continue;
						}
						CS_TEMP_HIT_MONSTER_PACKET send_packet;
						send_packet.size = sizeof(CS_TEMP_HIT_MONSTER_PACKET);
						send_packet.type = E_PACKET::E_PACKET_CS_TEMP_HIT_MONSTER_PACKET;
						send_packet.monster_id = monster->GetNum();
						send_packet.hit_damage = dynamic_cast<Player*>(gameObject)->GetAttack() * (monster->GetDefense() / (monster->GetDefense() + 100));
						PacketQueue::AddSendPacket(&send_packet);
					}
				}
			}
		}
	}

}

void AttackComponent::Skill()
{
	if (!Die) {
		AttackTimeLeft = AttackDuration + NextAttackInputTime;
		During_Attack = true;
		AttackAnimate();
		if (dynamic_cast<Monster*>(gameObject))
		{
			return;
		}
		if (dynamic_cast<Player*>(gameObject)->Magical)	// Mage Player
		{
			//////////////////////////// 자기 자신 힐
			dynamic_cast<Player*>(gameObject)->m_RemainHP += 100;
			if (dynamic_cast<Player*>(gameObject)->m_RemainHP > dynamic_cast<Player*>(gameObject)->m_Health)
			{
				dynamic_cast<Player*>(gameObject)->m_RemainHP = dynamic_cast<Player*>(gameObject)->m_Health;
			}

			dynamic_cast<Player*>(gameObject)->m_pHP_Dec_UI->Dec_HP = dynamic_cast<Player*>(gameObject)->m_RemainHP / 1000;
			dynamic_cast<Player*>(gameObject)->m_pOverHP_Dec_UI->Dec_HP = (dynamic_cast<Player*>(gameObject)->m_RemainHP - 1000) / 1000;
			dynamic_cast<Player*>(gameObject)->m_pHP_Dec_UI->HP = dynamic_cast<Player*>(gameObject)->m_pHP_Dec_UI->Dec_HP;
			dynamic_cast<Player*>(gameObject)->m_pOverHP_Dec_UI->HP = dynamic_cast<Player*>(gameObject)->m_pOverHP_Dec_UI->Dec_HP;
			if (dynamic_cast<Player*>(gameObject)->m_RemainHP <= 1000)
			{
				dynamic_cast<Player*>(gameObject)->m_pOverHP_Dec_UI->Dec_HP = 0;
				dynamic_cast<Player*>(gameObject)->m_pOverHP_Dec_UI->HP = 0;
			}

			if (dynamic_cast<Player*>(gameObject)->m_RemainHP >= 1000)
			{
				dynamic_cast<Player*>(gameObject)->m_pHP_Dec_UI->Dec_HP = 1;
				dynamic_cast<Player*>(gameObject)->m_pHP_Dec_UI->HP = 1;
			}

			//////////////////////////// 팀원 힐
			for (auto& p : GameFramework::MainGameFramework->m_OtherPlayers)
			{
				// 사용 플레이어와 거리가 2아래 라면
				if (sqrt(pow((dynamic_cast<Player*>(gameObject)->GetPosition().x - p->GetPosition().x), 2) + pow((dynamic_cast<Player*>(gameObject)->GetPosition().z - p->GetPosition().z), 2)) < 2)
				{
					p->m_RemainHP += 200;
					if (p->m_RemainHP > p->m_Health)
					{
						p->m_RemainHP = p->m_Health;
					}
				}
			}
			////////////////////////////
		}
		else if (!dynamic_cast<Player*>(gameObject)->Magical)	// Warrior
		{
		}
	}

}

void AttackComponent::ProjectileAttack(XMFLOAT3 dir)
{
	AttackTimeLeft = AttackDuration + NextAttackInputTime;
	During_Attack = true;
	if (dynamic_cast<MagePlayer*>(gameObject))	// Mage Player
	{
		dynamic_cast<MagePlayer*>(gameObject)->fireball->SetPosition(gameObject->GetPosition().x, gameObject->GetPosition().y + 0.5, gameObject->GetPosition().z);
		dynamic_cast<MagePlayer*>(gameObject)->fireball->Direction = dir;
		dynamic_cast<MagePlayer*>(gameObject)->fireball->Active = true;
	}
	AttackAnimate();
}

void AttackComponent::CheckMonsterAttackRange()
{
	if (AttackRange) {
			if (AttackRange->Intersects(*GameFramework::MainGameFramework->m_pPlayer->GetComponent<SphereCollideComponent>()->GetBoundingObject()))
			{
				if (!NetworkMGR::b_isNet) {
					GameFramework::MainGameFramework->m_pPlayer->GetHit(dynamic_cast<Character*>(gameObject)->GetAttack() * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100)));
				}
				else {
					CS_TEMP_HIT_PLAYER_PACKET send_packet;
					send_packet.size = sizeof(CS_TEMP_HIT_PLAYER_PACKET);
					send_packet.type = E_PACKET::E_PACKET_CS_TEMP_HIT_PLAYER_PACKET;
					send_packet.player_id = NetworkMGR::id;
					send_packet.hit_damage = dynamic_cast<Character*>(gameObject)->GetAttack() * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100));
					PacketQueue::AddSendPacket(&send_packet);
				}
				printf("%f -> %f = %f", dynamic_cast<Character*>(gameObject)->GetAttack(), GameFramework::MainGameFramework->m_pPlayer->GetDefense(), GameFramework::MainGameFramework->m_pPlayer->GetRemainHP());
			//	GameFramework::MainGameFramework->m_pPlayer->GetHit(dynamic_cast<Goblin*>(gameObject)->GetAttack() * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100)));
				if(GameFramework::MainGameFramework->GameSceneState == SIGHT_SCENE)
					GameFramework::MainGameFramework->m_pPlayer->Sight_DeBuff(5);
			}
		}
}

void AttackComponent::AttackAnimate() 
{
	if (!Type_ComboAttack)
	{
		gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(AttackCombo1_AnineSetNum);
		gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	}
	else
	{

		switch (type)
		{
		case Combo1:
			gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(AttackCombo1_AnineSetNum);
			gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
			type = Combo2;
			break;
		case Combo2:
			gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(AttackCombo2_AnineSetNum);
			gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
			type = Combo3;
			break;
		case Combo3:
			gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(AttackCombo3_AnineSetNum);
			gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
			type = Combo1;
			break;
		}

	}
}

AttackComponent::~AttackComponent()
{
	//AttackRange->Release();
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

		/// �׸��� ���� �ڵ�
		AttackRange->m_xmf4x4ToParent = gameObject->GetWorld();
		AttackRange->SetScale(AttackRange->Extents.x, AttackRange->Extents.y, AttackRange->Extents.z);
		AttackRange->SetPosition(AttackRange->Center.x, AttackRange->Center.y, AttackRange->Center.z);
	}

	if (dynamic_cast<Player*>(gameObject)) {
		if (((Player*)gameObject)->m_pSkinnedAnimationController)
		{
			if ((Input::InputKeyBuffer[VK_LBUTTON] & 0xF0) && !NetworkMGR::b_isNet)
			{
				if (!During_Attack && !ScriptMode && !OptionMode)
				{
					Attack();
				}
			}
			else {
				if (!During_Attack && b_Attack && !ScriptMode && !OptionMode)
				{
					Attack();
					b_Attack = false;
				}
			}
			if ((Input::InputKeyBuffer['E'] & 0xF0) && !NetworkMGR::b_isNet)
			{
				if (!During_Attack && !ScriptMode && !OptionMode)
				{
					Skill();
				}
			}
			else {
				if (!During_Attack && b_Attack && !ScriptMode && !OptionMode)
				{
					Skill();
					b_Attack = false;
				}
			}
		}
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


