#pragma once

#include <vector>

#include <Velox/ECS/IComponent.h>
#include <Velox/ECS/BaseProxy.hpp>

#include <Velox/Utilities.hpp>

#include "GUIComponent.h"

namespace vlx::gui
{
	class Container : public IComponent
	{
	private:
		using SizeType			= std::int16_t;
		using ChildType			= BaseProxy<GUIComponent>;

		template<class C>
		using SortFunc = std::function<bool(const C&, const C&)>;

	public:
		[[nodiscard]] constexpr bool IsEmpty() const noexcept;

	public:
		template<IsComponent C>
		void Push(const EntityAdmin& entity_admin, const EntityID entity_id);
		template<IsComponent C>
		void Erase(const EntityAdmin& entity_admin, const EntityID entity_id);

		void SelectNext(const EntityAdmin& entity_admin);
		void SelectPrev(const EntityAdmin& entity_admin);

		template<IsComponent C>
		void Sort(const EntityAdmin& entity_admin, const SortFunc<C>& func);

	private:
		void SelectSteps(const EntityAdmin& entity_admin, int steps);

	private:
		std::vector<ChildType*>	m_children;
		SizeType				m_selected_index {-1};
	};

	template<IsComponent C>
	inline void Container::Push(const EntityAdmin& entity_admin, const EntityID entity_id)
	{
		ChildType& child = entity_admin.GetBaseProxy<GUIComponent>(entity_id, entity_admin.GetComponentID<C>());

		child.ForceUpdate();
		if (child.IsExpired()) // if not a valid entity
			return;

		const auto it = std::find(m_children.begin(), m_children.end(), 
			[&entity_id](const ChildType* child)
			{
				return child->GetEntityID() == entity_id;
			});

		if (it == m_children.end()) // prevent duplicates
			m_children.emplace_back(&child);
	}

	template<IsComponent C>
	inline void Container::Erase(const EntityAdmin& entity_admin, const EntityID entity_id)
	{
		const auto it = std::find(m_children.begin(), m_children.end(),
			[&entity_id](const ChildType* child)
			{
				return child->GetEntityID() == entity_id;
			});

		if (it == m_children.end())
			return;

		const auto index = it - m_children.begin();

		if (m_selected_index >= index)
			SelectPrev(entity_admin);

		m_children.erase(it);
	}

	template<IsComponent C>
	inline void Container::Sort(const EntityAdmin& entity_admin, const SortFunc<C>& func)
	{
		std::sort(m_children.begin(), m_children.end(),
			[&entity_admin](const ChildType* lhs, const ChildType* rhs)
			{
				const auto [lhs_comp, lhs_success] = entity_admin.TryGetComponent<C>(lhs->GetEntityID());

				if (!lhs_success)
					return false;

				const auto [rhs_comp, rhs_success] = entity_admin.TryGetComponent<C>(rhs->GetEntityID());

				if (!rhs_success)
					return false;

				return func(*lhs_comp, *rhs_comp);
			});
	}
}