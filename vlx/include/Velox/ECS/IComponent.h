#pragma once

#include <Velox/ECS/Identifiers.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	class EntityAdmin;

	struct VELOX_API IComponent
	{
		virtual ~IComponent() = 0;

		virtual void Created(const EntityAdmin& entity_admin, const EntityID entity_id) {}
		virtual void Moved(const EntityAdmin& entity_admin, const EntityID entity_id) {}
		virtual void Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id) {}
	};

	inline IComponent::~IComponent() = default;
}