#include "PlayerMovementComponent.h"
#include "../Object.h"
#include "../Player.h"

void PlayerMovementComponent::start()
{
	scene = dynamic_cast<Scene*>(Scene::scene);
	
}

void PlayerMovementComponent::update()
{
	auto& keyboard = dynamic_cast<Player*>(gameObject)->remoteClient->m_KeyInput.keys;
	if (keyboard['W'] || keyboard['w']) {

	}
	if (keyboard['S'] || keyboard['s']) {

	}
	if (keyboard['A'] || keyboard['a']) {

	}
	if (keyboard['D'] || keyboard['d']) {

	}

}