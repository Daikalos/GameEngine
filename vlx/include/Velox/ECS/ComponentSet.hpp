#pragma once

#include <Velox/ECS.hpp>

#include <Velox/Utilities.hpp>

#include <unordered_map>
#include <memory>

#include "ComponentRef.hpp"
#include "ComponentAlloc.hpp"

namespace vlx
{
	/// <summary>
	///		ComponentSet is used to prevent having to write many ComponentProxy to access
	///		the components of an object.
	/// </summary>
	template<class... Cs> requires IsComponents<Cs...>
	class ComponentSet final : private NonCopyable
	{
	private:
		using ComponentSetMap = std::unordered_map<ComponentTypeID, std::shared_ptr<IComponentRef>>;

	public:
		ComponentSet(ComponentRefPtr<Cs>... refs);

		template<IsComponent C> requires Contains<C, Cs...>
		[[nodiscard]] const C& Get() const;

		template<IsComponent C> requires Contains<C, Cs...>
		[[nodiscard]] C& Get();

	private:
		ComponentSetMap m_components;
	};

	template<class... Cs> requires IsComponents<Cs...>
	inline ComponentSet<Cs...>::ComponentSet(ComponentRefPtr<Cs>... refs)
	{
		(m_components.try_emplace(ComponentAlloc<Cs>::GetTypeID(), refs), ...);
	}

	template<class... Cs> requires IsComponents<Cs...>
	template<IsComponent C> requires Contains<C, Cs...>
	inline const C& ComponentSet<Cs...>::Get() const
	{
		return *static_cast<ComponentRef<C>&>(*m_components.at(ComponentAlloc<C>::GetTypeID())).Get();
	}

	template<class... Cs> requires IsComponents<Cs...>
	template<IsComponent C> requires Contains<C, Cs...>
	inline C& ComponentSet<Cs...>::Get()
	{
		return const_cast<C&>(std::as_const(*this).Get<C>());
	}
}