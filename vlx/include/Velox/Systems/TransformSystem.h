#pragma once

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

#include <Velox/Components/Transform.h>
#include <Velox/Components/Relationship.h>

namespace vlx
{
	class VELOX_API TransformSystem
	{
	private:
		using System = System<Transform, Relationship>;

	public:
		TransformSystem(EntityAdmin* entity_admin);

	private:
		System m_system;
	};
}