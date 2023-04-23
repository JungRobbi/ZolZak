#pragma once
#include <list>
#include <memory>
#include "Components/Component.h"

class Object
{
protected:
	std::list<std::shared_ptr<Component>> components;

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
	std::shared_ptr<T> AddComponent();

	template<typename T>
	std::shared_ptr<T> GetComponent()
	{
		for (auto component : components)
		{
			std::shared_ptr<T> c = std::dynamic_pointer_cast<T>(component);
			if (c) 
				return c;
		}
		return nullptr;
	}
};

template<typename T>
std::shared_ptr<T> Object::AddComponent()
{
	auto component = std::make_shared<T>();
	component->gameObject = this;
	components.push_back(component);
	return component;
}