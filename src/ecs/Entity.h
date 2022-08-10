#pragma once

#include "Identifiers.h"

class Entity
{
public:
	Entity();
	virtual ~Entity();



private:
	EntityID _id;
};