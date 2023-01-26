#pragma once

#include <Velox/Utilities.hpp>

#include "EntityAdmin.h"
#include "Identifiers.hpp"
#include "ComponentRef.hpp"

namespace vlx
{
	class Entity
	{
	public:
		VELOX_API Entity() = default;
		VELOX_API explicit Entity(EntityAdmin& entity_admin, const EntityID entity_id);
		VELOX_API explicit Entity(EntityAdmin& entity_admin);
		VELOX_API Entity(Entity&& entity) noexcept;

		VELOX_API virtual ~Entity();

		VELOX_API Entity& operator=(Entity&& rhs) noexcept;

		VELOX_API operator EntityID() const;

	public:
		VELOX_API [[nodiscard]] constexpr EntityID GetID() const noexcept;

	public:
		template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
		auto AddComponent(Args&&... args);
		template<IsComponent C>
		auto AddComponent(C&& comp);

		template<class... Cs> requires IsComponents<Cs...>
		void AddComponents();

		template<class... Cs> requires IsComponents<Cs...>
		void AddComponents(std::tuple<Cs...>&& tuple);

		template<IsComponent C>
		bool RemoveComponent();

		template<class... Cs> requires IsComponents<Cs...>
		bool RemoveComponents();

		template<IsComponent C>
		[[nodiscard]] auto& GetComponent() const;
		template<IsComponent C>
		[[nodiscard]] auto TryGetComponent() const;

		template<IsComponent C>
		[[nodiscard]] auto GetComponentRef() const;
		template<IsComponent C>
		[[nodiscard]] auto TryGetComponentRef() const;

		template<class... Cs> requires IsComponents<Cs...>
		[[nodiscard]] auto GetComponents() const;

		template<IsComponent C>
		[[nodiscard]] auto HasComponent() const;

	public:
		VELOX_API [[nodiscard]] Entity Duplicate() const;
		VELOX_API void Destroy();

	protected:
		EntityID		m_id			{NULL_ENTITY}; // entity is just an id
		EntityAdmin*	m_entity_admin	{nullptr};
	};

	template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
	inline auto Entity::AddComponent(Args&&... args)
	{
		return m_entity_admin->AddComponent<C>(m_id, std::forward<Args>(args)...);
	}

	template<IsComponent C>
	inline auto Entity::AddComponent(C&& comp)
	{
		return m_entity_admin->AddComponent<C>(m_id, std::forward<C>(comp));
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void Entity::AddComponents()
	{
		m_entity_admin->AddComponents<Cs...>(m_id);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void Entity::AddComponents(std::tuple<Cs...>&& tuple)
	{
		m_entity_admin->AddComponents(m_id, std::forward<std::tuple<Cs...>>(tuple));
	}

	template<IsComponent C>
	inline bool Entity::RemoveComponent()
	{
		return m_entity_admin->RemoveComponent<C>(m_id);
	}

	template<class ...Cs> requires IsComponents<Cs...>
	inline bool Entity::RemoveComponents()
	{
		return m_entity_admin->RemoveComponents<Cs...>(m_id);
	}

	template<IsComponent C>
	inline auto& Entity::GetComponent() const
	{
		return m_entity_admin->GetComponent<C>(m_id);
	}

	template<IsComponent C>
	inline auto Entity::TryGetComponent() const
	{
		return m_entity_admin->TryGetComponent<C>(m_id);
	}

	template<IsComponent C>
	inline auto Entity::GetComponentRef() const
	{
		return m_entity_admin->GetComponentRef<C>(m_id);
	}
	template<IsComponent C>
	inline auto Entity::TryGetComponentRef() const
	{
		return m_entity_admin->TryGetComponentRef<C>(m_id);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline auto Entity::GetComponents() const
	{
		return m_entity_admin->GetComponents<Cs...>(m_id);
	}

	template<IsComponent C>
	inline auto Entity::HasComponent() const
	{
		return m_entity_admin->HasComponent<C>(m_id);
	}
}