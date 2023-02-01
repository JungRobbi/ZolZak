#include "GameObject.h"

GameObject::GameObject() //: Object()
{
	Scene::scene->creationQueue.push(this);
}

GameObject::~GameObject()
{
//	if (m_pMesh) m_pMesh->Release();
//	if (m_pMaterial) m_pMaterial->Release();
	
}
