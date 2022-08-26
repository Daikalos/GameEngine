#pragma once

#include "Identifiers.h"
#include "ECS.hpp"

namespace vlx
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

		EntityID GetId() const
		{
			return m_id;
		}

	private:
		EntityID	m_id;  // entity is just an id
		ECS*		m_ecs;
	};
}