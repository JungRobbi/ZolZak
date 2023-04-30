#include "Player.h"

#include "Components/PlayerMovementComponent.h"
#include "RemoteClients/RemoteClient.h"

#include <iostream>

Player::Player() : Object(false)
{
	AddComponent<PlayerMovementComponent>();
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
