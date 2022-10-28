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

		VELOX_API constexpr EntityID GetID() const noexcept;

	public:
		VELOX_API void Destroy();

	public:
		template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
		C* AddComponent(Args&&... args);
		template<IsComponent C>
		C* AddComponent(C&& comp);

		template<IsComponent C>
		void RemoveComponent();

		template<IsComponent C>
		C& GetComponent() const;
		template<IsComponent C>
		C* TryGetComponent() const;

		template<IsComponent C>
		bool HasComponent() const;

	protected:
		EntityID		m_id; // entity is just an id
		EntityAdmin*	m_entity_admin;
	};

	template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
	inline C* Entity::AddComponent(Args&&... args)
	{
		return m_entity_admin->AddComponent<C>(m_id, std::forward<Args>(args)...);
	}

	template<IsComponent C>
	inline C* Entity::AddComponent(C&& comp)
	{
		return m_entity_admin->AddComponent<C>(m_id, std::forward<C>(comp));
	}

	template<IsComponent C>
	inline void Entity::RemoveComponent()
	{
		m_entity_admin->RemoveComponent<C>(m_id);
	}

	template<IsComponent C>
	inline C& Entity::GetComponent() const
	{
		return m_entity_admin->GetComponent<C>(m_id);
	}

	template<IsComponent C>
	inline C* Entity::TryGetComponent() const
	{
		return m_entity_admin->TryGetComponent<C>(m_id);
	}

	template<IsComponent C>
	inline bool Entity::HasComponent() const
	{
		return m_entity_admin->HasComponent<C>(m_id);
	}
}