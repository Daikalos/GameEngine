#pragma once

#include <unordered_set>
#include <type_traits>
#include <functional>

#include <Velox/ECS/EntityAdmin.h>
#include <Velox/ECS/Identifiers.hpp>
#include <Velox/ECS/ComponentRef.hpp>
#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	/// <summary>
	///		Represents the relationship between entities that allows for scene graphs and possibly other things... 
	/// </summary>
	class Relation : public IComponent
	{
	private:
		using RelationPtr = ComponentRefPtr<Relation>;
		using Children = std::vector<RelationPtr>;

		template<class C>
		using CompFunc = std::function<bool(C&)>;

		template<class C>
		using SortFunc = std::function<bool(const C&, const C&)>;

	public:
		VELOX_API [[nodiscard]] bool HasParent() const noexcept;
		VELOX_API [[nodiscard]] constexpr bool HasChildren() const noexcept;

		VELOX_API [[nodiscard]] auto GetParent() const noexcept -> const RelationPtr;
		VELOX_API [[nodiscard]] auto GetChildren() const noexcept -> const Children&;

	public:
		VELOX_API void AttachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation& child);
		VELOX_API EntityID DetachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation& child);

	private:
		VELOX_API void Copied(const EntityAdmin& entity_admin, const EntityID entity_id) override;
		VELOX_API void Modified(const EntityAdmin& entity_admin, const EntityID entity_id, const IComponent& new_data) override;
		VELOX_API void Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id) override;

		VELOX_API void PropagateAttach(const EntityAdmin& entity_admin, const EntityID child_id, const Relation& child);
		VELOX_API void PropagateDetach(const EntityAdmin& entity_admin, const EntityID child_id, const Relation& child);

	public:
		template<class C>
		void IterateChildren(const CompFunc<C>& func, const EntityAdmin& entity_admin, bool include_descendants = true) const;

		template<class C>
		void SortChildren(const SortFunc<C>& func, const EntityAdmin& entity_admin, bool include_descendants = false);

	private:
		RelationPtr	m_parent;
		Children	m_children;

		std::unordered_set<EntityID> m_descendants; // list of all descendants (to prevent parenting them when attaching)
	};

	template<class C>
	inline void Relation::IterateChildren(const CompFunc<C>& func, const EntityAdmin& entity_admin, bool include_descendants) const
	{
		for (const RelationPtr& ptr : m_children)
		{
			auto [component, success] = entity_admin.TryGetComponent<std::decay_t<C>>(ptr->GetEntityID());

			if (success)
			{
				if (!func(*component))
					continue;
			}

			if (include_descendants) // continue iterating descendants
			{
				(*ptr)->IterateChildren<std::decay_t<C>>(func, entity_admin, include_descendants);
			}
		}
	}

	template<class C>
	inline void Relation::SortChildren(const SortFunc<C>& func, const EntityAdmin& entity_admin, bool include_descendants)
	{
		std::sort(m_children.begin(), m_children.end(),
			[&func, &entity_admin](const RelationPtr& lhs, const RelationPtr& rhs)
			{
				const auto [lhs_comp, lhs_success] = entity_admin.TryGetComponent<C>(lhs->GetEntityID());

				if (!lhs_success)
					return false;

				const auto [rhs_comp, rhs_success] = entity_admin.TryGetComponent<C>(rhs->GetEntityID());

				if (!rhs_success)
					return false;

				return func(*lhs_comp, *rhs_comp);
			});

		if (include_descendants)
		{
			for (const RelationPtr& ptr : m_children)
			{
				(*ptr)->SortChildren(func, entity_admin, include_descendants);
			}
		}
	}
}