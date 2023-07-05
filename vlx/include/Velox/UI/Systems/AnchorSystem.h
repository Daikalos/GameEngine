#pragma once

#include <Velox/ECS.hpp>

#include <Velox/Window/Window.h>

#include <Velox/Graphics/Components/Transform.h>

#include <Velox/UI/Components/Anchor.h>

#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API AnchorSystem final : public SystemAction
	{
	private:
		using System = System<Transform, ui::Anchor>;

	public:
		AnchorSystem(EntityAdmin& entity_admin, LayerType id, const Window& window);

	public:
		void Update() override;

	private:
		System m_system;

	};
}