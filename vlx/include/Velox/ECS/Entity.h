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
		template<IsComponentType C, typename... Args>
		C* AddComponent(Args&&... args) requires std::constructible_from<C, Args...>;
		template<IsComponentType C>
		C* AddComponent(C&& t);

		template<IsComponentType C>
		void RemoveComponent();

		template<IsComponentType C>
		C& GetComponent() const;

		template<IsComponentType C>
		C* TryGetComponent() const;

		template<IsComponentType C>
		bool HasComponent() const;

	protected:
		EntityID		m_id; // entity is just an id
		EntityAdmin*	m_entity_admin;
	};

	template<IsComponentType C, typename... Args>
	inline C* Entity::AddComponent(Args&&... args) requires std::constructible_from<C, Args...>
	{
		return m_entity_admin->AddComponent<C>(m_id, std::forward<Args>(args)...);
	}

	template<IsComponentType C>
	inline C* Entity::AddComponent(C&& t)
	{
		return m_entity_admin->AddComponent<C>(m_id, std::forward<C>(t));
	}

	template<IsComponentType C>
	inline void Entity::RemoveComponent()
	{
		m_entity_admin->RemoveComponent<C>(m_id);
	}

	template<IsComponentType C>
	inline C& Entity::GetComponent() const
	{
		return m_entity_admin->GetComponent<C>(m_id);
	}

	template<IsComponentType C>
	inline C* Entity::TryGetComponent() const
	{
		return m_entity_admin->TryGetComponent<C>(m_id);
	}

	template<IsComponentType C>
	inline bool Entity::HasComponent() const
	{
		return m_entity_admin->HasComponent<C>(m_id);
	}
}