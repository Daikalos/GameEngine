#pragma once

#include "EntityAdmin.h"
#include "Identifiers.hpp"
#include "IComponentRef.hpp"

namespace vlx
{
	/// <summary>
	///		The ComponentRef is to ensure that the component pointers remains valid even after the internal data of the ECS
	///		has been modified. This is to prevent having to write GetComponent everywhere all the time.
	/// </summary>
	template<class C>
	class ComponentRef final : public IComponentRef
	{
	public:
		ComponentRef(const EntityAdmin& entity_admin, const EntityID entity_id, C* component);
		ComponentRef(const EntityAdmin& entity_admin, const EntityID entity_id);

	public:
		C* operator->();
		const C* operator->() const;

		C& operator*();
		const C& operator*() const;

	public:
		operator bool() const noexcept;

	public:
		[[nodiscard]] C* Get();
		[[nodiscard]] const C* Get() const;

	public:
		[[nodiscard]] constexpr EntityID GetEntityID() const noexcept;
		[[nodiscard]] constexpr bool IsValid() const noexcept override;

	public:
		void Reset() override;
		void ForceUpdate() override;

	private:
		const EntityAdmin*	m_entity_admin	{nullptr};
		EntityID			m_entity_id		{NULL_ENTITY};
		C*					m_component		{nullptr};
	};

	template<class C>
	inline ComponentRef<C>::ComponentRef(const EntityAdmin& entity_admin, const EntityID entity_id, C* component)
		: m_entity_admin(&entity_admin), m_entity_id(entity_id), m_component(component) { }

	template<class C>
	inline ComponentRef<C>::ComponentRef(const EntityAdmin& entity_admin, const EntityID entity_id)
		: ComponentRef(entity_admin, entity_id, m_component) { }

	template<class C>
	inline C* ComponentRef<C>::operator->()
	{
		return Get();
	}

	template<class C>
	inline const C* ComponentRef<C>::operator->() const
	{
		return Get();
	}

	template<class C>
	inline C& ComponentRef<C>::operator*()
	{
		return *Get();
	}

	template<class C>
	inline const C& ComponentRef<C>::operator*() const
	{
		return *Get();
	}

	template<class C>
	inline ComponentRef<C>::operator bool() const noexcept
	{
		return IsValid();
	}

	template<class C>
	inline constexpr EntityID ComponentRef<C>::GetEntityID() const noexcept
	{
		return m_entity_id;
	}

	template<class C>
	inline C* ComponentRef<C>::Get()
	{
		ForceUpdate();
		return m_component;
	}

	template<class C>
	inline const C* ComponentRef<C>::Get() const
	{
		return const_cast<ComponentRef<C>&>(*this).Get();
	}

	template<class C>
	inline constexpr bool ComponentRef<C>::IsValid() const noexcept
	{
		return m_component != nullptr;
	}


	template<class C>
	inline void ComponentRef<C>::Reset()
	{
		m_component = nullptr;
	}

	template<class C>
	inline void ComponentRef<C>::ForceUpdate()
	{
		assert(m_entity_admin != nullptr && m_entity_id != NULL_ENTITY);

		if (!IsValid())
		{
			m_component = m_entity_admin->TryGetComponent<C>(m_entity_id).first;
		}
	}
}