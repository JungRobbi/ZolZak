#include "Characters.h"
#include "Scene.h"
#include "Room.h"
#include "Components/BoxCollideComponent.h"
#include "Components/CloseTypeFSMComponent.h"
#include "Components/AttackComponent.h"

Character::Character() :
	Object(false)
{
	Scene::scene->creationMonsterQueue.push((Character*)this);
}

Character::Character(int roomNum) :
	Object(false)
{
	Room::roomlist.at(roomNum)->GetScene()->creationMonsterQueue.push((Character*)this);
}


Character::~Character() 
{
	for (auto& p : components)
		delete p;
	components.clear();
}

Goblin::Goblin(MonsterType type, int roomNum) :
	Character(roomNum)
{
	BoundBox* bb = new BoundBox();
	BoundBox* bb2 = new BoundBox();
	switch (type)
	{
	case MONSTER_TYPE_CLOSE:
		AddComponent<CloseTypeFSMComponent>();
		AddComponent<AttackComponent>();
		GetComponent<AttackComponent>()->SetAttackSpeed(3.0f);
		GetComponent<AttackComponent>()->AttackCombo1_AnineSetNum = 4;
		GetComponent<AttackComponent>()->Type_ComboAttack = false;
		GetComponent<AttackComponent>()->SetBoundingObject(bb2);

		AddComponent<BoxCollideComponent>();
		GetComponent<BoxCollideComponent>()->SetBoundingObject(bb);
		GetComponent<BoxCollideComponent>()->SetCenterExtents(XMFLOAT3(0.0, 0.5, 0.0), XMFLOAT3(0.3, 0.5, 0.3));
		GetComponent<BoxCollideComponent>()->SetMoveAble(true);

		m_Health = 965;
		m_RemainHP = 965;
		m_Attack = 200;
		m_Defense = 90;
		break;
	case MONSTER_TYPE_FAR:
		m_Health = 675;
		m_RemainHP = 675;
		m_Attack = 180;
		m_Defense = 80;
		break;
	case MONSTER_TYPE_RUSH:
		m_Health = 1130;
		m_RemainHP = 1130;
		m_Attack = 460;
		m_Defense = 110;
		break;
	case MONSTER_TYPE_BOSS:
		m_Health = 20000;
		m_RemainHP = 20000;
		m_Attack = 200;
		m_Defense = 90;
		break;
	default:
		break;
	}
}

Goblin::~Goblin()
{
	for (auto& p : components)
		delete p;
	components.clear();
}