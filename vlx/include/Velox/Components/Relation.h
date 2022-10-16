#pragma once

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

	private:
		EntityID				m_parent		{NULL_ENTITY};
		std::vector<EntityID>	m_children;

		bool					m_invalidated	{false};
	};
}