#pragma once

#include <type_traits>
#include <functional>

#include <Velox/ECS/EntityAdmin.h>
#include <Velox/ECS/Identifiers.hpp>
#include <Velox/ECS/ComponentRef.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	/// Represents the relationship between entities
	/// 
	class Relation : public IComponent
	{
	public:
		using Ref = ComponentRef<Relation>;

		using Parent = Ref;
		using Children = std::vector<Ref>;

	private:
		template<class C>
		using CompFunc = std::function<bool(C&)>;

		template<class C>
		using SortFunc = std::function<bool(const C&, const C&)>;

	public:
		VELOX_API NODISC bool HasParent() const noexcept;
		VELOX_API NODISC constexpr bool HasChildren() const noexcept;

		VELOX_API NODISC auto GetParent() const noexcept -> const Parent&;
		VELOX_API NODISC auto GetChildren() const noexcept -> const Children&;

		VELOX_API NODISC bool IsDescendant(const EntityID descendant) const;

	private:
		VELOX_API void Copied(const EntityAdmin& entity_admin, const EntityID entity_id) override;
		VELOX_API void Modified(const EntityAdmin& entity_admin, const EntityID entity_id, IComponent& new_data) override;
		VELOX_API void Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id) override;

	public:
		template<class C>
		void IterateChildren(const CompFunc<C>& func, const EntityAdmin& entity_admin, bool include_descendants = true) const;

		template<class C>
		void SortChildren(const SortFunc<C>& func, const EntityAdmin& entity_admin, bool include_descendants = false);

	private:
		Parent		m_parent;
		Children	m_children;
		
		friend class RelationSystem;
	};

	template<class C>
	inline void Relation::IterateChildren(const CompFunc<C>& func, const EntityAdmin& entity_admin, bool include_descendants) const
	{
		for (const Ref& ptr : m_children)
		{
			const auto component = entity_admin.TryGetComponent<std::decay_t<C>>(ptr->GetEntityID());

			if (!component || !func(*component))
				continue;

			if (include_descendants) // continue iterating descendants
			{
				ptr->IterateChildren<std::decay_t<C>>(func, entity_admin, include_descendants);
			}
		}
	}

	template<class C>
	inline void Relation::SortChildren(const SortFunc<C>& func, const EntityAdmin& entity_admin, bool include_descendants)
	{
		std::ranges::sort(m_children.begin(), m_children.end(),
			[&func, &entity_admin](const Ref& lhs, const Ref& rhs)
			{
				const auto lhs_comp = entity_admin.TryGetComponent<C>(lhs.GetEntityID());

				if (!lhs_comp)
					return false;

				const auto rhs_comp = entity_admin.TryGetComponent<C>(rhs.GetEntityID());

				if (!rhs_comp)
					return false;

				return func(*lhs_comp, *rhs_comp);
			});

		if (include_descendants)
		{
			for (const Ref& ref : m_children)
			{
				ref->SortChildren(func, entity_admin, include_descendants);
			}
		}
	}
}