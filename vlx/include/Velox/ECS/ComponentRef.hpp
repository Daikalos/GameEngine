#pragma once

#include <memory>

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
		ComponentRef(const EntityID entity_id, C* component);

	public:
		C* operator->();
		const C* operator->() const;

		C& operator*();
		const C& operator*() const;

	public:
		[[nodiscard]] C* Get();
		[[nodiscard]] const C* Get() const;

	private:
		void Update(const EntityAdmin& entity_admin, void* updated_data) override;

	private:
		C* m_component {nullptr};
	};

	template<class C>
	inline ComponentRef<C>::ComponentRef(const EntityID entity_id, C* component)
		: IComponentRef(entity_id), m_component(component) { }

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
	inline C* ComponentRef<C>::Get()
	{
		return m_component;
	}

	template<class C>
	inline const C* ComponentRef<C>::Get() const
	{
		return const_cast<ComponentRef<C>&>(*this).Get();
	}

	template<class C>
	inline void ComponentRef<C>::Update(const EntityAdmin& entity_admin, void* component_data)
	{
		m_component = static_cast<C*>(component_data);
	}
}