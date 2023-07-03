#pragma once

#include <Velox/ECS/Identifiers.hpp>
#include <Velox/ECS/SystemAction.h>
#include <Velox/Window/Camera.h>

#include <Velox/Graphics/Components/Renderable.h>
#include <Velox/Graphics/Components/GlobalTransformMatrix.h>
#include <Velox/Graphics/Components/Sprite.h>
#include <Velox/Graphics/Components/Mesh.h>

namespace vlx
{
	class VELOX_API CullingSystem final : public SystemAction
	{
	public:
		using SpriteSystem = System<Renderable, GlobalTransformMatrix, Sprite>;
		using MeshSystem = System<Renderable, GlobalTransformMatrix, Mesh>;

	private:
		static constexpr int LENIENCY = 128;

	public:
		CullingSystem(EntityAdmin& entity_admin, const LayerType id, const Camera& camera);

	public:
		void PostUpdate() override;

	private:
		SpriteSystem	m_cull_sprites;

		const Camera*	m_camera {nullptr};
	};
}