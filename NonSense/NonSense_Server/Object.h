#pragma once
#include <list>

#include "Components/Component.h"

class Object
{
protected:
	std::list<Component*> components;

public:
	Object();
	virtual ~Object()
	{
		components.clear();
	}

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
T* Object::AddComponent()
{
	auto component = new T;
	component->gameObject = this;
	components.push_back(component);
	return component;
}