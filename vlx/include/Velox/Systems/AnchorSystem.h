#pragma once

#include <Velox/ECS.hpp>

#include <Velox/Graphics/GUI/Anchor.h>
#include <Velox/Components/Transform.h>

#include <Velox/Window/Window.h>
#include <Velox/Config.hpp>

#include "ISystemObject.h"

namespace vlx
{
	class AnchorSystem : public ISystemObject
	{
	private:
		using System = System<Transform, gui::Anchor>;

	public:
		VELOX_API AnchorSystem(EntityAdmin& entity_admin, const Window& window);

		VELOX_API void Update() override;

	private:
		EntityAdmin*	m_entity_admin {nullptr};
		System			m_system;
	};
}