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
	///		ComponentSet is used to prevent having to write many ComponentProxy to access
	///		the components of an object.
	/// </summary>
	template<class... Cs> requires IsComponents<Cs...>
	class ComponentSet final
	{
	private:
		using ComponentTypes	= std::tuple<Cs...>;
		using ComponentRefs		= std::tuple<std::shared_ptr<Cs*>...>;

	public:
		ComponentSet(ComponentRef<Cs>&&... refs);

	public:
		template<std::size_t N>
		[[nodiscard]] auto Get() const -> const std::tuple_element_t<N, ComponentTypes>*;

		template<std::size_t N>
		[[nodiscard]] auto Get() -> std::tuple_element_t<N, ComponentTypes>*;

	private:
		ComponentRefs m_components;
	};

	template<class... Cs> requires IsComponents<Cs...>
	inline ComponentSet<Cs...>::ComponentSet(ComponentRef<Cs>&&... refs)
		: m_components(std::reinterpret_pointer_cast<Cs*>(std::forward<ComponentRef<Cs>>(refs).m_component)...) { }

	template<class... Cs> requires IsComponents<Cs...>
	template<std::size_t N>
	inline auto ComponentSet<Cs...>::Get() const -> const std::tuple_element_t<N, ComponentTypes>*
	{
		using ComponentType = std::tuple_element_t<N, ComponentTypes>;
		return static_cast<ComponentType*>(*std::get<N>(m_components).get());
	}

	template<class... Cs> requires IsComponents<Cs...>
	template<std::size_t N>
	inline auto ComponentSet<Cs...>::Get() -> std::tuple_element_t<N, ComponentTypes>*
	{
		using ComponentType = std::tuple_element_t<N, ComponentTypes>;
		return const_cast<ComponentType*>(std::as_const(*this).Get<N>());
	}
}