#include "Object.h"
#include "Scene.h"

Object::Object()
{
	Scene::scene->creationQueue.push(this);
}