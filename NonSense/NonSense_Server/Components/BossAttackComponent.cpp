#include "BossAttackComponent.h"
#include "BossFSMComponent.h"
#include "../Input.h"
#include "../RemoteClients/RemoteClient.h"
#include "../Characters.h";
#include "../Scene.h"
#include "BoxCollideComponent.h"
#include "SphereCollideComponent.h"
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
		dynamic_cast<Monster*>(gameObject)->SetDefense(90);
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
	During_Attack = true;
	AttackTimeLeft = AttackDuration;
}

void BossAttackComponent::Attack()
{
	if (AttackRange) {
		if (AttackRange->Intersects(*gameObject->GetComponent<BossFSMComponent>()->GetTargetPlayer()->GetComponent<SphereCollideComponent>()->GetBoundingObject()))
		{
			//if (!NetworkMGR::b_isNet) {
			//	GameFramework::MainGameFramework->m_pPlayer->GetHit(dynamic_cast<Character*>(gameObject)->GetAttack() * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100)));
			//}
			//else {
			//	CS_TEMP_HIT_PLAYER_PACKET send_packet;
			//	send_packet.size = sizeof(CS_TEMP_HIT_PLAYER_PACKET);
			//	send_packet.type = E_PACKET::E_PACKET_CS_TEMP_HIT_PLAYER_PACKET;
			//	send_packet.player_id = NetworkMGR::id;
			//	send_packet.hit_damage = dynamic_cast<Character*>(gameObject)->GetAttack() * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100));
			//	PacketQueue::AddSendPacket(&send_packet);
			//}
			//printf("%f -> %f = %f", dynamic_cast<Character*>(gameObject)->GetAttack(), GameFramework::MainGameFramework->m_pPlayer->GetDefense(), GameFramework::MainGameFramework->m_pPlayer->GetRemainHP());
			////	GameFramework::MainGameFramework->m_pPlayer->GetHit(dynamic_cast<Goblin*>(gameObject)->GetAttack() * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100)));
			//GameFramework::MainGameFramework->m_pPlayer->Sight_DeBuff(5);
		}
	}
}

void BossAttackComponent::StealSenseAnimation()
{
	During_Skill = true;
}

void BossAttackComponent::StealSense()
{
	int Sense = RandomSense(dree);
	switch (Sense)
	{
	case 0:			//Sight
	//	GameFramework::MainGameFramework->m_pPlayer->Sight_DeBuff(5);
		break;
	case 1:			//Hearing
	//	GameScene::MainScene->Sound_Debuff(5);
		break;
	case 2:			//Touch
	//	GameFramework::MainGameFramework->Touch_Debuff(5);
		break;
	default:
		break;
	}

}

void BossAttackComponent::SummonAnimation()
{
	During_Skill = true;
}

void BossAttackComponent::Summon(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 Pos)
{
	static int num = 10001;
	int Type = RandomSense(dree);
	Object* temp;
	switch (Type)
	{
	case 0:			//Goblin
		temp = new Goblin(MONSTER_TYPE_CLOSE, gameObject->m_roomNum);
		temp->SetPosition(Pos.x + 2.0, Scene::terrain->GetHeight(Pos.x + 2.0, Pos.z), Pos.z);
		temp->SetNum(num++);
		temp = new Goblin(MONSTER_TYPE_FAR, gameObject->m_roomNum);
		temp->SetPosition(Pos.x + 2.0, Scene::terrain->GetHeight(Pos.x + 2.0, Pos.z + 2.0), Pos.z + 2.0);
		temp->SetNum(num++);
		temp = new Goblin(MONSTER_TYPE_RUSH, gameObject->m_roomNum);
		temp->SetPosition(Pos.x + 2.0, Scene::terrain->GetHeight(Pos.x + 2.0, Pos.z + 2.0), Pos.z - 2.0);
		temp->SetNum(num++);
		break;
	case 1:			//Orc
		temp = new Orc(MONSTER_TYPE_CLOSE, gameObject->m_roomNum);
		temp->SetPosition(Pos.x + 2.0, Scene::terrain->GetHeight(Pos.x + 2.0, Pos.z), Pos.z);
		temp->SetNum(num++);
		temp = new Orc(MONSTER_TYPE_FAR, gameObject->m_roomNum);
		temp->SetPosition(Pos.x + 2.0, Scene::terrain->GetHeight(Pos.x + 2.0, Pos.z + 2.0), Pos.z + 2.0);
		temp->SetNum(num++);
		temp = new Orc(MONSTER_TYPE_RUSH, gameObject->m_roomNum);
		temp->SetPosition(Pos.x + 2.0, Scene::terrain->GetHeight(Pos.x + 2.0, Pos.z + 2.0), Pos.z - 2.0);
		temp->SetNum(num++);
		break;
	case 2:			//Skull
		temp = new Skull(MONSTER_TYPE_CLOSE, gameObject->m_roomNum);
		temp->SetPosition(Pos.x + 2.0, Scene::terrain->GetHeight(Pos.x + 2.0, Pos.z), Pos.z);
		temp->SetNum(num++);
		temp = new Skull(MONSTER_TYPE_FAR, gameObject->m_roomNum);
		temp->SetPosition(Pos.x + 2.0, Scene::terrain->GetHeight(Pos.x + 2.0, Pos.z + 2.0), Pos.z + 2.0);
		temp->SetNum(num++);
		temp = new Skull(MONSTER_TYPE_RUSH, gameObject->m_roomNum);
		temp->SetPosition(Pos.x + 2.0, Scene::terrain->GetHeight(Pos.x + 2.0, Pos.z + 2.0), Pos.z - 2.0);
		temp->SetNum(num++);
		break;
	default:
		break;
	}
}

void BossAttackComponent::DefenceAnimation()
{
	During_Skill = true;
}

void BossAttackComponent::Defence()
{
	dynamic_cast<Character*>(gameObject)->SetDefense(120);
	DefenceTimeLeft = 10;
}

void BossAttackComponent::JumpAttackAnimation()
{
	During_Skill = true;
}

void BossAttackComponent::JumpAttack()
{
	XMFLOAT3 PlayerPos = dynamic_cast<Monster*>(gameObject)->GetComponent<BossFSMComponent>()->GetTargetPlayer()->GetPosition();
	XMFLOAT3 BossPos = gameObject->GetPosition();

	if (Vector3::Length(Vector3::Subtract(PlayerPos, BossPos)) < 5.0f)
	{
		/*if (!NetworkMGR::b_isNet) {
			GameFramework::MainGameFramework->m_pPlayer->GetHit(dynamic_cast<Character*>(gameObject)->GetAttack() * 1.5f * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100)));
		}
		else {
			CS_TEMP_HIT_PLAYER_PACKET send_packet;
			send_packet.size = sizeof(CS_TEMP_HIT_PLAYER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_CS_TEMP_HIT_PLAYER_PACKET;
			send_packet.player_id = NetworkMGR::id;
			send_packet.hit_damage = dynamic_cast<Character*>(gameObject)->GetAttack() * 1.5f * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100));
			PacketQueue::AddSendPacket(&send_packet);
		}*/
	}
}

void BossAttackComponent::TornadoAnimation()
{
	During_Skill = true;
}

void BossAttackComponent::Tornado()
{
	XMFLOAT3 PlayerPos = dynamic_cast<Monster*>(gameObject)->GetComponent<BossFSMComponent>()->GetTargetPlayer()->GetPosition();
	XMFLOAT3 BossPos = gameObject->GetPosition();

	if (Vector3::Length(Vector3::Subtract(PlayerPos, BossPos)) < 1.5f)
	{
		/*if (!NetworkMGR::b_isNet) {
			GameFramework::MainGameFramework->m_pPlayer->GetHit(dynamic_cast<Character*>(gameObject)->GetAttack() * 0.7f * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100)));
		}
		else {
			CS_TEMP_HIT_PLAYER_PACKET send_packet;
			send_packet.size = sizeof(CS_TEMP_HIT_PLAYER_PACKET);
			send_packet.type = E_PACKET::E_PACKET_CS_TEMP_HIT_PLAYER_PACKET;
			send_packet.player_id = NetworkMGR::id;
			send_packet.hit_damage = dynamic_cast<Character*>(gameObject)->GetAttack() * 0.7f * (GameFramework::MainGameFramework->m_pPlayer->GetDefense() / (GameFramework::MainGameFramework->m_pPlayer->GetDefense() + 100));
			PacketQueue::AddSendPacket(&send_packet);
		}*/
	}
}






