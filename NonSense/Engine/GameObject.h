#pragma once

#include <list>
#include "Component.h"
#include "Scene.h"
#include "Vectorint2.h"

enum E_LIFE_TYPE { E_LIFE_LIVE, E_LIFE_REMAIN, E_LIFE_DEAD };

class GameObject
{
	std::list<Component*> components;

public:
	Vectorint2 position;
	E_LIFE_TYPE lifeState{ E_LIFE_LIVE };

public:
	GameObject();
	GameObject(int x, int y);
	Vectorint2 getPosition() { return position; }
	E_LIFE_TYPE getLifeState() const { return lifeState; }
	void setLifeState(E_LIFE_TYPE state) { lifeState = state; }

	virtual void start()
	{
		for (auto component : components)
			component->start();
	}

	virtual void update()
	{
		for (auto component : components)
			component->update();
	}
	virtual void print() {}

	template<typename T>
	T* AddComponent();

	template<typename T>
	T* GetComponent()
	{
		for (auto component : components)
		{
			auto c = dynamic_cast<T*>(component);
			if (c) return c;
		}
		return nullptr;
	}
};

template<typename T>
T* GameObject::AddComponent()
{
	auto component = new T;
	component->gameObject = this;
	components.push_back(component);
	return component;
}
