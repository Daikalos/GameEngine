#pragma once

#include <Velox/ECS.hpp>

#include <Velox/Graphics/GUI/Anchor.h>
#include <Velox/Graphics/Components/Transform.h>

#include <Velox/Window/Window.h>
#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API AnchorSystem : public SystemObject
	{
	private:
		using System = System<Transform, gui::Anchor>;

	public:
		AnchorSystem(EntityAdmin& entity_admin, const LayerType id, const Window& window);

	public:
		void Update() override;

	private:
		System m_system;
	};
}