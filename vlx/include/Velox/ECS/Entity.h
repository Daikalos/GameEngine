#pragma once

#include <Velox/Utilities.hpp>

#include "Identifiers.hpp"
#include "EntityAdmin.h"
#include "ComponentProxy.hpp"

namespace vlx
{
	class Entity
	{
	public:
		VELOX_API explicit Entity(EntityAdmin& entity_admin, const EntityID entity_id);
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

		template<IsComponent... Cs>
		void AddComponents();

		template<IsComponent C>
		void RemoveComponent();

		template<IsComponent C>
		C& GetComponent() const;
		template<IsComponent C>
		bool TryGetComponent(C*& component) const;

		template<IsComponent C>
		ComponentProxy<C>& GetComponentProxy() const;
		template<IsComponent C>
		bool TryGetComponentProxy(ComponentProxy<C>*& component_proxy) const;

		template<IsComponent... Cs>
		ComponentSet<Cs...> GetComponents() const;

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

	template<IsComponent... Cs>
	inline void Entity::AddComponents()
	{
		m_entity_admin->AddComponents<Cs...>(m_id);
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
	inline bool Entity::TryGetComponent(C*& component) const
	{
		return m_entity_admin->TryGetComponent<C>(m_id, component);
	}

	template<IsComponent C>
	ComponentProxy<C>& Entity::GetComponentProxy() const
	{
		return m_entity_admin->GetComponentProxy<C>(m_id);
	}
	template<IsComponent C>
	bool Entity::TryGetComponentProxy(ComponentProxy<C>*& component_proxy) const
	{
		return m_entity_admin->TryGetComponentProxy<C>(m_id, component_proxy);
	}

	template<IsComponent... Cs>
	inline ComponentSet<Cs...> Entity::GetComponents() const
	{
		return m_entity_admin->GetComponents<Cs...>(m_id);
	}

	template<IsComponent C>
	inline bool Entity::HasComponent() const
	{
		return m_entity_admin->HasComponent<C>(m_id);
	}
}