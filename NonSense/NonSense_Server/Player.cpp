#include "Player.h"

#include "Components/PlayerMovementComponent.h"
#include "RemoteClients/RemoteClient.h"

Player::Player() : Object()
{
}

Player::~Player()
{
	components.clear();
}

void Player::start()
{
	AddComponent<PlayerMovementComponent>();
	Object::start();
}

void Player::update()
{
	Object::update();

}
