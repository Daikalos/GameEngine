#pragma once

#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>

#include <Velox/Window.hpp>

#include <Velox/Graphics/Components/GlobalTransformMatrix.h>
#include <Velox/Graphics/Components/TransformMatrix.h>
#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/Renderable.h>
#include <Velox/Graphics/Components/Sprite.h>
#include <Velox/Graphics/Components/Mesh.h>
#include <Velox/Graphics/SpriteBatch.h>

#include <Velox/Physics/PhysicsBody.h>
#include <Velox/Physics/PhysicsBodyTransform.h>

#include <Velox/System/Time.h>
#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API RenderSystem final : public SystemAction
	{
	private:
		using SpriteSystem	= SystemExclude<Renderable, Sprite, GlobalTransformMatrix>;
		using MeshSystem	= SystemExclude<Renderable, Mesh, GlobalTransformMatrix>;

		using SpriteBodySystem = System<Renderable, Sprite, PhysicsBody, PhysicsBodyTransform, Transform, TransformMatrix>;
		using MeshBodySystem = System<Renderable, Mesh, PhysicsBody, PhysicsBodyTransform, Transform, TransformMatrix>;

	public:
		RenderSystem(EntityAdmin& entity, LayerType id, Time& time);

	public:
		void SetBatchMode(BatchMode batch_mode);
		void SetBatchingEnabled(bool flag);

		void UpdateStaticBatch();

	public:
		void SetGUIBatchMode(BatchMode batch_mode);
		void SetGUIBatchingEnabled(bool flag);

		void UpdateStaticGUIBatch();

	public:
		void PreUpdate() override;
		void Update() override;
		void PostUpdate() override;

		void Draw(Window& window) const override;
		void DrawGUI(Window& window) const override;

	private:
		void BatchEntity(
			const Renderable& renderable, 
			const IBatchable& batchable, 
			const Mat4f& transform, 
			float depth = 0.0f);

		void BatchBody(
			const Time& time, 
			const Renderable& renderable, 
			const IBatchable& batchable, 
			const PhysicsBody& pb,
			const PhysicsBodyTransform& pbt,
			const Transform& t, 
			const TransformMatrix& tm, 
			float depth = 0.0f);

	private:
		SpriteSystem		m_sprites;
		MeshSystem			m_meshes;

		SpriteBodySystem	m_sprites_bodies;
		MeshBodySystem		m_meshes_bodies;

		SpriteBatch			m_static_batch;
		SpriteBatch			m_dynamic_batch;

		SpriteBatch			m_static_gui_batch;
		SpriteBatch			m_dynamic_gui_batch;

		bool				m_batching_enabled			{true};
		bool				m_update_static_batch		{true};

		bool				m_gui_batching_enabled		{true};
		bool				m_update_static_gui_batch	{true};
	};
}