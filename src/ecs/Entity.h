#pragma once

#include "Identifiers.h"

namespace fge
{
	class Entity
	{
	public:
		Entity();
		virtual ~Entity();



	private:
		EntityID _id;
	};
}