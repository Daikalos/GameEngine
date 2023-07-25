#pragma once

#include <Velox/ECS/Identifiers.hpp>
#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>

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
		CullingSystem(EntityAdmin& entity_admin, LayerType id, const Camera& camera);

	public:
		void PostUpdate() override;

	private:
		void CullSprites(std::size_t size, Renderable* renderables, GlobalTransformMatrix* gtms, Sprite* sprites) const;
		void CullMeshes(std::size_t size, Renderable* renderables, GlobalTransformMatrix* gtms, Mesh* meshes) const;

	private:
		const Camera*	m_camera {nullptr};

		SpriteSystem	m_cull_sprites;
		MeshSystem		m_cull_meshes;
	};
}