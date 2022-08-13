#pragma once

#include "Identifiers.h"
#include "ECS.hpp"

namespace fge
{
	class Entity
	{
	public:
		explicit Entity(ECS& ecs) : _id(ecs.get_new_id()), _ecs(&ecs) 
		{
			// register entity
		}

		template<class C, typename... Args>
		C* Add(Args&&... args)
		{
			
		}

		template<class C>
		C* Add(C&& t)
		{

		}

		EntityID get_id() const
		{
			return _id;
		}

	private:
		EntityID _id;
		ECS* _ecs;
	};
}