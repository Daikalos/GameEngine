#pragma once

#include <unordered_map>
#include <tuple>
#include <memory>

#include <Velox/Utilities.hpp>

#include "IComponent.h"
#include "ComponentRef.hpp"
#include "ComponentAlloc.hpp"

namespace vlx
{
	/// <summary>
	///		ComponentSet is used to prevent having to write many ComponentRef to access
	///		the components of an object.
	/// </summary>
	template<class... Cs> requires IsComponents<Cs...>
	class ComponentSet final
	{
	private:
		using ComponentTypes	= std::tuple<Cs...>;
		using ComponentRefs		= std::tuple<std::shared_ptr<Cs*>...>;

		template<std::size_t N>
		using ComponentType = std::tuple_element_t<N, ComponentTypes>;

	public:
		ComponentSet(ComponentRef<Cs>&&... refs);

	public:
		template<std::size_t N>
		[[nodiscard]] auto Get() const -> const ComponentType<N>*;

		template<std::size_t N>
		[[nodiscard]] auto Get() -> ComponentType<N>*;

		template<IsComponent C> requires Contains<C, Cs...>
		[[nodiscard]] const C* Get() const;

		template<IsComponent C> requires Contains<C, Cs...>
		[[nodiscard]] C* Get();

	private:
		ComponentRefs m_components;
	};

	template<class... Cs> requires IsComponents<Cs...>
	inline ComponentSet<Cs...>::ComponentSet(ComponentRef<Cs>&&... refs)
		: m_components(std::reinterpret_pointer_cast<Cs*>(std::forward<ComponentRef<Cs>>(refs).m_component)...) { }

	template<class... Cs> requires IsComponents<Cs...>
	template<std::size_t N>
	inline auto ComponentSet<Cs...>::Get() const -> const ComponentType<N>*
	{
		using ComponentType = std::tuple_element_t<N, ComponentTypes>;
		return static_cast<ComponentType*>(*std::get<N>(m_components).get());
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