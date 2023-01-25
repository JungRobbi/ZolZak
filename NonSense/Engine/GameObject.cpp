#include "GameObject.h"

GameObject::GameObject() : position{ 0, 0 }
{
	Scene::scene->creationQueue.push(this);
}

GameObject::GameObject(int x, int y) : position{ x, y }
{
	Scene::scene->creationQueue.push(this);
}
