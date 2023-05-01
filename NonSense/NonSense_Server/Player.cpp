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
