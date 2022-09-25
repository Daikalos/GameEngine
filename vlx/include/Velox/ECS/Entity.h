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
		VELOX_API Entity(Entity&& entity) noexcept;

		VELOX_API virtual ~Entity();

		VELOX_API constexpr EntityID GetId() const;

	public:
		template<IsComponentType C, typename... Args>
		C* Add(Args&&... args) requires std::constructible_from<C, Args...>;
		template<IsComponentType C>
		C* Add(C&& t);

		template<IsComponentType C>
		void Remove();

		template<IsComponentType C>
		C* Get();

		template<IsComponentType C>
		bool Has();

	protected:
		EntityID		m_id; // entity is just an id
		EntityAdmin*	m_entity_admin;
	};

	template<IsComponentType C, typename... Args>
	inline C* Entity::Add(Args&&... args) requires std::constructible_from<C, Args...>
	{
		return m_entity_admin->AddComponent<C>(m_id, std::forward<Args>(args)...);
	}

	template<IsComponentType C>
	inline C* Entity::Add(C&& t)
	{
		return m_entity_admin->AddComponent<C>(m_id, std::forward<C>(t));
	}

	template<IsComponentType C>
	inline void Entity::Remove()
	{
		m_entity_admin->RemoveComponent<C>(m_id);
	}

	template<IsComponentType C>
	inline C* Entity::Get()
	{
		return m_entity_admin->GetComponent<C>(m_id);
	}

	template<IsComponentType C>
	inline bool  Entity::Has()
	{
		return m_entity_admin->HasComponent<C>(m_id);
	}
}