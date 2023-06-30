#pragma once

#include <tuple>
#include <memory>

#include <Velox/System/Concepts.h>
#include <Velox/Config.hpp>

#include "ComponentRef.hpp"
#include "ComponentAlloc.hpp"

namespace vlx
{
	///	ComponentSet is used to prevent having to write many ComponentRef to access the components of an object.
	/// 
	template<class... Cs> requires IsComponents<Cs...>
	class ComponentSet final
	{
	private:
		using ComponentTypes	= std::tuple<Cs...>;
		using ComponentRefs		= std::array<std::shared_ptr<void*>, sizeof...(Cs)>;

		template<std::size_t N>
		using ComponentType = std::tuple_element_t<N, ComponentTypes>;

	public:
		ComponentSet() = delete;
		ComponentSet(ComponentRef<Cs>&&... refs);

		bool operator==(const ComponentSet& other) const;
		bool operator!=(const ComponentSet& other) const;

	public:
		NODISC bool IsAnyValid() const;
		NODISC bool IsAllValid() const;

		template<std::size_t N>
		NODISC bool IsValid() const;

		template<IsComponent C> requires Contains<C, Cs...>
		NODISC bool IsValid() const;

	public:
		template<std::size_t N>
		NODISC auto Get() const -> const ComponentType<N>*;

		template<std::size_t N>
		NODISC auto Get() -> ComponentType<N>*;

		template<IsComponent C> requires Contains<C, Cs...>
		NODISC const C* Get() const;

		template<IsComponent C> requires Contains<C, Cs...>
		NODISC C* Get();

	private:
		ComponentRefs m_components;
	};

	template<class... Cs> requires IsComponents<Cs...>
	inline ComponentSet<Cs...>::ComponentSet(ComponentRef<Cs>&&... refs)
		: m_components{ std::forward<ComponentRef<Cs>>(refs).m_component... } { }

	template<class... Cs> requires IsComponents<Cs...>
	inline bool ComponentSet<Cs...>::operator==(const ComponentSet& other) const
	{
		return m_components == other.m_components;
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline bool ComponentSet<Cs...>::operator!=(const ComponentSet& other) const
	{
		return !(*this == other);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline bool ComponentSet<Cs...>::IsAnyValid() const
	{
		for (std::size_t i = 0; i < sizeof...(Cs); ++i)
		{
			if (m_components[i] != nullptr)
				return true;
		}

		return false;
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline bool ComponentSet<Cs...>::IsAllValid() const
	{
		for (std::size_t i = 0; i < sizeof...(Cs); ++i)
		{
			if (m_components[i] == nullptr)
				return false;
		}

		return true;
	}

	template<class... Cs> requires IsComponents<Cs...>
	template<std::size_t N>
	inline bool ComponentSet<Cs...>::IsValid() const
	{
		return Get<N>() != nullptr;
	}

	template<class... Cs> requires IsComponents<Cs...>
	template<IsComponent C> requires Contains<C, Cs...>
	inline bool ComponentSet<Cs...>::IsValid() const
	{
		return Get<C>() != nullptr;
	}

	template<class... Cs> requires IsComponents<Cs...>
	template<std::size_t N>
	inline auto ComponentSet<Cs...>::Get() const -> const ComponentType<N>*
	{
		using ComponentType = std::tuple_element_t<N, ComponentTypes>;
		return static_cast<ComponentType*>(*m_components[N].get());
	}

	template<class... Cs> requires IsComponents<Cs...>
	template<std::size_t N>
	inline auto ComponentSet<Cs...>::Get() -> ComponentType<N>*
	{
		return const_cast<ComponentType<N>*>(std::as_const(*this).Get<N>());
	}

	template<class... Cs> requires IsComponents<Cs...>
	template<IsComponent C> requires Contains<C, Cs...>
	inline const C* ComponentSet<Cs...>::Get() const
	{
		return Get<traits::IndexInTuple<C, ComponentTypes>::value>();
	}

	template<class... Cs> requires IsComponents<Cs...>
	template<IsComponent C> requires Contains<C, Cs...>
	inline C* ComponentSet<Cs...>::Get()
	{
		return Get<traits::IndexInTuple<C, ComponentTypes>::value>();
	}
}