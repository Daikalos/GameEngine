#pragma once

#include <memory>

#include <Velox/Config.hpp>

#include "Identifiers.hpp"

namespace vlx
{
	/// The ComponentRef is to ensure that the component pointers remains valid even after the internal data of the ECS
	/// has been modified. This is to prevent having to write GetComponent everywhere all the time.
	/// 
	/// Basically just a wrapper around a shared pointer
	/// 
	template<class C>
	class ComponentRef final
	{
	public:
		ComponentRef() = default;
		ComponentRef(std::shared_ptr<void*> component);

	public:
		C* operator->();
		const C* operator->() const;

		C& operator*();
		const C& operator*() const;

		bool operator==(const ComponentRef& rhs) const;
		bool operator!=(const ComponentRef& rhs) const;

		operator bool() const;

	public:
		bool IsValid() const noexcept;

		void Reset();

	public:
		NODISC C* Get();
		NODISC const C* Get() const;

	private:
		std::shared_ptr<void*> m_component {nullptr};

		template<class... Cs> requires IsComponents<Cs...>
		friend class ComponentSet; // to enable access to m_component
	};

	template<class C>
	inline ComponentRef<C>::ComponentRef(std::shared_ptr<void*> component)
		: m_component(component) { }

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
	inline bool ComponentRef<C>::operator==(const ComponentRef& rhs) const
	{
		return Get() == rhs.Get();
	}

	template<class C>
	inline bool ComponentRef<C>::operator!=(const ComponentRef& rhs) const
	{
		return !(*this == rhs);
	}

	template<class C>
	inline ComponentRef<C>::operator bool() const
	{
		return IsValid();
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
	inline bool ComponentRef<C>::IsValid() const noexcept
	{
		return m_component != nullptr && *m_component != nullptr; // check that pointer and its contents is valid
	}

	template<class C>
	inline void ComponentRef<C>::Reset()
	{
		m_component.reset();
	}
}