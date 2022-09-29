#pragma once

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	/// <summary>
	/// Represents the relationship between entities that allows for scene graphs 
	/// and other things...
	/// </summary>
	class VELOX_API Relationship
	{
	public:
		~Relationship();

	public:
		void AttachParent(const EntityAdmin& entity_admin, Relationship& parent, const EntityID entity_id);
		void DetachParent(const EntityAdmin& entity_admin, Relationship& parent, const EntityID entity_id);

		void AttachChild(const EntityAdmin& entity_admin, Relationship& child, const EntityID entity_id);
		void DetachChild(const EntityAdmin& entity_admin, Relationship& child, const EntityID entity_id);

	private:
		EntityID parent	{NULL_ENTITY};
		std::vector<EntityID> children;
	};
}