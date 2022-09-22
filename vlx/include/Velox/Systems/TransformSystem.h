#pragma once

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

#include <Velox/Components/Transform.h>
#include <Velox/Components/Node.h>

namespace vlx
{
	class VELOX_API TransformSystem
	{
	private:
		using System = System<Transform, Node>;

	public:

	private:
		System m_system;
	};
}