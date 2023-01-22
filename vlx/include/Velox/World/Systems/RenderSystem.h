#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS.hpp>

#include <Velox/Components/Object.h>
#include <Velox/Components/Sprite.h>
#include <Velox/Components/Transform.h>

#include <Velox/Graphics/SpriteBatch.h>

#include <Velox/Config.hpp>

#include <Velox/World/SystemObject.h>

namespace vlx
{
	class VELOX_API RenderSystem : public SystemObject, public sf::Drawable
	{
	private:
		using System = System<Object, Transform, Sprite>;

	public:
		RenderSystem(EntityAdmin& entity, const LayerType id);

	public:
		void SetBatchMode(const BatchMode batch_mode);
		void SetBatchingEnabled(const bool flag);

		void UpdateStaticBatch();

	public:
		void Update() override;
		void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

	private:
		void PreUpdate();
		void PostUpdate();

	private:
		System				m_system;

		SpriteBatch			m_static_batch;
		SpriteBatch			m_dynamic_batch;

		bool				m_batching_enabled		{true};
		mutable bool		m_update_static_bash	{true};
	};
}