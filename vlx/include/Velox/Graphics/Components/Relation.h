#pragma once

#include <type_traits>
#include <functional>

#include <Velox/ECS/EntityAdmin.h>
#include <Velox/ECS/Identifiers.hpp>
#include <Velox/ECS/ComponentRef.hpp>
#include <Velox/ECS/ComponentEvents.h>

#include <Velox/Config.hpp>

namespace vlx
{
	class EntityAdmin;

	/// Represents the relationship between entities
	/// 
	class Relation : public EventSet<Relation, CopiedEvent, AlteredEvent, DestroyedEvent>
	{
	public:
		struct Ref
		{
			ComponentRef<Relation> ptr;
			EntityID entity_id {NULL_ENTITY};
		};

		using Parent = Ref;
		using Children = std::vector<Ref>;

	private:
		template<class C>
		using CompFunc = std::function<bool(C&)>;

		template<class C>
		using SortFunc = std::function<bool(const C&, const C&)>;

	public:
		NODISC VELOX_API bool HasParent() const noexcept;
		NODISC VELOX_API bool HasChildren() const noexcept;

		NODISC VELOX_API auto GetParent() const noexcept -> const Parent&;
		NODISC VELOX_API auto GetChildren() const noexcept -> const Children&;

		NODISC VELOX_API bool IsDescendant(EntityID descendant) const;

	private:
		VELOX_API void CopiedImpl(const EntityAdmin& entity_admin, EntityID entity_id);
		VELOX_API void AlteredImpl(const EntityAdmin& entity_admin, EntityID entity_id, Relation& new_data);
		VELOX_API void DestroyedImpl(const EntityAdmin& entity_admin, EntityID entity_id);

	public:
		template<class C>
		void IterateChildren(const CompFunc<C>& func, const EntityAdmin& entity_admin, bool include_descendants = true) const;

		template<class C>
		void SortChildren(const SortFunc<C>& func, const EntityAdmin& entity_admin, bool include_descendants = false);

	private:
		Parent		m_parent;
		Children	m_children;
		
		friend class RelationSystem;

		friend class CopiedEvent<Relation>;
		friend class AlteredEvent<Relation>;
		friend class DestroyedEvent<Relation>;
	};

	template<class C>
	inline void Relation::IterateChildren(const CompFunc<C>& func, const EntityAdmin& entity_admin, bool include_descendants) const
	{
		for (const Ref& ref : m_children)
		{
			const auto component = entity_admin.TryGetComponent<std::decay_t<C>>(ref.entity_id);

			if (!component || !func(*component))
				continue;

			if (include_descendants) // continue iterating descendants
			{
				ref.ptr->IterateChildren<std::decay_t<C>>(func, entity_admin, include_descendants);
			}
		}
	}

	template<class C>
	inline void Relation::SortChildren(const SortFunc<C>& func, const EntityAdmin& entity_admin, bool include_descendants)
	{
		std::ranges::sort(m_children.begin(), m_children.end(),
			[&func, &entity_admin](const Ref& lhs, const Ref& rhs)
			{
				const auto lhs_comp = entity_admin.TryGetComponent<C>(lhs.entity_id);

				if (!lhs_comp)
					return false;

				const auto rhs_comp = entity_admin.TryGetComponent<C>(rhs.entity_id);

				if (!rhs_comp)
					return false;

				return func(*lhs_comp, *rhs_comp);
			});

		if (include_descendants)
		{
			for (const Ref& ref : m_children)
			{
				ref.ptr->SortChildren(func, entity_admin, include_descendants);
			}
		}
	}
}