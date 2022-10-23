#pragma once

#include <unordered_set>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	/// <summary>
	/// Represents the relationship between entities that allows for scene graphs 
	/// and other things...
	/// </summary>
	class VELOX_API Relation : public IComponent
	{
	public:
		void Created(const EntityAdmin& entity_admin, const EntityID entity_id) override;
		void Moved(const EntityAdmin& entity_admin, const EntityID entity_id) override;
		void Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id) override;

		void AttachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation& child);
		const EntityID DetachChild(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation& child);

		[[nodiscard]] bool HasNewRoot() const noexcept;

	private:
		void PropagateAttach(const EntityAdmin& entity_admin, const EntityID entity_id);
		void PropagateDetach(const EntityAdmin& entity_admin, const EntityID entity_id);

		void UpdateNewRoot(const EntityAdmin& entity_admin);

	private:
		EntityID						m_parent	{NULL_ENTITY};
		std::vector<EntityID>			m_children;

		std::unordered_set<EntityID>	m_closed; // to prevent parenting descendants
		bool							m_new_root	{false};
	};
}