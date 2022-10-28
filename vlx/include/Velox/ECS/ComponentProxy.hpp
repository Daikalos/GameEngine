#pragma once

#include <Velox/Utilities.hpp>

#include "Identifiers.hpp"

namespace vlx
{
	class EntityAdmin;

	class IComponentProxy
	{
	public:
		virtual ~IComponentProxy() = default;
		virtual void Reset() = 0;
	};

	/// <summary>
	/// The ComponentProxy is to ensure that the component pointers remains valid even after the internal data of the ECS
	/// has been modified. This is to prevent having to write GetComponent everywhere all the time.
	/// </summary>
	template<IsComponent C>
	class ComponentProxy final : public IComponentProxy
	{
	public:
		ComponentProxy(const EntityAdmin& entity_admin, const EntityID entity_id);

	public:
		void Reset() override;

		C* Get();
		const C* Get() const;

		C* operator->();
		const C* operator->() const;

	private:
		const EntityAdmin*	m_entity_admin{ nullptr };
		EntityID			m_entity_id{ NULL_ENTITY };

		C*					m_component {nullptr};
	};

	template<IsComponent C>
	inline ComponentProxy<C>::ComponentProxy(const EntityAdmin& entity_admin, const EntityID entity_id)
		: m_entity_admin(&entity_admin), m_entity_id(entity_id) { }

	template<IsComponent C>
	inline void ComponentProxy<C>::Reset()
	{
		m_component = nullptr;
	}

	template<IsComponent C>
	inline C* ComponentProxy<C>::Get()
	{
		assert(m_entity_admin != nullptr && m_entity_id != NULL_ENTITY);

		if (m_component == nullptr)
		{
			if (!m_entity_admin->TryGetComponent<C>(m_entity_id, m_component))
				throw std::runtime_error("the entity does not exist or does not have this component");
		}

		return m_component;
	}

	template<IsComponent C>
	inline const C* ComponentProxy<C>::Get() const
	{
		return const_cast<ComponentProxy<C>&>(*this).Get();
	}

	template<IsComponent C>
	inline C* ComponentProxy<C>::operator->()
	{
		return Get();
	}

	template<IsComponent C>
	inline const C* ComponentProxy<C>::operator->() const
	{
		return Get();
	}
}