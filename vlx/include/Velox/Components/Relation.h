#pragma once

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	/// <summary>
	/// Represents the relationship between entities that allows for scene graphs 
	/// and other things...
	/// </summary>
	class VELOX_API Relation
	{
	public:
		~Relation();

	public:
		void AttachParent(const EntityAdmin& entity_admin, Relation& parent, const EntityID entity_id, bool invalidate_children = false);
		void DetachParent(const EntityAdmin& entity_admin, Relation& parent, const EntityID entity_id, bool invalidate_children = false);

		void AttachChild(const EntityAdmin& entity_admin, Relation& child, const EntityID entity_id, bool invalidate_children = false);
		void DetachChild(const EntityAdmin& entity_admin, Relation& child, const EntityID entity_id, bool invalidate_children = false);

	private:
		EntityID				m_parent		{NULL_ENTITY};
		std::vector<EntityID>	m_children;

		bool					m_invalidated	{false};
	};
}