#pragma once

#include <Velox/Utilities.hpp>
#include <Velox/ECS/Identifiers.hpp>

#include <unordered_map>
#include <memory>

#include "ComponentProxy.hpp"
#include "ComponentAlloc.hpp"

namespace vlx
{
	/// <summary>
	///		ComponentSet is used to prevent having to write many ComponentProxy to get
	///		the components of an object.
	/// </summary>
	template<IsComponent... Cs>
	class ComponentSet
	{
	public:
		template<class... Args>
		ComponentSet(Args&&... args);

		template<IsComponent C> requires Contains<C, Cs...>
		C& Get();

	private:
		std::unordered_map<ComponentTypeID, IComponentProxy*> m_components;
	};

	template<IsComponent... Cs>
	template<class... Args>
	inline ComponentSet<Cs...>::ComponentSet(Args&&... args)
	{
		(m_components.try_emplace(ComponentAlloc<Cs>::GetTypeID(), &std::forward<Args>(args)), ...);
	}

	template<IsComponent... Cs>
	template<IsComponent C> requires Contains<C, Cs...>
	inline C& ComponentSet<Cs...>::Get()
	{
		return *static_cast<ComponentProxy<C>&>(*m_components[ComponentAlloc<C>::GetTypeID()]).Get();
	}
}