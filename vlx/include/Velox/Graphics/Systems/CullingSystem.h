#pragma once

#include <Velox/ECS/Identifiers.hpp>
#include <Velox/ECS/SystemAction.h>
#include <Velox/Window/Camera.h>

#include <Velox/Graphics/Components/Renderable.h>
#include <Velox/Graphics/Components/GlobalTransform.h>
#include <Velox/Graphics/Components/Sprite.h>

namespace vlx
{
	class VELOX_API CullingSystem final : public SystemAction
	{
	public:
		using System = System<Renderable, GlobalTransform, Sprite>;

	private:
		static constexpr int LENIENCY = 128;

	public:
		CullingSystem(EntityAdmin& entity_admin, const LayerType id, const Camera& camera);

	public:
		bool IsRequired() const noexcept override;

	public:
		void Start() override;
		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

	private:
		System			m_system;
		const Camera*	m_camera {nullptr};
	};
}