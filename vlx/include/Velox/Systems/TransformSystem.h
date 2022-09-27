#pragma once

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

#include <Velox/Components/Transform.h>
#include <Velox/Components/Relationship.h>

namespace vlx
{
	constexpr std::uint16_t TRANSFORM_LAYER = 1;

	class VELOX_API TransformSystem
	{
	private:
		using System = System<Transform, Relationship>;

	public:
		TransformSystem(EntityAdmin* entity_admin);

		void AttachParent(const Relationship& target, const Relationship& parent);
		void DetachParent(const Relationship& target, const Relationship& parent);

		void AttachChild(const Relationship& target, const Relationship& child);
		void DetachChild(const Relationship& target, const Relationship& child);

	private:
		System m_system;
	};
}