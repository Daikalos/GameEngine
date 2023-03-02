#pragma once

#include <memory>

#include <Velox/Config.hpp>

#include "Identifiers.hpp"

namespace vlx
{
	/// The ComponentRef is to ensure that the component pointers remains valid even after the internal data of the ECS
	///	has been modified. This is to prevent having to write GetComponent everywhere all the time.
	/// 
	template<class C>
	class ComponentRef final
	{
	public:
		ComponentRef() = default;
		ComponentRef(const EntityID entity_id, std::shared_ptr<IComponent*> component);

	public:
		C* operator->();
		const C* operator->() const;

		C& operator*();
		const C& operator*() const;

	public:
		NODISC C* Get();
		NODISC const C* Get() const;

	public:
		EntityID GetEntityID() const noexcept;
		bool IsValid() const noexcept;

	public:
		void Reset();

	private:
		EntityID						m_entity_id	{NULL_ENTITY};
		std::shared_ptr<IComponent*>	m_component {nullptr};

		template<class... Cs> requires IsComponents<Cs...>
		friend class ComponentSet; // to enable access to m_component
	};

	template<class C>
	inline ComponentRef<C>::ComponentRef(const EntityID entity_id, std::shared_ptr<IComponent*> component)
		: m_entity_id(entity_id), m_component(component) { }

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
		return static_cast<C*>(*m_component);
	}

	template<class C>
	inline const C* ComponentRef<C>::Get() const
	{
		return const_cast<ComponentRef<C>&>(*this).Get();
	}

	template<class C>
	inline EntityID ComponentRef<C>::GetEntityID() const noexcept
	{
		return m_entity_id;
	}

	template<class C>
	inline bool ComponentRef<C>::IsValid() const noexcept
	{
		return m_component != nullptr && *m_component != nullptr; // check that pointer and contents is valid
	}

	template<class C>
	inline void ComponentRef<C>::Reset()
	{
		m_component.reset();
	}
}