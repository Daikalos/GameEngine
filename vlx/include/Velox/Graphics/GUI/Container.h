#pragma once

#include <vector>

#include <Velox/ECS/IComponent.h>
#include <Velox/ECS/ComponentProxy.hpp>

#include "Component.h"

namespace vlx::gui
{
	class Container : public IComponent
	{
	private:
		using SizeType = std::uint16_t;
		using EnityProxy = std::pair<EntityID, ComponentProxy<Component>>;

	public:
		[[nodiscard]] constexpr bool HasChildren() const noexcept;

	public:
		void Push(const EntityID entity_id);
		void Erase(const EntityID entity_id);

		void SelectNext(const EntityAdmin& entity_admin);
		void SelectPrev(const EntityAdmin& entity_admin);

	private:
		std::vector<EntityID>	m_children;
		SizeType				m_selected_index{0};
	};
}