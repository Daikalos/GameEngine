#pragma once

#include <vector>

#include <Velox/ECS.hpp>
#include <Velox/VeloxTypes.hpp>

#include "GUIComponent.h"

namespace vlx::gui
{
	class Container : public GUIComponent
	{
	private:
		using SizeType			= int16;
		using ChildType			= ComponentRef<GUIComponent>;

		template<class C>
		using SortFunc = std::function<bool(const C&, const C&)>;

	public:
		using GUIComponent::GUIComponent;

		Container() = default;

	public:
		VELOX_API NODISC bool IsEmpty() const noexcept;
		VELOX_API NODISC bool IsSelectable() const noexcept override;

	public:
		template<IsComponent C>
		void Push(const EntityAdmin& entity_admin, EntityID entity_id);
		template<IsComponent C>
		void Erase(EntityID entity_id);

		template<IsComponent C>
		void Sort(const EntityAdmin& entity_admin, const SortFunc<C>& func);

	public:
		VELOX_API bool HasSelection() const noexcept;

		VELOX_API void SelectAt(int index);

		VELOX_API void SelectNext();
		VELOX_API void SelectPrev();

	private:
		std::vector<ChildType>	m_children;
		SizeType				m_selected_child {-1};
	};

	template<IsComponent C>
	inline void Container::Push(const EntityAdmin& entity_admin, EntityID entity_id)
	{
		auto child = entity_admin.TryGetBaseRef<GUIComponent>(entity_id, entity_admin.GetComponentID<C>());

		if (!child.has_value())
			return;

		const auto it = std::find(m_children.begin(), m_children.end(), 
			[&entity_id](const ChildType& child)
			{
				return child.GetEntityID() == entity_id;
			});

		if (it == m_children.end()) // prevent duplicates
		{
			m_children.emplace_back(child);

			if (!HasSelection() && child->Get()->IsSelectable())
				SelectAt(m_children.size() - 1);
		}
	}

	template<IsComponent C>
	inline void Container::Erase(EntityID entity_id)
	{
		const auto it = std::find_if(m_children.begin(), m_children.end(),
			[&entity_id](const ChildType& child)
			{
				return child.GetEntityID() == entity_id;
			});

		if (it == m_children.end())
			return;

		const auto index = it - m_children.begin();

		if (m_selected_child >= index)
			SelectPrev();

		m_children.erase(it);
	}

	template<IsComponent C>
	inline void Container::Sort(const EntityAdmin& entity_admin, const SortFunc<C>& func)
	{
		std::sort(m_children.begin(), m_children.end(),
			[&entity_admin](const ChildType& lhs, const ChildType& rhs)
			{
				const auto [lhs_comp, lhs_success] = entity_admin.TryGetComponent<C>(lhs.GetEntityID());

				if (!lhs_success)
					return false;

				const auto [rhs_comp, rhs_success] = entity_admin.TryGetComponent<C>(rhs.GetEntityID());

				if (!rhs_success)
					return false;

				return func(*lhs_comp, *rhs_comp);
			});
	}
}