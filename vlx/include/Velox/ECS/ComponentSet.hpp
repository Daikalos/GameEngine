#pragma once

#include <Velox/ECS.hpp>

#include <Velox/Utilities.hpp>

#include <unordered_map>
#include <memory>

#include "ComponentProxy.hpp"
#include "ComponentAlloc.hpp"

namespace vlx
{
	/// <summary>
	///		ComponentSet is used to prevent having to write many ComponentProxy to access
	///		the components of an object.
	/// </summary>
	template<IsComponents... Cs>
	class ComponentSet final : private NonCopyable
	{
	private:
		using ComponentSetMap = std::unordered_map<ComponentTypeID, std::shared_ptr<IComponentProxy>>;

	public:
		ComponentSet(ComponentProxyPtr<Cs>... proxies);

		template<IsComponent C> requires Contains<C, Cs...>
		C& Get();

		template<IsComponent C> requires Contains<C, Cs...>
		const C& Get() const;

	private:
		ComponentSetMap m_components;
	};

	template<IsComponents... Cs>
	inline ComponentSet<Cs...>::ComponentSet(ComponentProxyPtr<Cs>... proxies)
	{
		(m_components.try_emplace(ComponentAlloc<Cs>::GetTypeID(), proxies), ...);
	}

	template<IsComponents... Cs>
	template<IsComponent C> requires Contains<C, Cs...>
	inline C& ComponentSet<Cs...>::Get()
	{
		return const_cast<C&>(std::as_const(*this).Get<C>());
	}

	template<IsComponents... Cs>
	template<IsComponent C> requires Contains<C, Cs...>
	inline const C& ComponentSet<Cs...>::Get() const
	{
		return *static_cast<ComponentProxy<C>&>(*m_components.at(ComponentAlloc<C>::GetTypeID())).Get();
	}
}