#pragma once

#include <vector>

#include <Velox/ECS/IComponent.h>
#include <Velox/ECS/ComponentProxy.hpp>

#include <Velox/Utilities.hpp>

#include "GUIComponent.h"

namespace vlx::gui
{
	class Container : public IComponent
	{
	private:
		using SizeType = std::int16_t;

		template<class C>
		using SortFunc = std::function<bool(const C&, const C&)>;

	public:
		[[nodiscard]] constexpr bool IsEmpty() const noexcept;

	public:
		void Push(const EntityAdmin& entity_admin, const EntityID entity_id);
		void Erase(const EntityAdmin& entity_admin, const EntityID entity_id);

		void SelectNext(const EntityAdmin& entity_admin);
		void SelectPrev(const EntityAdmin& entity_admin);

		template<IsComponent C>
		void Sort(const EntityAdmin& entity_admin, const SortFunc<C>& func);

	private:
		void SelectSteps(const EntityAdmin& entity_admin, int steps);

	private:
		std::vector<EntityID>	m_children;
		SizeType				m_selected_index	{-1};
	};

	template<IsComponent C>
	inline void Container::Sort(const EntityAdmin& entity_admin, const SortFunc<C>& func)
	{
		std::sort(m_children.begin(), m_children.end(),
			[&entity_admin](const EntityID lhs, const EntityID rhs)
			{
				const auto [lhs_comp, lhs_success] = entity_admin.TryGetComponent<C>(lhs);

				if (!lhs_success)
					return false;

				const auto [rhs_comp, rhs_success] = entity_admin.TryGetComponent<C>(rhs);

				if (!rhs_success)
					return false;

				return func(*lhs_comp, *rhs_comp);
			});
	}
}