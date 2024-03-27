#include "Player.h"

#include "Components/PlayerMovementComponent.h"
#include "Components/SphereCollideComponent.h"
#include "Components/AttackComponent.h"
#include "RemoteClients/RemoteClient.h"
#include "Scene.h"
#include <iostream>

Player::Player() : Object(false)
{
	AddComponent<PlayerMovementComponent>();

	BoundSphere* bs = new BoundSphere();
	bs->SetNum(1);
	AddComponent<SphereCollideComponent>();
	GetComponent<SphereCollideComponent>()->SetBoundingObject(bs);
	GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.3);

	BoundBox* bb = new BoundBox();
	bb->SetNum(3);
	AddComponent<AttackComponent>();
	GetComponent<AttackComponent>()->SetBoundingObject(bb);
}

Player::~Player()
{
	for (auto& p : components)
		delete p;
	components.clear();
}

void Player::start()
{
	Object::start();
}

void Player::update()
{
	Object::update();
}
