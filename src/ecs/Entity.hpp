#pragma once

#include "Identifiers.h"
#include "ECS.hpp"

namespace fge
{
	class Entity
	{
	public:
		explicit Entity(ECS& ecs) : m_id(ecs.GetNewId()), m_ecs(&ecs) 
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
			return m_id;
		}

	private:
		EntityID	m_id;  // entity is just an id
		ECS*		m_ecs;
	};
}