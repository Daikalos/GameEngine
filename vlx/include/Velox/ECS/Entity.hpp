#pragma once

#include <Velox/Utilities.hpp>

#include "Identifiers.hpp"
#include "EntityAdmin.h"

namespace vlx
{
	class Entity
	{
	public:
		explicit Entity(EntityAdmin& entity_admin);

		constexpr EntityID GetId() const;

	public:
		template<class C, typename... Args>
		C* Add(Args&&... args) requires std::constructible_from<C, Args...>;
		template<class C>
		C* Add(C&& t);

	private:
		EntityID		m_id;  // entity is just an id
		EntityAdmin*	m_entity_admin;
	};

	inline Entity::Entity(EntityAdmin& entity_admin)
		: m_id(entity_admin.GetNewId()), m_entity_admin(&entity_admin)
	{
		m_entity_admin->RegisterEntity(m_id);
	}

	inline constexpr EntityID Entity::GetId() const
	{
		return m_id;
	}

	template<class C, typename... Args>
	inline C* Entity::Add(Args&&... args) requires std::constructible_from<C, Args...>
	{
		return m_entity_admin->AddComponent<C>(m_id, std::forward<Args>(args)...);
	}

	template<class C>
	inline C* Entity::Add(C&& t)
	{
		return m_entity_admin->AddComponent<C>(m_id, std::forward<C>(c));
	}
}