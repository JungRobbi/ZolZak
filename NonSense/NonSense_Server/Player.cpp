#include "Player.h"

#include "Components/PlayerMovementComponent.h"
#include "RemoteClients/RemoteClient.h"

Player::Player() : Object()
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
