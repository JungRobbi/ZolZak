#include "GameObject.h"

GameObject::GameObject() //: Object()
{
	Scene::scene->creationQueue.push(this);
}

GameObject::~GameObject()
{
	
}
