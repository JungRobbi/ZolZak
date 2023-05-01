#pragma once
#include <memory>
#include "stdafx.h"

class Component
{
public:
	class Object* gameObject;
public:
	virtual ~Component() {}
	virtual void start() = 0;
	virtual void update() = 0;
};
