#pragma once

#include <Velox/ECS.hpp>

#include <Velox/Systems/ObjectSystem.h>
#include <Velox/Systems/RenderSystem.h>
#include <Velox/Systems/TransformSystem.h>

#include <Velox/Config.hpp>

namespace vlx
{
	/// <summary>
	/// Represents the world's logic e.g., objects, rendering, transforms, etc.
	/// </summary>
	class VELOX_API World
	{
	public:
		World();

		Entity CreateObject();





	private:
		EntityAdmin m_entity_admin;

		ObjectSystem m_object_system;
		RenderSystem m_render_system;
		TransformSystem m_transform_system;
	};
}