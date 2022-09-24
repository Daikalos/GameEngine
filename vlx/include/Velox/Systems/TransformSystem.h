#pragma once

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

#include <Velox/Components/Transform.h>
#include <Velox/Components/Node.h>

namespace vlx
{
	constexpr std::uint16_t TRANSFORM_LAYER = 1;

	class VELOX_API TransformSystem
	{
	private:
		using System = System<Transform, Node>;

	public:
		TransformSystem(EntityAdmin* entity_admin);

	private:
		System m_system;
	};
}