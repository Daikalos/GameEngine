#pragma once

#include <Velox/ECS.hpp>

#include <Velox/Graphics/GUI/Anchor.h>
#include <Velox/Components/Transform.h>

#include <Velox/Window/Window.h>
#include <Velox/Config.hpp>

#include "ISystemObject.h"

namespace vlx
{
	class VELOX_API AnchorSystem : public ISystemObject
	{
	private:
		using System = System<Transform, gui::Anchor>;

	public:
		AnchorSystem(EntityAdmin& entity_admin, const Window& window);

		[[nodiscard]] constexpr LayerType GetID() const noexcept override;

	public:
		void Update() override;

	private:
		EntityAdmin*	m_entity_admin {nullptr};
		System			m_system;
	};
}