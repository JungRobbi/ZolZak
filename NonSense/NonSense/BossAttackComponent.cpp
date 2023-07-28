#include "BossAttackComponent.h"
#include "Input.h"
#include "GameFramework.h"
#include "BoxCollideComponent.h"
#include "SphereCollideComponent.h"
#include "NetworkMGR.h"
#include <random>


std::uniform_int_distribution<int> RandomSense(0, 2);
std::uniform_int_distribution<int> RandomSummon(0, 2);
std::random_device rdd;
std::default_random_engine dree(rdd());

void BossAttackComponent::start()
{
}

void BossAttackComponent::update()
{

	if(AttackTimeLeft <= 0.0f)
	{
		During_Attack = false;
	}
	else
	{
		AttackTimeLeft -= Timer::GetTimeElapsed();
	}

	if (DefenceTimeLeft <= 0.0f)
	{
		dynamic_cast<Character*>(gameObject)->SetDefense(90);
	}
	else
	{
		DefenceTimeLeft -= Timer::GetTimeElapsed();
	}
}

void BossAttackComponent::SetAttackSpeed(float speed)
{
	AttackDuration = speed;
}

void BossAttackComponent::AttackAnimation()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_B_ATTACK);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	During_Attack = true;
	AttackTimeLeft = AttackDuration;
}

void BossAttackComponent::Attack()
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
		}
	}
}

void BossAttackComponent::StealSenseAnimation()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_B_ROAR);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	During_Skill = true;
}

void BossAttackComponent::StealSense()
{
	int Sense = RandomSense(dree);
	switch (Sense)
	{
	case 0:			//Sight
		GameFramework::MainGameFramework->m_pPlayer->Sight_DeBuff(5);
		break;
	case 1:			//Hearing
		GameScene::MainScene->Sound_Debuff(5);
		break;
	case 2:			//Touch
		GameFramework::MainGameFramework->Touch_Debuff(5);
		break;
	default:
		break;
	}

}

void BossAttackComponent::SummonAnimation()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_B_SUMMON);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	During_Skill = true;
}

void BossAttackComponent::Summon(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 Pos)
{
	static int num = 50001;
	int Type = RandomSense(dree);
	Object* temp;
	switch (Type)
	{
	case 0:			//Goblin
		temp = new Goblin(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, GameScene::MainScene->ModelMap["Goblin_Close"], NULL,NULL, MONSTER_TYPE_CLOSE);
		temp->SetPosition(Pos.x + 2.0, GameScene::MainScene->m_pTerrain->GetHeight(Pos.x + 2.0, Pos.z), Pos.z);
		temp->SetNum(num++);
		temp = new Goblin(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, GameScene::MainScene->ModelMap["Goblin_Far"], NULL, NULL, MONSTER_TYPE_FAR);
		temp->SetPosition(Pos.x + 2.0, GameScene::MainScene->m_pTerrain->GetHeight(Pos.x + 2.0, Pos.z + 2.0), Pos.z + 2.0);
		temp->SetNum(num++);
		temp = new Goblin(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, GameScene::MainScene->ModelMap["Goblin_Close"], NULL, NULL, MONSTER_TYPE_RUSH);
		temp->SetPosition(Pos.x + 2.0, GameScene::MainScene->m_pTerrain->GetHeight(Pos.x + 2.0, Pos.z + 2.0), Pos.z - 2.0);
		temp->SetNum(num++);
		break;
	case 1:			//Orc
		temp = new Orc(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, GameScene::MainScene->ModelMap["Orc_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
		temp->SetPosition(Pos.x + 2.0, GameScene::MainScene->m_pTerrain->GetHeight(Pos.x + 2.0, Pos.z), Pos.z);
		temp->SetNum(num++);
		temp = new Orc(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, GameScene::MainScene->ModelMap["Orc_Far"], NULL, NULL, MONSTER_TYPE_FAR);
		temp->SetPosition(Pos.x + 2.0, GameScene::MainScene->m_pTerrain->GetHeight(Pos.x + 2.0, Pos.z + 2.0), Pos.z + 2.0);
		temp->SetNum(num++);
		temp = new Orc(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, GameScene::MainScene->ModelMap["Orc_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
		temp->SetPosition(Pos.x + 2.0, GameScene::MainScene->m_pTerrain->GetHeight(Pos.x + 2.0, Pos.z + 2.0), Pos.z - 2.0);
		temp->SetNum(num++);
		break;
	case 2:			//Skull
		temp = new Skull(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, GameScene::MainScene->ModelMap["Skull_Close"], NULL, NULL, MONSTER_TYPE_CLOSE);
		temp->SetPosition(Pos.x + 2.0, GameScene::MainScene->m_pTerrain->GetHeight(Pos.x + 2.0, Pos.z), Pos.z);
		temp->SetNum(num++);
		temp = new Skull(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, GameScene::MainScene->ModelMap["Skull_Far"], NULL, NULL, MONSTER_TYPE_FAR);
		temp->SetPosition(Pos.x + 2.0, GameScene::MainScene->m_pTerrain->GetHeight(Pos.x + 2.0, Pos.z + 2.0), Pos.z + 2.0);
		temp->SetNum(num++);
		temp = new Skull(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, GameScene::MainScene->ModelMap["Skull_Rush"], NULL, NULL, MONSTER_TYPE_RUSH);
		temp->SetPosition(Pos.x + 2.0, GameScene::MainScene->m_pTerrain->GetHeight(Pos.x + 2.0, Pos.z + 2.0), Pos.z - 2.0);
		temp->SetNum(num++);
		break;
	default:
		break;
	}
}

void BossAttackComponent::DefenceAnimation()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_B_DEFENCE);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	During_Skill = true;
}

void BossAttackComponent::Defence()
{
	dynamic_cast<Character*>(gameObject)->SetDefense(120);
	DefenceTimeLeft = 10;
}

void BossAttackComponent::JumpAttackAnimation()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_B_JUMPATTACK);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	During_Skill = true;
}

void BossAttackComponent::JumpAttack()
{
	XMFLOAT3 PlayerPos = GameFramework::MainGameFramework->m_pPlayer->GetPosition();
	XMFLOAT3 BossPos = gameObject->GetPosition();

	if (Vector3::Length(Vector3::Subtract(PlayerPos, BossPos)) < 5.0f)
	{
		if (!NetworkMGR::b_isNet) {
			GameFramework::MainGameFramework->m_pPlayer->GetHit(dynamic_cast<Character*>(gameObject)->GetAttack() * 1.5f * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100)));
		}
		else {
			CS_TEMP_HIT_PLAYER_PACKET send_packet;
			send_packet.size = sizeof(CS_TEMP_HIT_PLAYER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_CS_TEMP_HIT_PLAYER_PACKET;
			send_packet.player_id = NetworkMGR::id;
			send_packet.hit_damage = dynamic_cast<Character*>(gameObject)->GetAttack() * 1.5f * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100));
			PacketQueue::AddSendPacket(&send_packet);
		}
	}


}

void BossAttackComponent::TornadoAnimation()
{
	gameObject->m_pSkinnedAnimationController->ChangeAnimationWithoutBlending(E_B_TORNADO);
	gameObject->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	During_Skill = true;
}

void BossAttackComponent::Tornado()
{
	XMFLOAT3 PlayerPos = GameFramework::MainGameFramework->m_pPlayer->GetPosition();
	XMFLOAT3 BossPos = gameObject->GetPosition();

	if (Vector3::Length(Vector3::Subtract(PlayerPos, BossPos)) <= 4.0f)
	{
		if (!NetworkMGR::b_isNet) {
			GameFramework::MainGameFramework->m_pPlayer->GetHit(dynamic_cast<Character*>(gameObject)->GetAttack() * 0.7f * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100)));
		}
		else {
			CS_TEMP_HIT_PLAYER_PACKET send_packet;
			send_packet.size = sizeof(CS_TEMP_HIT_PLAYER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_CS_TEMP_HIT_PLAYER_PACKET;
			send_packet.player_id = NetworkMGR::id;
			send_packet.hit_damage = dynamic_cast<Character*>(gameObject)->GetAttack() * 0.7f * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100));
			PacketQueue::AddSendPacket(&send_packet);
		}
	}
}






