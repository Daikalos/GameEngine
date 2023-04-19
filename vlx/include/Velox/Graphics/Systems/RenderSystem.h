#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/Graphics/Components/Renderable.h>
#include <Velox/Graphics/Components/Sprite.h>
#include <Velox/Graphics/Components/GlobalTransform.h>
#include <Velox/Graphics/Components/Transform.h>

#include <Velox/Algorithms/LQuadTree.hpp>

#include <Velox/Graphics/SpriteBatch.h>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>
#include <Velox/Window.hpp>

namespace vlx
{
	class VELOX_API RenderSystem final : public SystemAction
	{
	private:
		using System = System<Renderable, GlobalTransform, Sprite>;

	public:
		RenderSystem(EntityAdmin& entity, const LayerType id);

	public:
		bool IsRequired() const noexcept override;

	public:
		void SetBatchMode(const BatchMode batch_mode);
		void SetBatchingEnabled(const bool flag);

		void UpdateStaticBatch();

	public:
		void SetGUIBatchMode(const BatchMode batch_mode);
		void SetGUIBatchingEnabled(const bool flag);

		void UpdateStaticGUIBatch();

	public:
		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

		void Draw(Window& window) const;
		void DrawGUI(Window& window) const;

	private:
		void DrawEntity(const Renderable& renderable, const IBatchable& batchable, const Mat4f& transform, const float depth = 0.0f);

	private:
		System		m_render_system;

		SpriteBatch	m_static_batch;
		SpriteBatch	m_dynamic_batch;

		SpriteBatch	m_static_gui_batch;
		SpriteBatch	m_dynamic_gui_batch;

		bool		m_batching_enabled			{true};
		bool		m_update_static_bash		{true};

		bool		m_gui_batching_enabled		{true};
		bool		m_update_static_gui_bash	{true};
	};
}