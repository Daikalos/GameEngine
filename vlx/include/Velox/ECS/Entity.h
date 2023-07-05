#pragma once

#include <Velox/System/Concepts.h>
#include <Velox/Config.hpp>

#include <Velox/Utility/NonCopyable.h>

#include "EntityAdmin.h"
#include "Identifiers.hpp"
#include "ComponentRef.hpp"

namespace vlx
{
	/// Essentially just a wrapper around an entity id
	/// 
	class Entity : private NonCopyable
	{
	public:
		VELOX_API Entity() = default;

		VELOX_API explicit Entity(EntityAdmin& entity_admin);
		VELOX_API explicit Entity(EntityAdmin& entity_admin, EntityID entity_id);

		VELOX_API Entity(Entity&& entity) noexcept;

		VELOX_API virtual ~Entity();

		VELOX_API Entity& operator=(Entity&& rhs) noexcept;

	public:
		VELOX_API operator EntityID() const noexcept; // allow implicit conversion
		VELOX_API NODISC EntityID GetID() const noexcept;

	public:
		VELOX_API NODISC Entity Duplicate() const;
		VELOX_API void Destroy();

	public:
		template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
		auto AddComponent(Args&&... args);

		template<class... Cs> requires IsComponents<Cs...>
		void AddComponents();

		template<class... Cs> requires IsComponents<Cs...>
		void AddComponents(std::type_identity<std::tuple<Cs...>>);

		template<IsComponent C>
		bool RemoveComponent();

		template<class... Cs> requires IsComponents<Cs...>
		bool RemoveComponents();

		template<IsComponent C>
		NODISC auto& GetComponent() const;
		template<IsComponent C>
		NODISC auto TryGetComponent() const;

		template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
		auto SetComponent(Args&&... args);
		template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
		auto TrySetComponent(Args&&... args);

		template<IsComponent C>
		NODISC auto GetComponentRef() const;

		template<class... Cs> requires IsComponents<Cs...>
		NODISC auto GetComponents() const;
		template<class... Cs> requires IsComponents<Cs...>
		NODISC auto GetComponentsRef() const;

		template<IsComponent C>
		NODISC auto HasComponent() const;

	protected:
		EntityID		m_id			{NULL_ENTITY}; // entity is just an id
		EntityAdmin*	m_entity_admin	{nullptr};
	};

	template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
	inline auto Entity::AddComponent(Args&&... args)
	{
		return m_entity_admin->AddComponent<C>(m_id, std::forward<Args>(args)...);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void Entity::AddComponents()
	{
		m_entity_admin->AddComponents<Cs...>(m_id);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void Entity::AddComponents(std::type_identity<std::tuple<Cs...>>)
	{
		m_entity_admin->AddComponents<Cs...>(m_id);
	}

	template<IsComponent C>
	inline bool Entity::RemoveComponent()
	{
		return m_entity_admin->RemoveComponent<C>(m_id);
	}

	template<class... Cs> requires IsComponents<Cs...>
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

	template<IsComponent C, typename ...Args> requires std::constructible_from<C, Args...>
	inline auto Entity::SetComponent(Args&&... args)
	{
		return m_entity_admin->SetComponent<C>(m_id, std::forward<Args>(args)...);
	}

	template<IsComponent C, typename ...Args> requires std::constructible_from<C, Args...>
	inline auto Entity::TrySetComponent(Args&&... args)
	{
		return m_entity_admin->TrySetComponent<C>(m_id, std::forward<Args>(args)...);
	}

	template<IsComponent C>
	inline auto Entity::GetComponentRef() const
	{
		return m_entity_admin->GetComponentRef<C>(m_id);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline auto Entity::GetComponents() const
	{
		return m_entity_admin->GetComponents<Cs...>(m_id);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline auto Entity::GetComponentsRef() const
	{
		return m_entity_admin->GetComponentsRef<Cs...>(m_id);
	}

	template<IsComponent C>
	inline auto Entity::HasComponent() const
	{
		return m_entity_admin->HasComponent<C>(m_id);
	}

}