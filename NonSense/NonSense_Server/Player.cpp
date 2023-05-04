#include "Player.h"

#include "Components/PlayerMovementComponent.h"
#include "Components/SphereCollideComponent.h"
#include "RemoteClients/RemoteClient.h"

#include <iostream>

Player::Player() : Object(false)
{
	AddComponent<PlayerMovementComponent>();

	AddComponent<SphereCollideComponent>();
	GetComponent<SphereCollideComponent>()->SetBoundingObject(new BoundSphere());
	GetComponent<SphereCollideComponent>()->SetCenterRadius(XMFLOAT3(0.0, 0.5, 0.0), 0.3);
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
