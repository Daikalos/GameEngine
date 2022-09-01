#pragma once

#include <Velox/Utilities.hpp>

#include "Identifiers.hpp"
#include "EntityAdmin.h"

namespace vlx
{
	class Entity
	{
	public:
		VELOX_API explicit Entity(EntityAdmin& entity_admin);

		VELOX_API constexpr EntityID GetId() const;

	public:
		template<class C, typename... Args>
		C* Add(Args&&... args) requires std::constructible_from<C, Args...>;
		template<class C>
		C* Add(C&& t);

		template<class C>
		void Remove();

	private:
		EntityID		m_id;  // entity is just an id
		EntityAdmin*	m_entity_admin;
	};

	template<class C, typename... Args>
	inline C* Entity::Add(Args&&... args) requires std::constructible_from<C, Args...>
	{
		return m_entity_admin->AddComponent<C>(m_id, std::forward<Args>(args)...);
	}

	template<class C>
	inline C* Entity::Add(C&& t)
	{
		return m_entity_admin->AddComponent<C>(m_id, std::forward<C>(t));
	}

	template<class C>
	inline void Entity::Remove()
	{
		m_entity_admin->RemoveComponent<C>(m_id);
	}
}