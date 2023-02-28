#pragma once

#include <Velox/ECS/Identifiers.hpp>
#include <Velox/ECS/SystemAction.h>
#include <Velox/Window/Camera.h>

#include <Velox/Algorithms/QTElement.hpp>

#include <Velox/Graphics/Components/Object.h>
#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/Sprite.h>

namespace vlx
{
	class VELOX_API CullingSystem final : public SystemAction
	{
	public:
		using QTSystem	= System<Object, Transform, Sprite>;

	private:
		static constexpr int LENIENCY = 128;

	public:
		CullingSystem(EntityAdmin& entity_admin, const LayerType id, const Camera& camera);

	public:
		bool IsRequired() const noexcept override;

	public:
		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

	private:
		QTSystem			m_system;
		const Camera*		m_camera {nullptr};
	};
}