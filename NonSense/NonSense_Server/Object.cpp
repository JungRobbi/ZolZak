#include "Object.h"

Object::Object()
{
	Scene::scene->creationQueue.push(this);
}